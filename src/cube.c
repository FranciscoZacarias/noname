
function Cube cube_create(Vec3f32 position, Vec3f32 color) {
	Cube result = { 0 };
	result.transform = translate_mat4f32(position.x, position.y, position.z);
	result.color = color;
	return result;
}

function Cube cube_translate(Cube cube, Vec3f32 translation) {
	Cube result = cube;
	Mat4f32 translate = translate_mat4f32(translation.x, translation.y, translation.z);
	result.transform = mul_mat4f32(translate, cube.transform);
	return result;
}

function Cube cube_rotate(Cube cube, Vec3f32 axis, f32 radians) {
	Cube result = cube;
	Mat4f32 rotation = rotate_axis_mat4f32(axis, radians);
	result.transform = mul_mat4f32(rotation, result.transform);
	return result;
}