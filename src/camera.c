function Camera camera_create() {
	Camera camera;
	camera.position = vec3f32(-9.21f, 5.45f, 14.81f);
	camera.front = vec3f32(0.56f, -0.31f, -0.77f);
	camera.up = vec3f32(0.18f, 0.95f, -0.25f);
	camera.right = vec3f32(0.81f, -0.00f, 0.58f);
	camera.pitch = -18.10f;
	camera.yaw = -54.20f;
	_camera_update(&camera);
	return camera;
}

function void print_camera(Camera camera) {
	print_vec3f32(camera.position, "camera.position =");
	print_vec3f32(camera.front, "camera.front =");
	print_vec3f32(camera.up, "camera.up =");
	print_vec3f32(camera.right, "camera.right =");
	printf("camera.pitch = %.2ff;\ncamera.yaw = %.2ff;\n-------------\n", camera.pitch, camera.yaw);
}

function void camera_mouse_callback(Camera* camera, f64 x_pos, f64 y_pos) {
	camera->yaw   += (x_pos * CAMERA_SENSITIVITY);
	camera->pitch += (y_pos * CAMERA_SENSITIVITY);

	if (camera->pitch >  89.0f) camera->pitch = 89.0f;
	if (camera->pitch < -89.0f) camera->pitch = -89.0f;

	_camera_update(camera);
}

function void camera_keyboard_callback(Camera* camera, Camera_Movement movement, f32 delta_time) {
	f32 cameraSpeed = (f32)(CameraSpeed * delta_time);

  if (movement == CameraMovement_Front) {
    Vec3f32 delta = scale_vec3f32(camera->front, cameraSpeed);
    camera->position = add_vec3f32(camera->position, delta);
  }
  if (movement == CameraMovement_Back) {
    Vec3f32 delta = scale_vec3f32(camera->front, cameraSpeed);
    camera->position = sub_vec3f32(camera->position, delta);
  }
  if (movement == CameraMovement_Left) {
    Vec3f32 cross = cross_vec3f32(camera->front, camera->up);
    Vec3f32 delta = scale_vec3f32(cross, cameraSpeed);
    camera->position = sub_vec3f32(camera->position, delta);
  }
  if (movement == CameraMovement_Right) {
    Vec3f32 cross = cross_vec3f32(camera->front, camera->up);
    Vec3f32 delta = scale_vec3f32(cross, cameraSpeed);
    camera->position = add_vec3f32(camera->position, delta);
  }
  if (movement == CameraMovement_Down) {
    camera->position.y -= cameraSpeed;
  }
  if (movement == CameraMovement_Up) {
    camera->position.y += cameraSpeed;
  }
}

function void _camera_update(Camera* camera) {
	Vec3f32 front = vec3f32(
		cos(Radians(camera->yaw)) * cos(Radians(camera->pitch)),
		sin(Radians(camera->pitch)),
		sin(Radians(camera->yaw)) * cos(Radians(camera->pitch))
	);

	camera->front = normalize_vec3f32(front);
	Vec3f32 right = cross_vec3f32(camera->front, WORLD_UP);
	camera->right = normalize_vec3f32(right);
	Vec3f32 up    = cross_vec3f32(camera->right, camera->front);
	camera->up    = normalize_vec3f32(up);
}