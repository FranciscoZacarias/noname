#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_YAW         -90.0f
#define CAMERA_PITCH        0.0f
#define CAMERA_SPEED        5.0f
#define CAMERA_SENSITIVITY  0.1f
#define WORLD_UP            vec3f32(0.0f, 1.0f, 0.0f)

typedef struct Camera {
	Vec3f32 position;
	Vec3f32 front;
	Vec3f32 up;
	Vec3f32 right;
	f32 yaw;
	f32 pitch;
} Camera;

function Camera camera_create(Vec3f32 position);
function void _camera_update(Camera* camera);

#endif // CAMERA_H