#include "main.h"

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

global b32 R_KeyPreviousState = 1;
global b32 R_KeyState = 0;
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
global s32 TotalCubes = 0;

typedef struct CubeUnderCursor {
	CubeFace hovered_face;
	u32 index;
	f32 distance_to_camera;
} CubeUnderCursor;

function b32 find_cube_under_cursor(CubeUnderCursor* result);

function void framebuffer_size_callback(GLFWwindow* window, int width, int height);
function void process_input(GLFWwindow *window);
function void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

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

	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f,  0.0f,  0.0f), PALLETE_COLOR_A);
	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f,  0.0f, -8.0f), PALLETE_COLOR_B);
	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f, -0.0f,  8.0f), PALLETE_COLOR_C);
	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f,  8.0f,  0.0f), PALLETE_COLOR_C);
	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f, -8.0f,  0.0f), PALLETE_COLOR_A);
	Cubes[TotalCubes++] = cube_new(vec3f32( 8.0f,  0.0f,  0.0f), PALLETE_COLOR_B);
	Cubes[TotalCubes++] = cube_new(vec3f32(-8.0f,  0.0f,  0.0f), PALLETE_COLOR_C);
	Cubes[TotalCubes++] = cube_new(vec3f32( 8.0f,  8.0f,  8.0f), PALLETE_COLOR_C);
	Cubes[TotalCubes++] = cube_new(vec3f32(-8.0f, -8.0f, -8.0f), PALLETE_COLOR_A);
	Cubes[TotalCubes++] = cube_new(vec3f32( 8.0f, -8.0f, -8.0f), PALLETE_COLOR_B);

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
		Raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(camera.position.x, camera.position.y, camera.position.z)));

		renderer_begin_frame(&renderer, PALLETE_COLOR_D);
		{
			renderer_set_uniform_mat4fv(renderer.shader_program, "view", view);
			renderer_set_uniform_mat4fv(renderer.shader_program, "projection", projection);

			// Axis
			{ 
				f32 size = 20.0f;
				renderer_push_line(&renderer, vec3f32(-size,   0.0f,   0.0f), vec3f32(size,  0.0f,  0.0f), COLOR_RED);
				renderer_push_line(&renderer, vec3f32(  0.0f, -size,   0.0f), vec3f32( 0.0f, size,  0.0f), COLOR_GREEN);
				renderer_push_line(&renderer, vec3f32(  0.0f,   0.0f, -size), vec3f32( 0.0f,  0.0f, size), COLOR_BLUE);
			}

			for(u32 i = 0; i < TotalCubes; i++) {
				renderer_set_uniform_mat4fv(renderer.shader_program, "model", mat4f32(1.0f));
				CubeUnderCursor cuc;
				if (find_cube_under_cursor(&cuc) && cuc.index == i) {
					renderer_push_cube_highlight_face(&renderer, Cubes[i], vec4f32(0.5+0.5*sin(4*glfwGetTime()), 0.5+0.5*sin(4*glfwGetTime()), 0.0f), cuc.hovered_face, scale_vec4f32(Cubes[i].color, 0.80));
				} else {
					renderer_push_cube(&renderer, Cubes[i], COLOR_BLACK);	
				}
			}
		}
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

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			if (R_KeyState == 0 && R_KeyPreviousState == 1) {
				R_KeyPreviousState = 0;
				R_KeyState = 1;
			} else {
				R_KeyState = 0;
			}
		} else {
			R_KeyPreviousState = 1;
			R_KeyState = 0;
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

function b32 find_cube_under_cursor(CubeUnderCursor* result) {
	b32 match = false;
	for (u32 i = 0; i < TotalCubes; i++) {
		Cube it = Cubes[i];
		for(u32 j = 0; j < 6; j++) {
			Quad face = transform_quad(cube_get_local_space_face_quad(j), it.transform);
			Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, Raycast), face.p0, face.p1, face.p2);
			if (is_vector_inside_rectangle(intersection, face.p0, face.p1, face.p2)) {
				if (!match) {
					result->hovered_face = j;
					result->index = i;
					result->distance_to_camera = distance_vec3f32(intersection, camera.position);
					match = true;
				} else {
					f32 distance = distance_vec3f32(intersection, camera.position);
					if (distance < result->distance_to_camera) {
						result->hovered_face = j;
						result->index = i;
						result->distance_to_camera = distance;
					}
				}
			}
		}
	}

	return match;
}