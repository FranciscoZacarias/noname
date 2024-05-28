/*

## noname:
[x] - Add texture support to the renderer
[x] - Put any kind of text to the screen
[x] - Try rendering text directly to the screen program and be able to say (x,y) in pixels, where the bottom left of the string goes
[x] - Add cube to the hovered cube face
[x] - Add hotloadable stats on top left
[x] - Replace GlobalArena with a either more specific arenas or just thread context scratch arenas
[x] - Delete cubes
[x] - MAX_TRIANGLES should be in allocated memory instead of a stack allocation
[x] - Border thickness should be a cube attribute
[x] - Add more robust (generic)input system
[x] - Be able to select a cube on click
[x] - Be able to select multiple cubes
[x] - Add translation gizmos to selected cube (xyz arrows) and (xy, xz, yz planes)
[x] - Where should Program_State live? (Result: new module, core.h)
[x] - We need a way to specify that some triangles will just be rendered in front of everything else (gizmos, for exmaple).
[x] - Stuff that is not glfw but core to the program, should be in like a program or core module. Like the Program_State stuff for example.
[ ] - Add directional light
[ ] - Add phong light
[ ] - Add logs to the screen that fade after 1 second or so.
[ ] - Add way to save and load levels from files
[ ] - Add undo system for the add/remove cubes
[ ] - For a selected cube, add a small UI to configure stuff about it (like colors) 
 [ ] - Gizmos should  actually transform the cube on each axis
[ ] - Moving cubes from gizmos must snap to the grid
[ ] - Add some sort of post processing shake when loading variables from hotload, just to know it was loaded and feature creep
[ ] - Remove renderer_update. Stuff should either belong to the renderer and be in end_frame, or belong to the game logic and be in game_update or higher.

## BUGS:
[x] - When highlighting a cube, we get more triangles than we should have. We should have just the same 
[x] - Cubes are still being selected (in a weird way) when the camera is in fly mode.
[ ] - Font rendering is not taking into account the aspect ratio of the screen
[ ] - We should not push cubes into the renderer that are not visible on the frustum
[ ] - Game crashes when it gets minimized to tray
[ ] - Camera resets to previous position if you are pressing RMB, and then press the LMB without releasing RMB

## F_BASE:
[x] - Add thread context module
[ ] - Add windows window layer I.e. remove glfw dependency
[ ] - Implement get memory usage in os_layer (for win32 should be like GetProcessMemoryInfo(), for example)

## OVERALL NOTES:

- Should be better to just typedef Vec4 to a Color. I'm always making the mistake of making a color a Vec3.
- Just have the fucking vec4f32() function take the w. Eventhough its almost always 1.0f, I think its better to write the fucking 1.0f, rather than obfuscate it in the constructor
- Just add the true or false macros... It's even ambiguos dealing with 0 and 1 for bools.

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
  
  while (ProgramState.program_is_running) {
    hotload_variables(&ProgramState);
    hotload_shader_programs(&ProgramRenderer, ProgramState.current_time);
    
    renderer_begin_frame(&ProgramRenderer, PALLETE_COLOR_D);
    
    //~ Updates
    program_update();
    
    //~ Game logic
    game_update(&ProgramState.camera, ProgramState.raycast);
    
    //~ Render
    renderer_end_frame(&ProgramRenderer, ProgramState.view, ProgramState.projection);
    
    //~ Input NOTE(fz): Keep at the end!
    input_update();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  return 0;
}

internal void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  ProgramState.window_width  = width;
  ProgramState.window_height = height;
  
  renderer_generate_msaa_and_intermidiate_buffers(&ProgramRenderer);
}

internal void keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
  // NOTE(fz): Some key codes form GLFW don't map directly to Win32 VKs...
  // So these are just quick hacks to make it work.
  // TODO(fz): We really need to get rid of glfw
  switch (key) {
    case 256: key = 0x1B; break; // ESC
    case 258: key = 0x09; break; // TAB
    case 341: key = 0xA2; break; // LEFT CONTROL
    case 261: key = 0x2E; break; // DELETE
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