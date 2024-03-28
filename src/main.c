#include "main.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

global s32 WindowWidth  = 1280;
global s32 WindowHeight = 720;
#define AspectRatio ((f32)WindowWidth/(f32)WindowHeight)

global Camera camera;
global f32 LastX;
global f32 LastY;

typedef struct MouseState {
	f32 screen_space_x;
	f32 screen_space_y;

	f32 ndc_x;
	f32 ndc_y;

} MouseState;

global MouseState Mouse = { 0 };

global b32 RightMouseButton = 0;
global b32 LeftMouseButton = 0;

global f32 DeltaTime = 0.0f;
global f32 LastFrame = 0.0f;

global u32 SelectedCube = 0;
global Cube Cubes[10];

Vec3f32 intersectLinePlane(Linef32 line, Vec3f32 point1, Vec3f32 point2, Vec3f32 point3) {
		Vec3f32 result = vec3f32(F32_MAX, F32_MAX, F32_MAX);

		Vec3f32 plane_v1 = vec3f32(point2.x - point1.x,
															 point2.y - point1.y,
															 point2.z - point1.z);

		Vec3f32 plane_v2 = vec3f32(point3.x - point1.x,
															 point3.y - point1.y,
															 point3.z - point1.z);

		Vec3f32 plane_normal = vec3f32(plane_v1.y * plane_v2.z - plane_v1.z * plane_v2.y,
																	 plane_v1.z * plane_v2.x - plane_v1.x * plane_v2.z,
																	 plane_v1.x * plane_v2.y - plane_v1.y * plane_v2.x);

		f32 dot_product = dot_vec3f32(line.direction, plane_normal);

		// If the dot product is close to zero, the line is parallel to the plane
		if (fabs(dot_product) < 0.000001f) {
				return result;
		}

		// Calculate the vector from a point on the line to a point on the plane
		Vec3f32 lineToPlane = vec3f32(point1.x - line.point.x,
																	point1.y - line.point.y,
																	point1.z - line.point.z);

		// Calculate the distance along the line to the intersection point
		f32 t = (lineToPlane.x * plane_normal.x +
						 lineToPlane.y * plane_normal.y +
						 lineToPlane.z * plane_normal.z) / dot_product;

		// Calculate the intersection point
		result = vec3f32(line.point.x + t * line.direction.x,
										 line.point.y + t * line.direction.y,
										 line.point.z + t * line.direction.z);

		return result;
}

int main(void) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, APP_NAME, NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	camera = camera_create();
	LastX  = WindowWidth / 2.0f;
	LastY  = WindowHeight / 2.0f;

	Mouse.screen_space_x = LastX;
	Mouse.screen_space_y = LastY;
	Mouse.ndc_x = LastX;
	Mouse.ndc_y = LastY;

	// Cubes -------------
	Cubes[0] = cube_create(vec3f32( 0.0f,  0.0f,  0.0f), vec3f32(1.0f, 0.0f, 0.0f));
	Cubes[1] = cube_create(vec3f32( 0.0f,  0.0f, -5.0f), vec3f32(0.0f, 1.0f, 0.0f));
	Cubes[2] = cube_create(vec3f32( 0.0f, -0.0f,  5.0f), vec3f32(0.0f, 0.0f, 1.0f));
	Cubes[3] = cube_create(vec3f32( 0.0f,  5.0f,  0.0f), vec3f32(1.0f, 0.5f, 0.0f));
	Cubes[4] = cube_create(vec3f32( 0.0f, -5.0f,  0.0f), vec3f32(1.0f, 1.0f, 0.0f));
	Cubes[5] = cube_create(vec3f32( 5.0f,  0.0f,  0.0f), vec3f32(0.5f, 0.5f, 0.5f));
	Cubes[6] = cube_create(vec3f32(-5.0f,  0.0f,  0.0f), vec3f32(1.0f, 0.0f, 1.0f));
	Cubes[7] = cube_create(vec3f32( 5.0f,  5.0f,  5.0f), vec3f32(0.5f, 0.0f, 1.0f));
	Cubes[8] = cube_create(vec3f32(-5.0f, -5.0f, -5.0f), vec3f32(0.0f, 0.0f, 0.0f));
	Cubes[9] = cube_create(vec3f32( 5.0f, -5.0f, -5.0f), vec3f32(0.0f, 1.0f, 1.0f));

	Shader cube_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());
	cube_program_init(cube_program);

	// Far plane --------
	f32 var_plane_vertices[] = {
		// Front face
		-10.f, -10.f, -100.f,  // 0
		 10.f, -10.f, -100.f,  // 1
		 10.f,  10.f, -100.f,  // 2
		-10.f,  10.f, -100.f,  // 3
	};
	u32 far_plane_indices[] = {
		0, 1, 2, 2, 3, 0
	};

	u32 VAO_far, VBO_far, EBO_far;

	glGenVertexArrays(1, &VAO_far);
	glGenBuffers(1, &VBO_far);
	glGenBuffers(1, &EBO_far);

	glBindVertexArray(VAO_far);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_far);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_far);

	glBufferData(GL_ARRAY_BUFFER, sizeof(var_plane_vertices), var_plane_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(far_plane_indices), far_plane_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Axis -------------
	Shader lines_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX());
	f32 lenxyz = 16.0f;
	f32 xyz[] = {
		lenxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, //  X
	 -lenxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, // -X
			0.0f,  lenxyz,    0.0f, 0.0f, 1.0f, 0.0f, //  Y
			0.0f, -lenxyz,    0.0f, 0.0f, 1.0f, 0.0f, // -Y
			0.0f,    0.0f,  lenxyz, 0.0f, 0.0f, 1.0f, //  Z
			0.0f,    0.0f, -lenxyz, 0.0f, 0.0f, 1.0f  // -Z
	};

	u32 VBO_axis, VAO_axis;
	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);

	glBindVertexArray(VAO_axis);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xyz), xyz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)(3* sizeof(f32)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Raycast -------------
	f32 cursor_ray[] = {
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
	};

	u32 VBO_ray, VAO_ray;
	glGenVertexArrays(1, &VAO_ray);
	glGenBuffers(1, &VBO_ray);

	glBindVertexArray(VAO_ray);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_ray);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cursor_ray), cursor_ray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)(3* sizeof(f32)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	while(!glfwWindowShouldClose(window)) {
		f32 currentFrame = (f32)(glfwGetTime());
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		process_input(window);

		glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4f32 projection = mat4f32(1.0f);
		Mat4f32 perspective = perspective_mat4f32(Radians(45), AspectRatio, 0.1f, 100.0f);
		projection = mul_mat4f32(perspective, projection);

		Mat4f32 view = mat4f32(1.0f);
		Mat4f32 look_at = look_at_mat4f32(camera.position, add_vec3f32(camera.position, camera.front), camera.up);
		view = mul_mat4f32(look_at, view);

		// far plane, to test intersections
		shader_use(cube_program);
		{
			glBindVertexArray(VAO_far);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_far);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_far);

			shader_set_uniform_mat4fv(cube_program, "view", view);
			shader_set_uniform_mat4fv(cube_program, "projection", projection);
			shader_set_uniform_mat4fv(cube_program, "model", mat4f32(1.0f));
			shader_set_uniform_vec3fv(cube_program, "color", vec3f32(0.0f, 0.0f, 1.0f));

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);			
		}

		shader_use(lines_program);
		{
			glLineWidth(2.0f);

			glBindVertexArray(VAO_axis);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);

			Mat4f32 model = mat4f32(1.0f);
			shader_set_uniform_mat4fv(lines_program, "model", model);
			shader_set_uniform_mat4fv(lines_program, "view", view);
			shader_set_uniform_mat4fv(lines_program, "projection", projection);

			glDrawArrays(GL_LINES, 0, 6);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		Vec3f32 raycast = { 0 };
		shader_use(lines_program);
		{
			glBindVertexArray(VAO_ray);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_ray);

			glLineWidth(3.0f);

			raycast = unproject_vec3f32(vec3f32(Mouse.ndc_x, Mouse.ndc_y, 1.0f), projection, view);

			if (LeftMouseButton) {
				cursor_ray[0] = camera.position.x;
				cursor_ray[1] = camera.position.y;
				cursor_ray[2] = camera.position.z;

				cursor_ray[6] = raycast.x;
				cursor_ray[7] = raycast.y;
				cursor_ray[8] = raycast.z;
			}
			glBufferData(GL_ARRAY_BUFFER, sizeof(cursor_ray), cursor_ray, GL_STATIC_DRAW);

			Vec3f32 ray_direction = sub_vec3f32(vec3f32(cursor_ray[6], cursor_ray[7], cursor_ray[8]),
																					vec3f32(cursor_ray[0], cursor_ray[1], cursor_ray[2]));

			Vec3f32 far_1 = vec3f32(0.0f, 0.0f, -100.0f);
			Vec3f32 far_2 = vec3f32(5.0f, 5.0f, -100.0f);
			Vec3f32 far_3 = vec3f32(0.0f, 5.0f, -100.0f);
			Vec3f32 intersect = intersectLinePlane(linef32(vec3f32(cursor_ray[0], cursor_ray[1], cursor_ray[2]), ray_direction), far_1, far_2, far_3);
			print_vec3f32(intersect, "Far plane ray intersection:");

			Mat4f32 model = mat4f32(1.0f);
			shader_set_uniform_mat4fv(lines_program, "model", model);
			shader_set_uniform_mat4fv(lines_program, "view", view);
			shader_set_uniform_mat4fv(lines_program, "projection", projection);

			glDrawArrays(GL_LINES, 0, 2);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			Cube cube = cube_create(intersect, vec3f32(1.0f, 0.0f, 0.0f));
			cube_scale(&cube, vec3f32(0.25f, 0.25f, 0.25f));
			cube_program_draw(cube, view, projection);
		}

		shader_use(cube_program);
		{
			glLineWidth(1.0f);

			for(u32 i = 0; i < ArrayCount(Cubes); i++) {
				Cube cube = Cubes[i];
				cube_rotate(&cube, vec3f32(1.0f, 0.3f, 0.5f), (f32)glfwGetTime()*sin(i));
				
				if (SelectedCube == i) {
					glLineWidth(3.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				cube_program_draw(cube, view, projection);

				if (SelectedCube == i) {
					glLineWidth(1.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cube_program_clean();

	glDeleteVertexArrays(1, &VAO_axis);
	glDeleteBuffers(1, &VBO_axis);

	glDeleteVertexArrays(1, &VAO_ray);
	glDeleteBuffers(1, &VBO_ray);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WindowWidth  = width;
	WindowHeight = height;
}

void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}

	local_persist b32 is_tab_down = 0;
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (!is_tab_down) {
			is_tab_down = 1;
			if (SelectedCube + 1 > 9) {
				SelectedCube = 0;
			} else {
				SelectedCube++;
			}
		}
	} else {
		if (is_tab_down) {
			is_tab_down = 0;
		}
	}


	f32 step = 5.0f * DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
		cube_translate(&Cubes[SelectedCube], vec3f32(0.0f, step, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) {
		cube_translate(&Cubes[SelectedCube], vec3f32(0.0f, -step, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
		cube_translate(&Cubes[SelectedCube], vec3f32(-step, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
		cube_translate(&Cubes[SelectedCube], vec3f32(step, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) {
		cube_translate(&Cubes[SelectedCube], vec3f32(0.0f, 0.0f, step));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) {
		cube_translate(&Cubes[SelectedCube], vec3f32(0.0f, 0.0f, -step));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS) {
		cube_scale(&Cubes[SelectedCube], vec3f32(1.0f+step, 1.0f+step, 1.0f+step));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
		cube_scale(&Cubes[SelectedCube], vec3f32(1.0f-step, 1.0f-step, 1.0f-step));
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		LeftMouseButton = 1;
	} else {
		LeftMouseButton = 0;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (RightMouseButton == 0) {
			RightMouseButton = 1;
			LastX = WindowWidth/2;
			LastY = WindowHeight/2;
			glfwSetCursorPos(window, WindowWidth/2, WindowHeight/2);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Front, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Back, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Left, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Right, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Down, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Up, DeltaTime);
		}
	} else {
		if (RightMouseButton == 1) {
			RightMouseButton = 0;
			LastX = WindowWidth/2;
			LastY = WindowHeight/2;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, Mouse.screen_space_x, Mouse.screen_space_y);
		}
	}
}

void mouse_callback(GLFWwindow* window, f64 xposIn, f64 yposIn) {
	local_persist b32 FirstMouse = 1;

	if (RightMouseButton == 1) {
		f32 xpos = (f32)xposIn;
		f32 ypos = (f32)yposIn;

		if (FirstMouse == 1) {
			LastX = xpos;
			LastY = ypos;
			FirstMouse = 0;
		}

		f32 xoffset = xpos - LastX;
		f32 yoffset = LastY - ypos;
		LastX = xpos;
		LastY = ypos;

		camera_mouse_callback(&camera, xoffset, yoffset);
	} else {
			Mouse.screen_space_x = xposIn;
			Mouse.screen_space_y = yposIn;

			Mouse.ndc_x = (2.0f * xposIn) / WindowWidth - 1.0f;
			Mouse.ndc_y = 1.0f - (2.0f * yposIn) / WindowHeight;
	}
}