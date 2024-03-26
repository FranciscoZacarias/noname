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

global b32 RightMouseButton = 0;

global f32 DeltaTime = 0.0f;
global f32 LastFrame = 0.0f;

global u32 SelectedCube = 0;
global Cube Cubes[10];

int main() {

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

	Shader cube_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());
	Shader axis_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_COLOR_FROM_VERTEX());

	
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

	f32 vertices[] = {
		// Front face
		-0.5f, -0.5f,  0.5f,  // 0
		 0.5f, -0.5f,  0.5f,  // 1
		 0.5f,  0.5f,  0.5f,  // 2
		-0.5f,  0.5f,  0.5f,  // 3
		// Back face
		-0.5f, -0.5f, -0.5f,  // 4
		 0.5f, -0.5f, -0.5f,  // 5
		 0.5f,  0.5f, -0.5f,  // 6
		-0.5f,  0.5f, -0.5f   // 7
	};

	u32 indices[] = {
		// Front face
		0, 1, 2,
		2, 3, 0,
		// Right face
		1, 5, 6,
		6, 2, 1,
		// Back face
		5, 4, 7,
		7, 6, 5,
		// Left face
		4, 0, 3,
		3, 7, 4,
		// Top face
		3, 2, 6,
		6, 7, 3,
		// Bottom face
		4, 5, 1,
		1, 0, 4
	};

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

	f32 lenxyz = 16.0f;
	f32 xyz[] = {
	  lenxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, //  X
	 -lenxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, // -X
		  0.0f,  lenxyz,    0.0f, 0.0f, 1.0f, 0.0f, //  Y
		  0.0f, -lenxyz,    0.0f, 0.0f, 1.0f, 0.0f, // -Y
		  0.0f,    0.0f,  lenxyz, 0.0f, 0.0f, 1.0f, //  Z
		  0.0f,    0.0f, -lenxyz, 0.0f, 0.0f, 1.0f  // -Z
	};

	u32 VBO_axis, VAO_axis, EBO_axis;
	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);
	glGenBuffers(1, &EBO_axis);

	glBindVertexArray(VAO_axis);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xyz), xyz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_False, 6 * sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	while(!glfwWindowShouldClose(window)) {
		f32 currentFrame = (f32)(glfwGetTime());
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		process_input(window);

		Mat4f32 projection = mat4f32(1.0f);
		Mat4f32 perspective = perspective_mat4f32(Radians(45), AspectRatio, 0.1f, 100.0f);
		projection = mul_mat4f32(perspective, projection);

		Mat4f32 view = mat4f32(1.0f);
		Mat4f32 look_at = look_at_mat4f32(camera.position, add_vec3f32(camera.position, camera.front), camera.up);
		view = mul_mat4f32(look_at, view);

		shader_use(axis_program);
		{
			glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glLineWidth(2.0f);

			glBindVertexArray(VAO_axis);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);

			Mat4f32 model = mat4f32(1.0f);
			shader_set_uniform_mat4fv(axis_program, "model", model);
			shader_set_uniform_mat4fv(axis_program, "view", view);
			shader_set_uniform_mat4fv(axis_program, "projection", projection);

			glDrawArrays(GL_LINES, 0, 6);

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

				shader_set_uniform_mat4fv(cube_program, "view", view);
				shader_set_uniform_mat4fv(cube_program, "projection", projection);
				shader_set_uniform_mat4fv(cube_program, "model", cube.transform);

				if (SelectedCube == i) {
					glLineWidth(3.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				} else {
					glLineWidth(1.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}

				shader_set_uniform_vec3fv(cube_program, "color", cube.color);

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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


	f32 step = 0.1f;
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
		cube_scale(&Cubes[SelectedCube], vec3f32(1.05, 1.05, 1.05));
	}
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
		cube_scale(&Cubes[SelectedCube], vec3f32(0.95, 0.95, 0.95));
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
			glfwSetCursorPos(window, WindowWidth/2, WindowHeight/2);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
	}
}