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

global b32 LeftMouseButton = 0;

internal void program_init();
internal void program_update(Mat4f32 view, Mat4f32 projection);

internal void framebuffer_size_callback(GLFWwindow* window, int width, int height);
internal void process_input(GLFWwindow *window);
internal void keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
internal void mouse_callback(GLFWwindow* window, f64 x_position, f64 y_position);

int main(void) {
	os_init();
  
	Thread_Context main_thread_context;
	thread_context_init_and_attach(&main_thread_context);
  
  program_init();
  game_init();
  input_init();
  
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
  
  glfwSetKeyCallback(window, keyboard_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    Assert(0);
  }
  
	ProgramRenderer = renderer_init(&ProgramState);
  
  while(!glfwWindowShouldClose(window)) {
    hotload_variables(&ProgramState);
    hotload_shader_programs(&ProgramRenderer, ProgramState.current_time);
    
    renderer_begin_frame(&ProgramRenderer, PALLETE_COLOR_D);
    
    camera_update(&ProgramState.camera, ProgramState.delta_time);
    
    //~ Perspective 
    Mat4f32 view = look_at_mat4f32(ProgramState.camera.position, add_vec3f32(ProgramState.camera.position, ProgramState.camera.front), ProgramState.camera.up);
    Mat4f32 projection = perspective_mat4f32(Radians(45), ProgramState.window_width, ProgramState.window_height, ProgramState.near_plane, ProgramState.far_plane);
    
    //~ Updates
    process_input(window);
    program_update(view, projection);
    
    //~ Game logic
    game_update(&ProgramState.camera, ProgramState.raycast);
    
    //~ Render
    renderer_update(GameState, &ProgramRenderer, view, projection);
    
    //~ Input
    input_update();
    
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
  hotload_variables(&ProgramState);
  
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
  
  ProgramState.camera = camera_init();
  
  ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
}

internal void program_update(Mat4f32 view, Mat4f32 projection) {
  ProgramState.current_time = glfwGetTime();
  ProgramState.delta_time   = ProgramState.current_time - ProgramState.last_frame;;
  ProgramState.last_frame   = ProgramState.current_time;
  
  // Update Raycast
  if (ProgramState.camera.mode == CameraMode_Select) {
    f32 mouse_x_ndc = (2.0f * InputState.mouse_current.screen_space_x) / ProgramState.window_width - 1.0f;
    f32 mouse_y_ndc = 1.0f - (2.0f * InputState.mouse_current.screen_space_y) / ProgramState.window_height;
    
    Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(mouse_x_ndc, mouse_y_ndc, 1.0f), projection, view);
    ProgramState.raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(ProgramState.camera.position.x, ProgramState.camera.position.y, ProgramState.camera.position.z)));
  } else {
    ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  }
}


internal void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  ProgramState.window_width  = width;
  ProgramState.window_height = height;
  
  renderer_generate_msaa_and_intermidiate_buffers(&ProgramRenderer);
}

internal void process_input(GLFWwindow *window) {
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
      glfwSetCursorPos(window, InputState.mouse_previous.screen_space_x, InputState.mouse_previous.screen_space_y);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    
  } else {
    if (ProgramState.camera.mode == CameraMode_Fly) {
      ProgramState.camera.mode = CameraMode_Select;
      glfwSetCursorPos(window, InputState.mouse_previous.screen_space_x, InputState.mouse_previous.screen_space_y);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
}

internal void keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
  if (key >= 0 && key < KEYBOARD_STATE_SIZE) {
    b32 is_pressed = (action != GLFW_RELEASE);
    input_process_keyboard_key(key, is_pressed);
  }
}

internal void mouse_callback(GLFWwindow* window, f64 x_position, f64 y_position) {
  local_persist b32 FirstMouse = 1;
  
  if (ProgramState.camera.mode == CameraMode_Fly) {
    f32 xoffset = x_position - InputState.mouse_previous.screen_space_x;
    f32 yoffset = InputState.mouse_previous.screen_space_y - y_position;
    InputState.mouse_current.screen_space_x = x_position;
    InputState.mouse_current.screen_space_y = y_position;
    camera_mouse_callback(&ProgramState.camera, xoffset, yoffset);
  } else {
    InputState.mouse_current.screen_space_x = x_position;
    InputState.mouse_current.screen_space_y = y_position;
  }
}
