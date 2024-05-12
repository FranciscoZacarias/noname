#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_YAW         -90.0f
#define CAMERA_PITCH        0.0f
#define CAMERA_SENSITIVITY  0.1f
#define WORLD_UP            vec3f32(0.0f, 1.0f, 0.0f)

global f32 CAMERA_SPEED = 8.0f;

typedef enum CameraMovement {
	CameraMovement_Front,
	CameraMovement_Back,
	CameraMovement_Right,
	CameraMovement_Left,
	CameraMovement_Up,
	CameraMovement_Down
} CameraMovement;

typedef struct Camera {
	Vec3f32 position;
	Vec3f32 front;
	Vec3f32 up;
	Vec3f32 right;
	f32 yaw;
	f32 pitch;
} Camera;

function Camera camera_create();
function void print_camera(Camera camera);
function void camera_mouse_callback(Camera* camera, f64 x_pos, f64 y_pos);
function void camera_keyboard_callback(Camera* camera, CameraMovement movement, f32 delta_time);
function void _camera_update(Camera* camera);

#endif // CAMERA_H