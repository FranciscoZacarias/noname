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

function Vec3f32 line_plane_intersect(Vec3f32 ray, Vec3f32 planePoint1, Vec3f32 planePoint2, Vec3f32 planePoint3) {
    Vec3f32 planeNormal = cross_vec3f32(vec3f32(planePoint2.x - planePoint1.x, planePoint2.y - planePoint1.y, planePoint2.z - planePoint1.z),
                                       vec3f32(planePoint3.x - planePoint1.x, planePoint3.y - planePoint1.y, planePoint3.z - planePoint1.z));
    f32 dotProductValue = dot_vec3f32(ray, planeNormal);
    
    // Check if the line is parallel to the plane
    if (dotProductValue == 0) {
        printf("The line is parallel to the plane.\n");
        return (Vec3f32){0.0f, 0.0f, 0.0f}; // Return a zero vector indicating no intersection
    }
    
    // Calculate parameter t
    Vec3f32 lineToPlaneVector = {line_start.x - planePoint1.x, line_start.y - planePoint1.y, line_start.z - planePoint1.z};
    f32 t = -dot_vec3f32(lineToPlaneVector, planeNormal) / dotProductValue;
    
    // Calculate intersection point
    Vec3f32 intersectionPoint = { 0 };
    intersectionPoint.x = line_start.x + t * ray.x;
    intersectionPoint.y = line_start.y + t * ray.y;
    intersectionPoint.z = line_start.z + t * ray.z;
    
    return intersectionPoint;
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

	Shader cube_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());
	Shader lines_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX());

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

	u32 VBO_cube, VAO_cube, EBO_cube;
	glGenVertexArrays(1, &VAO_cube);
	glGenBuffers(1, &VBO_cube);
	glGenBuffers(1, &EBO_cube);

	glBindVertexArray(VAO_cube);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_cube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Axis -------------
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

			Mat4f32 model = mat4f32(1.0f);
			shader_set_uniform_mat4fv(lines_program, "model", model);
			shader_set_uniform_mat4fv(lines_program, "view", view);
			shader_set_uniform_mat4fv(lines_program, "projection", projection);

			glDrawArrays(GL_LINES, 0, 2);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		shader_use(cube_program);
		{
			glBindVertexArray(VAO_cube);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_cube);

			glLineWidth(1.0f); // For wireframe

			for(u32 i = 0; i < ArrayCount(Cubes); i++) {
				Cube cube = Cubes[i];
				cube_rotate(&cube, vec3f32(1.0f, 0.3f, 0.5f), (f32)glfwGetTime()*sin(i));
				
				Mat4f32 cube_world_space_transform = mul_mat4f32(view, cube.transform);

				shader_set_uniform_mat4fv(cube_program, "view", view);
				shader_set_uniform_mat4fv(cube_program, "projection", projection);
				shader_set_uniform_mat4fv(cube_program, "model", cube.transform);

				if (SelectedCube == i) {
					glLineWidth(3.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				} else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}

				shader_set_uniform_vec3fv(cube_program, "color", cube.color);

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glLineWidth(1.0f);
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_cube);
	glDeleteBuffers(1, &EBO_cube);

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