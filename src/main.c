/*
noname:
[x] - Add texture support to the renderer
[ ] - Put any kind of text to the screen
[ ] - Add directional light
[ ] - Add phong light
[ ] - Add cube to the hovered cube face
[ ] - Delete cubes
[ ] - Add more robust input system
[ ] - Add way to save and load levels from files
[ ] - Add undo system for the add/remove cubes
[ ] - Be able to select a cube on click
[ ] - Add translation gizmos to selected cube (xyz arrows) and (xy, xz, yz planes), that actually transform the cube each arrow
[ ] - Moving cubes from gizmos must snap to the grid
[ ] - Add some sort of post processing shake when loading variables from hotload, just to know it was loaded and feature creep
[ ] - MAX_TRIANGLES should be in allocated memory instead of a stack allocation
[x] - Replace GlobalArena with a either more specific arenas or just thread context scratch arenas
f_base:
[x] - Add thread context module
[ ] - Add windows window layer I.e. remove glfw dependency
*/

#include "main.h"

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

typedef struct CubeUnderCursor {
	Cube_Face hovered_face;
	u32 index;
	f32 distance_to_camera;
} CubeUnderCursor;

Renderer ProgramRenderer;

internal b32 find_cube_under_cursor(CubeUnderCursor* result);
internal void framebuffer_size_callback(GLFWwindow* window, int width, int height);
internal void process_input(GLFWwindow *window);
internal void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

global Arena* CubesArena;
global Cube* Cubes;
global u32 TotalCubes = 0;

int main(void) {
	os_init();
	os_file_create(StringLiteral(VARIABLES_TWEAK_FILE));
  
	Thread_Context tctx;
	thread_context_init_and_equip(&tctx);
  
	hotload_variables();
  
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, APP_NAME, NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		Assert(0);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
	glfwSetCursorPosCallback(window, mouse_callback);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    Assert(0);
  }
  
	CubesArena  = arena_init();
	Cubes = (Cube*)PushArray(CubesArena, Cube, 1024);
  
	// Camera and Mouse -----------
	camera = camera_create();
	LastX  = WindowWidth / 2.0f;
	LastY  = WindowHeight / 2.0f;
  
	Mouse.screen_space_x = LastX;
	Mouse.screen_space_y = LastY;
	Mouse.ndc_x = LastX;
	Mouse.ndc_y = LastY;
  
	ProgramRenderer = renderer_init(WindowWidth, WindowHeight);
	u32 pepper = renderer_texture_load(StringLiteral("D:\\work\\noname\\res\\pepper.png"));
  
	Renderer_Font_Info font_info = {0};
	renderer_font_load(&font_info, StringLiteral("D:\\work\\noname\\res\\Inconsolata.ttf"), 32);
  
	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f,  0.0f,  0.0f), PALLETE_COLOR_A);
	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f,  0.0f,  0.0f), PALLETE_COLOR_A);
	Cubes[TotalCubes++] = cube_new(vec3f32( 0.0f,  0.0f, -8.0f), PALLETE_COLOR_B);
	Cubes[TotalCubes++] = cube_new(vec3f32( 2.0f,  0.0f, -8.0f), PALLETE_COLOR_B);
	Cubes[TotalCubes++] = cube_new(vec3f32( 4.0f,  2.0f, -8.0f), PALLETE_COLOR_B);
	Cubes[TotalCubes++] = cube_new(vec3f32( 6.0f,  0.0f, -8.0f), PALLETE_COLOR_B);
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
		CurrentTime = glfwGetTime();
		DeltaTime = CurrentTime - LastFrame;
		LastFrame = CurrentTime;
    
		process_input(window);
    
		// Hotloading files
		{
			local_persist f64 last_hotload_time = -1;
			if (CurrentTime - last_hotload_time > 1) {
				hotload_variables();
				hotload_shader_programs(&ProgramRenderer);
				last_hotload_time = CurrentTime;
			}
		}
    
		// View
		Mat4f32 view = mat4f32(1.0f);
		Mat4f32 look_at = look_at_mat4f32(camera.position, add_vec3f32(camera.position, camera.front), camera.up);
		view = mul_mat4f32(look_at, view);
    
		// Projection 
		Mat4f32 projection = mat4f32(1.0f);
		Mat4f32 perspective = perspective_mat4f32(Radians(45), WindowWidth, WindowHeight, NearPlane, FarPlane);
		projection = mul_mat4f32(perspective, projection);
    
		// Raycast
		if (ActiveCameraMode == CameraMode_Select) {
			Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(Mouse.ndc_x, Mouse.ndc_y, 1.0f), projection, view);
			Raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(camera.position.x, camera.position.y, camera.position.z)));
		} else {
			Raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
		}
    
		Mat4f32 r = rotate_axis_mat4f32(vec3f32(-1.0f, 1.0f, 1.0f), CurrentTime);
		Mat4f32 t = translate_mat4f32(3.0f, 3.0f, 3.0f);
		Mat4f32 trans = mul_mat4f32(t, r);
		trans = mul_mat4f32(r, trans);
		Cubes[0].transform = trans;
    
		r = rotate_axis_mat4f32(vec3f32(1.0f, -1.0f, 1.0f), CurrentTime);
		t = translate_mat4f32(-3.0f, 3.0f, 3.0f);
		trans = mul_mat4f32(t, r);
		Cubes[1].transform = trans;
    
		renderer_begin_frame(&ProgramRenderer, PALLETE_COLOR_D);
		{
			renderer_set_uniform_mat4fv(ProgramRenderer.shader_program, "model", mat4f32(1.0f));
			renderer_set_uniform_mat4fv(ProgramRenderer.shader_program, "view", view);
			renderer_set_uniform_mat4fv(ProgramRenderer.shader_program, "projection", projection);
      
			renderer_push_arrow(&ProgramRenderer, vec3f32(0.0f, 0.0f, 0.0f), scale_vec3f32(vec3f32(Cubes[0].transform.m12, Cubes[0].transform.m13, Cubes[0].transform.m14), 0.5), vec4f32(0.0f, 1.0f, 1.0f), 0.5f);
			renderer_push_arrow(&ProgramRenderer, vec3f32(0.0f, 0.0f, 0.0f), scale_vec3f32(vec3f32(Cubes[1].transform.m12, Cubes[1].transform.m13, Cubes[1].transform.m14), 0.5), vec4f32(1.0f, 1.0f, 0.0f), 0.5f);
      
			// Axis
			{ 
				f32 size = 20.0f;
				renderer_push_arrow(&ProgramRenderer, vec3f32(-size,   0.0f,   0.0f), vec3f32(size,  0.0f,  0.0f), COLOR_RED, 0.5f);
				renderer_push_arrow(&ProgramRenderer, vec3f32(  0.0f, -size,   0.0f), vec3f32( 0.0f, size,  0.0f), COLOR_GREEN, 0.5f);
				renderer_push_arrow(&ProgramRenderer, vec3f32(  0.0f,   0.0f, -size), vec3f32( 0.0f,  0.0f, size), COLOR_BLUE, 0.5f);
			}
      
			// Render cubes and highlight if cursor on top.
			{
				for(u32 i = 0; i < TotalCubes; i++) {
					CubeUnderCursor cuc;
					if (find_cube_under_cursor(&cuc) && cuc.index == i) {
						f32 highlight_scale = 0.8f;
						renderer_push_cube_highlight_face(&ProgramRenderer, Cubes[i], vec4f32(0.5+0.5*sin(5*CurrentTime), 0.5+0.5*sin(5*CurrentTime), 0.0f), cuc.hovered_face, vec4f32(Cubes[i].color.x * highlight_scale, Cubes[i].color.y * highlight_scale, Cubes[i].color.z * highlight_scale));
					} else {
						renderer_push_cube(&ProgramRenderer, Cubes[i], COLOR_BLACK);	
					}
				}
			}
      
			// Render stuff with textures.
			{
			}
      
			// Render text
			{
				
			}
		}
		renderer_end_frame(&ProgramRenderer, WindowWidth, WindowHeight);
		
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
  
	renderer_generate_msaa_and_intermidiate_buffers(&ProgramRenderer, WindowWidth, WindowHeight);
}

void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		printf("Program exited from pressing Escape!\n");
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

internal b32 find_cube_under_cursor(CubeUnderCursor* result) {
	b32 match = 0;
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
					match = 1;
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