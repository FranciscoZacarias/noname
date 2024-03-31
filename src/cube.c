
function void cube_program_init() {
	CubeProgramObject.shader_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());

	glGenVertexArrays(1, &CubeProgramObject.VAO);
	glGenBuffers(1, &CubeProgramObject.VBO);
	glGenBuffers(1, &CubeProgramObject.EBO);

	glBindVertexArray(CubeProgramObject.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeProgramObject.VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeProgramObject.EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeObjectVerticesLocalSpace), CubeObjectVerticesLocalSpace, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeObjectIndices), CubeObjectIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

function void cube_program_draw(Cube cube, Mat4f32 view, Mat4f32 projection) {
	shader_use(CubeProgramObject.shader_program);

	glBindVertexArray(CubeProgramObject.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeProgramObject.VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeProgramObject.EBO);

	shader_set_uniform_mat4fv(CubeProgramObject.shader_program, "view", view);
	shader_set_uniform_mat4fv(CubeProgramObject.shader_program, "projection", projection);
	shader_set_uniform_mat4fv(CubeProgramObject.shader_program, "model", cube.transform);
	shader_set_uniform_vec3fv(CubeProgramObject.shader_program, "color", cube.color);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

function void cube_program_clean() {
	glDeleteVertexArrays(1, &CubeProgramObject.VAO);
	glDeleteBuffers(1, &CubeProgramObject.VBO);
	glDeleteBuffers(1, &CubeProgramObject.EBO);
}

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

function Vec3f32 cube_get_center(Cube cube) {
	Vec4f32 transformed_center = mul_vec4f32_mat4f32(vec4f32(0.0f, 0.0f, 0.0f), cube.transform);
	Vec3f32 result = vec3f32_from_vec4f32(transformed_center);
	return result;
}