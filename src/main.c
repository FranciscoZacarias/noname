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
[x] - Border thickness should be a cube attribute
[ ] - For a selected cube, add a small UI to configure stuff about it (like colors) 
[ ] - Be able to select a cube on click
[ ] - Add translation gizmos to selected cube (xyz arrows) and (xy, xz, yz planes), that actually transform the cube each arrow
[ ] - Moving cubes from gizmos must snap to the grid
[ ] - Add some sort of post processing shake when loading variables from hotload, just to know it was loaded and feature creep
[ ] - Stuff that is not glfw but core to the program, should be in like a program or core module. Like the Program_State stuff for example.
[x] - Where should Program_State live? (Result: new module, core.h)
bugs:
[x] - When highlighting a cube, we get more triangles than we should have. We should have just the same 
[ ] - Font rendering is not taking into account the aspect ratio of the screen
[ ] - We should not push cubes into the renderer that are not visible on the frustum
[ ] - Cubes are still being selected (in a weird way) when the camera is in fly mode.
[ ] - Game crashes when it gets minimized to tray
f_base:
[x] - Add thread context module
[ ] - Add windows window layer I.e. remove glfw dependency
*/

#include "main.h"

internal void framebuffer_size_callback(GLFWwindow* window, int width, int height);
internal void keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
internal void mouse_cursor_callback(GLFWwindow* window, f64 x_position, f64 y_position);
internal void mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods);

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
	if (window == NULL) { printf("Failed to create GLFW window"); Assert(0); }
  
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window,             keyboard_callback);
	glfwSetCursorPosCallback(window,       mouse_cursor_callback);
  glfwSetMouseButtonCallback(window,     mouse_button_callback);
  
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
    program_update(view, projection);
    
    //~ Game logic
    game_update(&ProgramState.camera, ProgramState.raycast);
    
    //~ Render
    renderer_update(GameState, &ProgramRenderer, view, projection);
    
    //~ NOTE(fz): Not sure if it should be here. but I want all glfw code contained in main
    // while we don't switch to using os for windowing and input/
    if (input_is_key_pressed(KeyboardKey_ESCAPE)) {
      printf("Program exited from pressing Escape!\n");
      glfwSetWindowShouldClose(window, 1);
    }
    
    //~ Input
    // NOTE(fz): Keep at the end! Otherwise the current 
    input_update();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwTerminate();
  return 0;
}

internal void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  ProgramState.window_width  = width;
  ProgramState.window_height = height;
  
  renderer_generate_msaa_and_intermidiate_buffers(&ProgramRenderer);
}

internal void keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
  // NOTE(fz): ESC key in Win32 is 0x1b but for glfw is 256. Just glfw hacks.
  if (key == 256) { 
    key = 0x1B;
  }
  
  if (key >= 0 && key <= KEYBOARD_STATE_SIZE) {
    b32 is_pressed = (action != GLFW_RELEASE);
    input_process_keyboard_key(key, is_pressed);
  }
}

internal void mouse_cursor_callback(GLFWwindow* window, f64 x_position, f64 y_position) {
  input_process_mouse_cursor(x_position, y_position);
}

internal void mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods) {
  
  // NOTE(fz): This is hacky code but because I dont want to call
  // glfw stuff on the input layer. I'd rather just have the mess here.
  // Hopefully to be removed when replaced by native API.
  b32 is_pressed = (action != GLFW_RELEASE);
  if (button == MouseButton_Right && is_pressed) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  
  // NOTE(fz): Lass than 3 as per GLFW define for mouse buttons, 
  // They are respectively 0, 1, 2 for left, right and middle buttons.
  if (button < 3) {
    input_process_mouse_button(button, is_pressed);
  }
}