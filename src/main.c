/*
noname:
[x] - Add texture support to the renderer
[x] - Put any kind of text to the screen
[x] - Try rendering text directly to the screen program and be able to say (x,y) in pixels, where the bottom left of the string goes
[ ] - Add directional light
[ ] - Add phong light
[x] - Add cube to the hovered cube face
[ ] - Delete cubes
[ ] - Add more robust (generic)input system
[ ] - Add way to save and load levels from files
[ ] - Add undo system for the add/remove cubes
[ ] - Be able to select a cube on click
[ ] - Add translation gizmos to selected cube (xyz arrows) and (xy, xz, yz planes), that actually transform the cube each arrow
[ ] - Moving cubes from gizmos must snap to the grid
[ ] - Add some sort of post processing shake when loading variables from hotload, just to know it was loaded and feature creep
[ ] - MAX_TRIANGLES should be in allocated memory instead of a stack allocation
[x] - Add hotloadable stats on top left
[x] - Replace GlobalArena with a either more specific arenas or just thread context scratch arenas
bugs:
[ ] - When highlighting a cube, we get more triangles than we should have. We should have just the same 
f_base:
[x] - Add thread context module
[ ] - Add windows window layer I.e. remove glfw dependency
[ ] - Cubes are still being selected (in a weird way) when the camera is in fly mode.
*/

#include "main.h"

typedef struct Mouse_State {
	f32 screen_space_x;
	f32 screen_space_y;
  
	f32 ndc_x;
	f32 ndc_y;
  
  f32 last_x;
  f32 last_y;
} Mouse_State;

// TODO(fz): Move to game module
typedef struct CubeUnderCursor {
	Cube_Face hovered_face;
	u32 index;
	f32 distance_to_camera;
} CubeUnderCursor;

typedef struct Program_State {
  
  Camera camera;
  Mouse_State mouse;
  Vec3f32 raycast;
  
  s32 window_width;
  s32 window_height;
  b32 show_debug_stats;
  
} Program_State;

global Program_State ProgramState;

internal void program_init(s32 window_width, s32 window_height) {
  AssertNoReentry();
  
  MemoryZeroStruct(&ProgramState);
  
  // Camera
  ProgramState.camera = camera_init();
  
  // Mouse
  ProgramState.mouse.last_x = window_width / 2.0f;
  ProgramState.mouse.last_y = window_height / 2.0f;
	ProgramState.mouse.screen_space_x = ProgramState.mouse.last_x;
	ProgramState.mouse.screen_space_y = ProgramState.mouse.last_y;
	ProgramState.mouse.ndc_x = ProgramState.mouse.last_x;
	ProgramState.mouse.ndc_y = ProgramState.mouse.last_y;
  
  ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  
  ProgramState.window_width  = window_width;
  ProgramState.window_height = window_height;
  ProgramState.show_debug_stats = 1;
}

internal void program_update(Mat4f32 view, Mat4f32 projection) {
  
  // Update Raycast
  if (ProgramState.camera.mode == CameraMode_Select) {
    Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(ProgramState.mouse.ndc_x, ProgramState.mouse.ndc_y, 1.0f), projection, view);
    ProgramState.raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(ProgramState.camera.position.x, ProgramState.camera.position.y, ProgramState.camera.position.z)));
  } else {
    ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  }
  
}

global f32 NearPlane = 0.1f;
global f32 FarPlane = 100.0f;

// Keyboard state
global b32 F_KeyPreviousState = 1;
global b32 F_KeyState = 0;

global b32 G_KeyPreviousState = 1;
global b32 G_KeyState = 0;

global b32 R_KeyPreviousState = 1;
global b32 R_KeyState = 0;
/////////////////

global b32 LeftMouseButton = 0;

// Time
global f64 CurrentTime  = 0.0f;
global f32 DeltaTime = 0.0f;
global f32 LastFrame = 0.0f;
// FPS
global f64 FpsLastTime = 0.0f;
global s64 FrameCount = 0.0f;
global u64 FPS = 0.0f;

Renderer ProgramRenderer;

internal b32 find_cube_under_cursor(CubeUnderCursor* result); // TODO(Fz): Move to game module
internal void framebuffer_size_callback(GLFWwindow* window, int width, int height);
internal void process_input(GLFWwindow *window);
internal void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

global Arena* CubesArena;
global Cube* Cubes;
global u32 TotalCubes = 0;

CubeUnderCursor CurrentCubeUnderCursor;

int main(void) {
	os_init();
	os_file_create(StringLiteral(VARIABLES_TWEAK_FILE));
  
	Thread_Context tctx;
	thread_context_init_and_equip(&tctx);
  
	hotload_variables(&ProgramState.window_width, &ProgramState.window_height, &ProgramState.show_debug_stats);
  
  program_init(1280, 720);
  
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
	GLFWwindow* window = glfwCreateWindow(ProgramState.window_width, ProgramState.window_height, APP_NAME, NULL, NULL);
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
	Cubes = (Cube*)PushArray(CubesArena, Cube, 2048);
  
	ProgramRenderer = renderer_init(ProgramState.window_width, ProgramState.window_height);
  
	Renderer_Font_Info font_info = {0};
	renderer_font_load(&font_info, StringLiteral("D:\\work\\noname\\res\\Karmina.Otf"), 16);
  
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
    
    FrameCount += 1;
    if (CurrentTime - FpsLastTime >= 0.1f) {
      FPS = FrameCount / (CurrentTime - FpsLastTime);
      FrameCount  = 0;
      FpsLastTime = CurrentTime;
    }
    
    
		// View
		Mat4f32 view = mat4f32(1.0f);
		Mat4f32 look_at = look_at_mat4f32(ProgramState.camera.position, add_vec3f32(ProgramState.camera.position, ProgramState.camera.front), ProgramState.camera.up);
		view = mul_mat4f32(look_at, view);
    
		// Projection 
		Mat4f32 projection = mat4f32(1.0f);
		Mat4f32 perspective = perspective_mat4f32(Radians(45), ProgramState.window_width, ProgramState.window_height, NearPlane, FarPlane);
		projection = mul_mat4f32(perspective, projection);
    
    process_input(window);
    program_update(view, projection);
    
		// Hotloading files
		{
			local_persist f64 last_hotload_time = -1;
			if (CurrentTime - last_hotload_time > 1) {
				hotload_variables(&ProgramState.window_width, &ProgramState.window_height, &ProgramState.show_debug_stats);
        hotload_shader_programs(&ProgramRenderer);
				last_hotload_time = CurrentTime;
			}
		}
    
    //~ Game logic
    {
      for(u32 i = 0; i < TotalCubes; i++) {
        if (find_cube_under_cursor(&CurrentCubeUnderCursor)) {
          break;
        }
        if (i == TotalCubes-1) {
          MemoryZeroStruct(&CurrentCubeUnderCursor);
          CurrentCubeUnderCursor.index = U32_MAX;
        }
      }
      
      if (CurrentCubeUnderCursor.index != U32_MAX) {
        Quad face = cube_get_local_space_face_quad(CurrentCubeUnderCursor.hovered_face);
        face = transform_quad(face, Cubes[CurrentCubeUnderCursor.index].transform);
        
        Vec3f32 center = cube_get_center(Cubes[CurrentCubeUnderCursor.index]);
        Vec3f32 direction = sub_vec3f32(quad_get_center(face), center);
        Vec3f32 new_cube_center = add_vec3f32(center, scale_vec3f32(direction, 2.0f));
        
        if (F_KeyState) {
          Cubes[TotalCubes++] = cube_new(new_cube_center, PALLETE_COLOR_B);
        }
      }
    }
    
    //~ Render
		renderer_begin_frame(&ProgramRenderer, PALLETE_COLOR_D);
		{
			renderer_set_uniform_mat4fv(ProgramRenderer.shader_program, "model", mat4f32(1.0f));
			renderer_set_uniform_mat4fv(ProgramRenderer.shader_program, "view", view);
			renderer_set_uniform_mat4fv(ProgramRenderer.shader_program, "projection", projection);
      
			{
        // Axis
        { 
          f32 size = 20.0f;
          renderer_push_arrow(&ProgramRenderer, vec3f32(-size,   0.0f,   0.0f), vec3f32(size,  0.0f,  0.0f), COLOR_RED, 0.5f);
          renderer_push_arrow(&ProgramRenderer, vec3f32(  0.0f, -size,   0.0f), vec3f32( 0.0f, size,  0.0f), COLOR_GREEN, 0.5f);
          renderer_push_arrow(&ProgramRenderer, vec3f32(  0.0f,   0.0f, -size), vec3f32( 0.0f,  0.0f, size), COLOR_BLUE, 0.5f);
        }
        
        f32 highlight_scale = 0.8f;
        if (CurrentCubeUnderCursor.index != U32_MAX) {
          renderer_push_cube_highlight_face(&ProgramRenderer, Cubes[CurrentCubeUnderCursor.index], vec4f32(0.5+0.5*sin(5*CurrentTime), 0.5+0.5*sin(5*CurrentTime), 0.0f), CurrentCubeUnderCursor.hovered_face, vec4f32(Cubes[CurrentCubeUnderCursor.index].color.x * highlight_scale, Cubes[CurrentCubeUnderCursor.index].color.y * highlight_scale, Cubes[CurrentCubeUnderCursor.index].color.z * highlight_scale));
        }
        
        for(u32 i = 0; i < TotalCubes; i++) {
          if (CurrentCubeUnderCursor.index == U32_MAX) {
            if (CurrentCubeUnderCursor.index == i) {
              continue;
            }
          }
          renderer_push_cube(&ProgramRenderer, Cubes[i], COLOR_BLACK);	
        }
        
        // Render text
        {
          if (ProgramState.show_debug_stats) {
            String txt;
            s32 len;
            f32 y_pos = 0.95f;
            
            // NOTE(fz): This is a but of a hack, but let's say this is a temporary thing ok..?
#define AddStat(fmt, tag, ...) do {\
char tag##_buffer[192] = {0}; \
len = stbsp_sprintf(tag##_buffer, fmt, __VA_ARGS__); \
txt.size = (u64)len; \
txt.str  = (u8*)tag##_buffer; \
renderer_push_string(&ProgramRenderer, &font_info, ProgramState.window_width, ProgramState.window_height, txt, vec2f32(-0.998, y_pos), COLOR_YELLOW); \
y_pos -= 0.05f; } while(0); \
            
            AddStat("FPS: %d", fps, FPS);
            AddStat("Ms/Frame: %0.4f", msframe, (f32)DeltaTime/1000);
            AddStat("Triangles Count/Max: %d/%d", trigs, ProgramRenderer.triangle_count, MAX_TRIANGLES);
            AddStat("Cube Count: %d", cubs, TotalCubes-1);
            AddStat("Hovered Cube Index: %d", hovered, (CurrentCubeUnderCursor.index == U32_MAX) ? -1 : CurrentCubeUnderCursor.index);
            
          }
        }
      }
    }
    renderer_end_frame(&ProgramRenderer, ProgramState.window_width, ProgramState.window_height);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  ProgramState.window_width  = width;
  ProgramState.window_height = height;
  
  renderer_generate_msaa_and_intermidiate_buffers(&ProgramRenderer, ProgramState.window_width, ProgramState.window_height);
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
    if (ProgramState.camera.mode == CameraMode_Select) {
      ProgramState.camera.mode = CameraMode_Fly;
      ProgramState.mouse.last_x = ProgramState.window_width/2;
      ProgramState.mouse.last_y = ProgramState.window_height/2;
      glfwSetCursorPos(window, ProgramState.mouse.last_x, ProgramState.mouse.last_y);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Front, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Back, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Left, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Right, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Down, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Up, DeltaTime);
    }
  } else {
    if (ProgramState.camera.mode == CameraMode_Fly) {
      ProgramState.camera.mode = CameraMode_Select;
      ProgramState.mouse.last_x = ProgramState.window_width/2;
      ProgramState.mouse.last_y = ProgramState.window_height/2;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPos(window, ProgramState.mouse.screen_space_x, ProgramState.mouse.screen_space_y);
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
  
  if (ProgramState.camera.mode == CameraMode_Fly) {
    f32 xpos = (f32)xposIn;
    f32 ypos = (f32)yposIn;
    
    if (FirstMouse == 1) {
      ProgramState.mouse.last_x = xpos;
      ProgramState.mouse.last_y = ypos;
      FirstMouse = 0;
    }
    
    f32 xoffset = xpos - ProgramState.mouse.last_x;
    f32 yoffset = ProgramState.mouse.last_y - ypos;
    ProgramState.mouse.last_x = xpos;
    ProgramState.mouse.last_y = ypos;
    
    camera_mouse_callback(&ProgramState.camera, xoffset, yoffset);
  } else {
    ProgramState.mouse.screen_space_x = xposIn;
    ProgramState.mouse.screen_space_y = yposIn;
    
    ProgramState.mouse.ndc_x = (2.0f * xposIn) / ProgramState.window_width - 1.0f;
    ProgramState.mouse.ndc_y = 1.0f - (2.0f * yposIn) / ProgramState.window_height;
  }
}

internal b32 find_cube_under_cursor(CubeUnderCursor* result) {
  b32 match = 0;
  for (u32 i = 0; i < TotalCubes; i++) {
    Cube it = Cubes[i];
    for(u32 j = 0; j < 6; j++) {
      Quad face = transform_quad(cube_get_local_space_face_quad(j), it.transform);
      Vec3f32 intersection = intersect_line_with_plane(linef32(ProgramState.camera.position, ProgramState.raycast), face.p0, face.p1, face.p2);
      if (is_vector_inside_rectangle(intersection, face.p0, face.p1, face.p2)) {
        if (!match) {
          result->hovered_face = j;
          result->index = i;
          result->distance_to_camera = distance_vec3f32(intersection, ProgramState.camera.position);
          match = 1;
        } else {
          f32 distance = distance_vec3f32(intersection, ProgramState.camera.position);
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