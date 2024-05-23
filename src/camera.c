internal Camera camera_init() {
	Camera camera;
	camera.position = vec3f32(-9.21f, 5.45f, 14.81f);
	camera.front = vec3f32(0.56f, -0.31f, -0.77f);
	camera.up = vec3f32(0.18f, 0.95f, -0.25f);
	camera.right = vec3f32(0.81f, -0.00f, 0.58f);
	camera.pitch = -18.10f;
	camera.yaw = -54.20f;
  camera.mode = CameraMode_Select;
  
  _camera_update(&camera);
  return camera;
}

internal void print_camera(Camera camera) {
  print_vec3f32(camera.position, "camera.position =");
  print_vec3f32(camera.front, "camera.front =");
  print_vec3f32(camera.up, "camera.up =");
  print_vec3f32(camera.right, "camera.right =");
  printf("camera.pitch = %.2ff;\ncamera.yaw = %.2ff;\n-------------\n", camera.pitch, camera.yaw);
}

internal void camera_update(Camera* camera, f32 delta_time) {
  if (input_is_button_down(MouseButton_Right)) {
    camera->mode = CameraMode_Fly;
    f32 camera_speed = (f32)(HotloadableCameraSpeed * delta_time);
    
    if (input_is_key_down(KeyboardKey_W)) {
      Vec3f32 delta = scale_vec3f32(camera->front, camera_speed);
      camera->position = add_vec3f32(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_S)) {
      Vec3f32 delta = scale_vec3f32(camera->front, camera_speed);
      camera->position = sub_vec3f32(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_A)) {
      Vec3f32 cross = cross_vec3f32(camera->front, camera->up);
      Vec3f32 delta = scale_vec3f32(cross, camera_speed);
      camera->position = sub_vec3f32(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_D)) {
      Vec3f32 cross = cross_vec3f32(camera->front, camera->up);
      Vec3f32 delta = scale_vec3f32(cross, camera_speed);
      camera->position = add_vec3f32(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_Q)) {
      camera->position.y -= camera_speed;
    }
    if (input_is_key_down(KeyboardKey_E)) {
      camera->position.y += camera_speed;
    }
    
    f32 x_offset = InputState.mouse_current.screen_space_x - InputState.mouse_previous.screen_space_x;
    f32 y_offset = InputState.mouse_previous.screen_space_y - InputState.mouse_current.screen_space_y;
    
    camera->yaw   += (x_offset * CAMERA_SENSITIVITY);
    camera->pitch += (y_offset * CAMERA_SENSITIVITY);
    
    if (camera->pitch >  89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;
    
    _camera_update(camera);
  } else {
    camera->mode = CameraMode_Select;
  }
}

internal void _camera_update(Camera* camera) {
  Vec3f32 front = vec3f32(cos(Radians(camera->yaw)) * cos(Radians(camera->pitch)),sin(Radians(camera->pitch)),sin(Radians(camera->yaw)) * cos(Radians(camera->pitch)));
  
  camera->front = normalize_vec3f32(front);
  Vec3f32 right = cross_vec3f32(camera->front, WORLD_UP);
  camera->right = normalize_vec3f32(right);
  Vec3f32 up    = cross_vec3f32(camera->right, camera->front);
  camera->up    = normalize_vec3f32(up);
}