/*
[ ] - noted but nothing done about it
[x] - done
[-] - won't do anything about it

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
[x] - Remove renderer_update. Stuff should either belong to the renderer and be in end_frame, or belong to the game logic and be in game_update or higher.
[x] - Be able to select gizmos arrows and panels
 [x] - Gizmos should  actually transform the cube on each axis
[x] - Moving cubes from gizmos must snap to the grid
[x] - Use F keys to switch selected cube colors
[x] - Add way to save and load levels from files
[ ] - Add phong light
[-] - For a selected cube, add a small UI to configure stuff about it (like colors) 
[-] - Add logs to the screen that fade after 1 second or so.
[-] - Add undo system for the add/remove cubes
[-] - Add some sort of post processing shake when loading variables from hotload, just to know it was loaded and feature creep

## BUGS:
[x] - When highlighting a cube, we get more triangles than we should have. We should have just the same 
[x] - Cubes are still being selected (in a weird way) when the camera is in fly mode.
[-] - Font rendering is not taking into account the aspect ratio of the screen
[-] - We should not push cubes into the renderer that are not visible on the frustum
[-] - Game crashes when it gets minimized to tray
[-] - Camera resets to previous position if you are pressing RMB, and then press the LMB without releasing RMB

## F_BASE:
[x] - Add thread context module
[ ] - os_file_load_entire_file should return a bool and output OS_File from out arg
[ ] - Add windows window layer I.e. remove glfw dependency
[ ] - Add a close file function.

## OVERALL NOTES:

[x] - SCRATCH. I will think about it further. Right now, no... Should be better to just typedef Vec4 to a Color. I'm always making the mistake of making a color a Vec3.
[x] - Just have the fucking vec4f32() function take the w. Eventhough its almost always 1.0f, I think its better to write the fucking 1.0f, rather than obfuscate it in the constructor
[x] - Just add the true or false macros... It's even ambiguos dealing with 0 and 1 for bools.
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
  
  GlfwWindow = glfwCreateWindow(ProgramState.window_width, ProgramState.window_height, APP_NAME, NULL, NULL);
	if (GlfwWindow == NULL) { printf("Failed to create GLFW window"); Assert(0); }
  
	glfwMakeContextCurrent(GlfwWindow);
	glfwSetFramebufferSizeCallback(GlfwWindow, framebuffer_size_callback);
  glfwSetKeyCallback(GlfwWindow,             keyboard_callback);
	glfwSetCursorPosCallback(GlfwWindow,       mouse_cursor_callback);
  glfwSetMouseButtonCallback(GlfwWindow,     mouse_button_callback);
  
  ArrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  DragCursor  = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
  glfwSetCursor(GlfwWindow, ArrowCursor);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    Assert(0);
  }
  
  ProgramRenderer = renderer_init(&ProgramState);
  
  //~ Try to load save
  os_file_create(SAVE_FILE_PATH);
  OS_File save_file = os_file_load_entire_file(GameState.arena, SAVE_FILE_PATH);
  if (save_file.size == 0) {
    printf("Unable to load save file.\n");
    
    // Dummy data because no save file!
    for (f32 i = -5.0f; i < 6.0f; i += 2.0f) {
      for (f32 j = -5.0f; j < 6.0f; j += 2.0f) {
        Cube cube = cube_new(vec3f32((f32)i, -1.0f, (f32)j), PALLETE_COLOR_B, 0.05f);
        game_push_cube(cube);
      }
    }
  } else {
    // Load save file data.
    MemoryCopy(&ProgramState.camera, save_file.data, sizeof(Camera));
    s32 offset = sizeof(Camera);
    while (offset < save_file.size) {
      Cube cube;
      MemoryCopy(&cube, save_file.data+offset, sizeof(Cube));
      if (cube.is_dead) {
        offset += sizeof(Cube);
        continue;
      }
      cube.is_selected = 0;
      game_push_cube(cube);
      offset += sizeof(Cube);
    }
  }
  
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
    
    glfwSwapBuffers(GlfwWindow);
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
    case 340: key = 0xA0; break; // LEFT SHIFT
    case 290: key = 0x70; break; // F1
    case 291: key = 0x71; break; // F2
    case 292: key = 0x72; break; // F3
    case 293: key = 0x73; break; // F4
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