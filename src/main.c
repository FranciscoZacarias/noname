#include "main.h"

global_variable s32 WindowWidth  = 800;
global_variable s32 WindowHeight = 800;
#define aspect_ratio() (WindowWidth/WindowHeight)

global_variable Camera camera;
global_variable f32 LastX;
global_variable f32 LastY;
global_variable b32 FirstMouse = 1;

/* 0 for selecting, 1 for moving*/
global_variable MouseMode = 0;

global_variable f32 DeltaTime = 0.0f;
global_variable f32 LastFrame = 0.0f;

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

	Shader shader = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());

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

	Vec3f32 positions[] = {
		vec3f32( 0.0f,  0.0f,  0.0f),
		vec3f32( 2.0f,  5.0f, -15.0f),
		vec3f32(-1.5f, -2.2f, -2.5f),
		vec3f32(-3.8f, -2.0f, -12.3f),
		vec3f32( 2.4f, -0.4f, -3.5f),
		vec3f32(-1.7f,  3.0f, -7.5f),
		vec3f32( 1.3f, -2.0f, -2.5f),
		vec3f32( 1.5f,  2.0f, -2.5f),
		vec3f32( 1.5f,  0.2f, -1.5f),
		vec3f32(-1.3f,  1.0f, -1.5f)
	};

	Vec3f32 colors[] = {
		vec3f32(1.0f, 0.0f, 0.0f),
		vec3f32(0.0f, 0.0f, 1.0f),
		vec3f32(0.0f, 1.0f, 0.0f),
		vec3f32(1.0f, 1.0f, 0.0f),
		vec3f32(1.0f, 0.0f, 1.0f),
		vec3f32(1.0f, 0.5f, 0.0f),
		vec3f32(0.0f, 1.0f, 0.5f),
		vec3f32(0.5f, 0.5f, 1.0f),
		vec3f32(0.5f, 1.0f, 0.0f),
		vec3f32(0.0f, 1.0f, 1.0f)
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	shader_use(shader);

	Mat4f32 projection = mat4f32_perspective(45.0f, aspect_ratio(), 0.1f, 100.0f);
	shader_set_uniform_mat4fv(shader, "projection", projection);

	while(!glfwWindowShouldClose(window)) {
		f32 currentFrame = (f32)(glfwGetTime());
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		process_input(window);

		Mat4f32 view = mat4f32_look_at(camera.position, add_vec3f32_vec3f32(camera.position, camera.front), camera.up);
		shader_set_uniform_mat4fv(shader, "view", view);

		glClearColor(0.3f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		for(u32 i = 0; i < 10; i++) {
			Mat4f32 model = mat4f32_make_translate(positions[i].x, positions[i].y, positions[i].z);
			model = mat4f32_rotate(model, 1.0f, 0.3f, 0.5f, (f32)glfwGetTime() * (20.0f * i));
			shader_set_uniform_vec3fv(shader, "color", colors[i]);
			shader_set_uniform_mat4fv(shader, "model", model);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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

	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		MouseMode = 0;
		LastX = WindowWidth/2;
		LastY = WindowHeight/2;
		glfwSetCursorPos(window, WindowWidth/2, WindowHeight/2);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		MouseMode = 1;
		LastX = WindowWidth/2;
		LastY = WindowHeight/2;
		glfwSetCursorPos(window, WindowWidth/2, WindowHeight/2);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (MouseMode == 1) {
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
	}

	local_persist GLenum polygon_mode = GL_FILL;
	if (glfwGetKey(window, GLFW_KEY_F1)) {
		polygon_mode = (polygon_mode == GL_FILL) ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
	}
}

void mouse_callback(GLFWwindow* window, f64 xposIn, f64 yposIn) {
	if (MouseMode == 1) {
		f32 xpos = (f32)xposIn;
		f32 ypos = (f32)yposIn;

		if (FirstMouse) {
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