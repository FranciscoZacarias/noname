
function Cube cube_create(Vec3f32 position, Vec3f32 color) {
	Cube result = { 0 };
	result.transform = translate_mat4f32(position.x, position.y, position.z);
	result.color = color;
	return result;
}

function void cube_translate(Cube* cube, Vec3f32 translation) {
	Mat4f32 translate = translate_mat4f32(translation.x, translation.y, translation.z);
	cube->transform = mul_mat4f32(translate, cube->transform);
}

function void cube_rotate(Cube* cube, Vec3f32 axis, f32 radians) {
	Mat4f32 rotation = rotate_axis_mat4f32(axis, radians);
	cube->transform = mul_mat4f32(rotation, cube->transform);
}

function void cube_scale(Cube* cube, Vec3f32 scale) {
	Mat4f32 scalar = scale_mat4f32(scale.x, scale.y, scale.z);
	cube->transform = mul_mat4f32(scalar, cube->transform);
}