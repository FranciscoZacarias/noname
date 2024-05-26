
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
  
  ProgramState.view = mat4f32(1.0f);
  ProgramState.projection = mat4f32(1.0f);
  
  ProgramState.camera = camera_init();
  ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  
  ProgramState.program_is_running = 1;
}

internal void program_update() {
  ProgramState.view = look_at_mat4f32(ProgramState.camera.position, add_vec3f32(ProgramState.camera.position, ProgramState.camera.front), ProgramState.camera.up);
  ProgramState.projection = perspective_mat4f32(Radians(45), ProgramState.window_width, ProgramState.window_height, ProgramState.near_plane, ProgramState.far_plane);
  
  camera_update(&ProgramState.camera, ProgramState.delta_time);
  
  ProgramState.current_time = glfwGetTime();
  ProgramState.delta_time   = ProgramState.current_time - ProgramState.last_frame;;
  ProgramState.last_frame   = ProgramState.current_time;
  
  // Update Raycast
  if (ProgramState.camera.mode == CameraMode_Select) {
    f32 mouse_x_ndc = (2.0f * InputState.mouse_current.screen_space_x) / ProgramState.window_width - 1.0f;
    f32 mouse_y_ndc = 1.0f - (2.0f * InputState.mouse_current.screen_space_y) / ProgramState.window_height;
    
    Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(mouse_x_ndc, mouse_y_ndc, 1.0f), ProgramState.projection, ProgramState.view);
    ProgramState.raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(ProgramState.camera.position.x, ProgramState.camera.position.y, ProgramState.camera.position.z)));
  } else {
    ProgramState.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  }
  
  if (input_is_key_pressed(KeyboardKey_ESCAPE)) {
    printf("Program exited from pressing Escape!\n");
    ProgramState.program_is_running = 0;
  }
}