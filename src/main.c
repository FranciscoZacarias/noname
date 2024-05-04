#include "main.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

global s32 WindowWidth  = 1280;
global s32 WindowHeight = 720;
#define AspectRatio ((f32)WindowWidth/(f32)WindowHeight)

global f32 NearPlane = 0.1f;
global f32 FarPlane = 100.0f;

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

// Keyboard state
global b32 F_KeyPreviousState = 1;
global b32 F_KeyState = 0;

global b32 G_KeyPreviousState = 1;
global b32 G_KeyState = 0;
/////////////////

typedef enum CameraMode {
	CameraMode_Select,
	CameraMode_Fly
} CameraMode;

global CameraMode ActiveCameraMode = CameraMode_Select;
global b32 LeftMouseButton = 0;

global f32 DeltaTime = 0.0f;
global f32 LastFrame = 0.0f;

f64 PreviousTime = 0.0f;
f64 CurrentTime  = 0.0f;
u32 FrameCounter;

global Vec3f32 Raycast = {F32_MAX, F32_MAX, F32_MAX};

#define MAX_CUBES 1024
global Cube Cubes[MAX_CUBES];
global s32 LevelCubesIndex = 0;

function u32 find_cube_under_cursor() {
	u32 result = -1;
	for (u32 i = 0; i < LevelCubesIndex; i++) {
		CubeVertices vertices  = cube_vertices_apply_transform(CubeVerticesLocalSpace, Cubes[i].transform);
		
		// Back face
		{
			Quad quad = cube_vertices_get_quad_back(vertices);
			Vec3f32 intersect_back = intersect_line_with_plane(linef32(camera.position, Raycast), quad.p0, quad.p1, quad.p2);
			if (is_vector_inside_rectangle(intersect_back, quad.p0, quad.p1, quad.p2)) {
				printf("Back Face hit\n");
			}
		}

		// Front face
		{
			Quad quad = cube_vertices_get_quad_front(vertices);
			Vec3f32 intersect_front = intersect_line_with_plane(linef32(camera.position, Raycast), quad.p0, quad.p1, quad.p2);
			if (is_vector_inside_rectangle(intersect_front, quad.p0, quad.p1, quad.p2)) {
				printf("Front Face hit\n");
			}
		}

		// Left face
		{
			Quad quad = cube_vertices_get_quad_left(vertices);
			Vec3f32 intersect_front = intersect_line_with_plane(linef32(camera.position, Raycast), quad.p0, quad.p1, quad.p2);
			if (is_vector_inside_rectangle(intersect_front, quad.p0, quad.p1, quad.p2)) {
				printf("Left Face hit\n");
			}
		}

		// Right face
		{
			Quad quad = cube_vertices_get_quad_right(vertices);
			Vec3f32 intersect_front = intersect_line_with_plane(linef32(camera.position, Raycast), quad.p0, quad.p1, quad.p2);
			if (is_vector_inside_rectangle(intersect_front, quad.p0, quad.p1, quad.p2)) {
				printf("Right Face hit\n");
			}
		}

		// Bot face
		{
			Quad quad = cube_vertices_get_quad_bot(vertices);
			Vec3f32 intersect_front = intersect_line_with_plane(linef32(camera.position, Raycast), quad.p0, quad.p1, quad.p2);
			if (is_vector_inside_rectangle(intersect_front, quad.p0, quad.p1, quad.p2)) {
				printf("Bot Face hit\n");
			}
		}

		// Top face
		{
			Quad quad = cube_vertices_get_quad_top(vertices);
			Vec3f32 intersect_front = intersect_line_with_plane(linef32(camera.position, Raycast), quad.p0, quad.p1, quad.p2);
			if (is_vector_inside_rectangle(intersect_front, quad.p0, quad.p1, quad.p2)) {
				printf("Top Face hit\n");
			}
		}

	}
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

	// Camera and Mouse -----------
	camera = camera_create();
	LastX  = WindowWidth / 2.0f;
	LastY  = WindowHeight / 2.0f;

	Mouse.screen_space_x = LastX;
	Mouse.screen_space_y = LastY;
	Mouse.ndc_x = LastX;
	Mouse.ndc_y = LastY;

	Renderer renderer = renderer_init(WindowWidth, WindowHeight);

	Cubes[LevelCubesIndex++] = cube_new(vec3f32(0.0f, 0.0f, 0.0f), PALLETE_COLOR_B);

	while(!glfwWindowShouldClose(window)) {
		f64 currentFrame = (f64)(glfwGetTime());
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		CurrentTime   = glfwGetTime();
		f64 time_diff = CurrentTime - PreviousTime;
		FrameCounter++;
		if (time_diff >= (1.0f/30.0f)) {
			u32 fps = (1.0f / time_diff) * FrameCounter;
			f32 ms = (time_diff / FrameCounter) * 1000;
			char window_title[64];
			sprintf(window_title, "noname - %u FPS / %.2f ms", fps, ms);
			glfwSetWindowTitle(window, window_title);
		}

		process_input(window);

		// View
		Mat4f32 view = mat4f32(1.0f);
		Mat4f32 look_at = look_at_mat4f32(camera.position, add_vec3f32(camera.position, camera.front), camera.up);
		view = mul_mat4f32(look_at, view);

		// Projection 
		Mat4f32 projection = mat4f32(1.0f);
		Mat4f32 perspective = perspective_mat4f32(Radians(45), WindowWidth, WindowHeight, NearPlane, FarPlane);
		projection = mul_mat4f32(perspective, projection);

		// Raycast
		Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(Mouse.ndc_x, Mouse.ndc_y, 1.0f), projection, view);
		Raycast = sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(camera.position.x, camera.position.y, camera.position.z));

		renderer_begin_frame(&renderer, PALLETE_COLOR_D);

		Mat4f32 model = mat4f32(1.0f);
		renderer_set_uniform_mat4fv(renderer.shader_program, "model", model);
		renderer_set_uniform_mat4fv(renderer.shader_program, "view", view);
		renderer_set_uniform_mat4fv(renderer.shader_program, "projection", projection);

		for (u32 i = 0; i < LevelCubesIndex; i++) {
			renderer_push_cube(&renderer, Cubes[i], COLOR_BLACK);
		}

		find_cube_under_cursor();
		renderer_end_frame(&renderer, WindowWidth, WindowHeight);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	renderer_free(&renderer);
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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		LeftMouseButton = 1;
	} else {
		LeftMouseButton = 0;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (ActiveCameraMode == CameraMode_Select) {
			ActiveCameraMode = CameraMode_Fly;
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
		if (ActiveCameraMode == CameraMode_Fly) {
			ActiveCameraMode = CameraMode_Select;
			LastX = WindowWidth/2;
			LastY = WindowHeight/2;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, Mouse.screen_space_x, Mouse.screen_space_y);
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			if (G_KeyState == 0 && G_KeyPreviousState == 1) {
				G_KeyPreviousState = 0;
				G_KeyState = 1;
			} else {
				G_KeyState = 0;
			}
		} else {
			G_KeyPreviousState = 1;
			G_KeyState = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			if (F_KeyState == 0 && F_KeyPreviousState == 1) {
				F_KeyPreviousState = 0;
				F_KeyState = 1;
			} else {
				F_KeyState = 0;
			}
		} else {
			F_KeyPreviousState = 1;
			F_KeyState = 0;
		}
	}
}

void mouse_callback(GLFWwindow* window, f64 xposIn, f64 yposIn) {
	local_persist b32 FirstMouse = 1;

	if (ActiveCameraMode == CameraMode_Fly) {
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