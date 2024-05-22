/*
noname:
[x] - Add texture support to the renderer
[x] - Put any kind of text to the screen
[x] - Try rendering text directly to the screen program and be able to say (x,y) in pixels, where the bottom left of the string goes
[x] - Add cube to the hovered cube face
[x] - Add hotloadable stats on top left
[x] - Replace GlobalArena with a either more specific arenas or just thread context scratch arenas
[x] - Delete cubes
[x] - MAX_TRIANGLES should be in allocated memory instead of a stack allocation
[ ] - Add directional light
[ ] - Add phong light
[ ] - Add logs to the screen that fade after 1 second or so.
[ ] - Add more robust (generic)input system
[ ] - Add way to save and load levels from files
[ ] - Add undo system for the add/remove cubes
[ ] - For a selected cube, add a small UI to configure stuff about it (like colors) 
[ ] - Be able to select a cube on click
[ ] - Add translation gizmos to selected cube (xyz arrows) and (xy, xz, yz planes), that actually transform the cube each arrow
[ ] - Moving cubes from gizmos must snap to the grid
[ ] - Add some sort of post processing shake when loading variables from hotload, just to know it was loaded and feature creep
[ ] - Where should Program_State live?
bugs:
[x] - When highlighting a cube, we get more triangles than we should have. We should have just the same 
[ ] - Font rendering is not taking into account the aspect ratio of the screen
[ ] - We should not push cubes into the renderer that are not visible on the frustum
[ ] - Cubes are still being selected (in a weird way) when the camera is in fly mode.
f_base:
[x] - Add thread context module
[ ] - Add windows window layer I.e. remove glfw dependency
*/

#include "main.h"

global Program_State ProgramState;

//~ Keyboard state
global b32 F_KeyPreviousState = 1;
global b32 F_KeyState = 0;
global b32 G_KeyPreviousState = 1;
global b32 G_KeyState = 0;
global b32 R_KeyPreviousState = 1;
global b32 R_KeyState = 0;

global b32 LeftMouseButton = 0;

internal void program_init();
internal void program_update(Mat4f32 view, Mat4f32 projection);

internal void framebuffer_size_callback(GLFWwindow* window, int width, int height);
internal void process_input(GLFWwindow *window);
internal void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

int main(void) {
	os_init();
  
	Thread_Context main_thread_context;
	thread_context_init_and_equip(&main_thread_context);
  
  program_init();
  
  game_init();
  
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
  
	ProgramRenderer = renderer_init(ProgramState.window_width, ProgramState.window_height);
  
  while(!glfwWindowShouldClose(window)) {
    //~ Perspective 
    Mat4f32 view = look_at_mat4f32(ProgramState.camera.position, add_vec3f32(ProgramState.camera.position, ProgramState.camera.front), ProgramState.camera.up);
    Mat4f32 projection = perspective_mat4f32(Radians(45), ProgramState.window_width, ProgramState.window_height, ProgramState.near_plane, ProgramState.far_plane);
    
    //~ Updates
    process_input(window);
    program_update(view, projection);
    
    hotload_variables(&ProgramState.window_width, &ProgramState.window_height, &ProgramState.show_debug_stats, ProgramState.current_time);
    hotload_shader_programs(&ProgramRenderer, ProgramState.current_time);
    
    //~ Game logic
    game_update(&ProgramState.camera, ProgramState.raycast, F_KeyState, G_KeyState);
    
    //~ Render
    renderer_update(ProgramState, GameState, &ProgramRenderer, view, projection);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwTerminate();
  return 0;
}


internal void program_init() {
  AssertNoReentry();
  
  MemoryZeroStruct(&ProgramState);
  
  os_file_create(StringLiteral(VARIABLES_TWEAK_FILE));
  // NOTE(fz): CurrentTime in this call is set to max to make sure we load it immediately.
  hotload_variables(&ProgramState.window_width, &ProgramState.window_height, &ProgramState.show_debug_stats, F64_MAX);
  
  if (ProgramState.window_width == 0 || ProgramState.window_height == 0) {
    ProgramState.window_width  = 1280;
    ProgramState.window_height = 720;
  }
  
  ProgramState.current_time = 0.0f;
  ProgramState.delta_time   = 0.0f;
  ProgramState.last_frame   = 0.0f;
  
  ProgramState.show_debug_stats = 1;
  ProgramState.near_plane = 0.1f;
  ProgramState.far_plane  = 100.f;
  
  // Camera
  ProgramState.camera = camera_init();
  
  // Mouse
  ProgramState.mouse.last_x = ProgramState.window_width / 2.0f;
  ProgramState.mouse.last_y = ProgramState.window_height / 2.0f;
  ProgramState.mouse.screen_space_x = ProgramState.mouse.last_x;
  ProgramState.mouse.screen_space_y = ProgramState.mouse.last_y;
  ProgramState.mouse.ndc_x = ProgramState.mouse.last_x;
  ProgramState.mouse.ndc_y = ProgramState.mouse.last_y;
  
  ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
}

internal void program_update(Mat4f32 view, Mat4f32 projection) {
  ProgramState.current_time = glfwGetTime();
  ProgramState.delta_time   = ProgramState.current_time - ProgramState.last_frame;;
  ProgramState.last_frame   = ProgramState.current_time;
  
  // Update Raycast
  if (ProgramState.camera.mode == CameraMode_Select) {
    Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(ProgramState.mouse.ndc_x, ProgramState.mouse.ndc_y, 1.0f), projection, view);
    ProgramState.raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(ProgramState.camera.position.x, ProgramState.camera.position.y, ProgramState.camera.position.z)));
  } else {
    ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  }
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
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Front, ProgramState.delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Back, ProgramState.delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Left, ProgramState.delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Right, ProgramState.delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Down, ProgramState.delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      camera_keyboard_callback(&ProgramState.camera, CameraMovement_Up, ProgramState.delta_time);
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
