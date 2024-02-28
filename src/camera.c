
function Camera camera_create(Vec3f32 position) {
	Camera camera;
	camera.position = position;
	camera.front    = vec3f32(0.0f, 0.0, -1.0f);
	camera.up       = WORLD_UP;
	camera.right    = vec3f32(1.0f, 0.0f, 0.0f);
	camera.yaw      = -90.0f;
	camera.pitch    =  0.0f;
	_camera_update(&camera);
	return camera;
}

function void _camera_update(Camera* camera) {
	Vec3f32 front = vec3f32(
		cos(radians_from_degrees(camera->yaw)) * cos(radians_from_degrees(camera->pitch)),
		sin(radians_from_degrees(camera->pitch)),
		sin(radians_from_degrees(camera->yaw)) * cos(radians_from_degrees(camera->pitch))
	);
	
	camera->front = normalize_vec3f32(front);
	Vec3f32 right = cross_vec3f32(camera->front, WORLD_UP);
	camera->right = normalize_vec3f32(right);
	Vec3f32 up    = cross_vec3f32(camera->right, camera->front);
	camera->up    = normalize_vec3f32(up);
}