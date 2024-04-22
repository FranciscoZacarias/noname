
function void cube_program_init() {
	CubeProgramObject.shader_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());

	// Cube Program
	glGenVertexArrays(1, &CubeProgramObject.VAO);
	glGenBuffers(1, &CubeProgramObject.VBO);

	glBindVertexArray(CubeProgramObject.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeProgramObject.VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVerticesLocalSpace), CubeVerticesLocalSpace.data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Cube Outline
	CubeOutlineObject.shader_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());
	glGenVertexArrays(1, &CubeOutlineObject.VAO);
	glGenBuffers(1, &CubeOutlineObject.VBO);

	glBindVertexArray(CubeOutlineObject.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeOutlineObject.VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeOutlineLocalSpace), CubeOutlineLocalSpace.data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

function void cube_draw(Cube cube, Mat4f32 view, Mat4f32 projection) {
	// Draw normal cube
	glUseProgram(CubeProgramObject.shader_program);

	glBindVertexArray(CubeProgramObject.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeProgramObject.VBO);

	shader_set_uniform_mat4fv(CubeProgramObject.shader_program, "view", view);
	shader_set_uniform_mat4fv(CubeProgramObject.shader_program, "projection", projection);
	shader_set_uniform_mat4fv(CubeProgramObject.shader_program, "model", cube.transform);
	shader_set_uniform_vec3fv(CubeProgramObject.shader_program, "color", cube.color);

	glDrawArrays(GL_TRIANGLES, 0, 108);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Draw outline
	glUseProgram(CubeOutlineObject.shader_program);

	glBindVertexArray(CubeOutlineObject.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeOutlineObject.VBO);

	shader_set_uniform_mat4fv(CubeOutlineObject.shader_program, "view", view);
	shader_set_uniform_mat4fv(CubeOutlineObject.shader_program, "projection", projection);
	shader_set_uniform_mat4fv(CubeOutlineObject.shader_program, "model", cube.transform);
	shader_set_uniform_vec3fv(CubeOutlineObject.shader_program, "color", vec3f32(0.4, 0.4, 0.4f));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 24);
	glLineWidth(1.0f);

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

function void cube_program_clean() {
	glDeleteVertexArrays(1, &CubeProgramObject.VAO);
	glDeleteBuffers(1, &CubeProgramObject.VBO);
}

function Cube cube_create(Vec3f32 position, Vec3f32 color) {
	Cube result = { 0 };
	result.transform = translate_mat4f32(position.x, position.y, position.z);
	result.color = color;
	result.dead = 0;
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

function CubeVertices cube_get_transformed_vertices(Cube cube) {
	CubeVertices result = { 0 };

	Vec4f32 back0   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.back_1_0, 1.0f);
	Vec4f32 back1   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.back_1_1, 1.0f);
	Vec4f32 back2   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.back_1_2, 1.0f);
	Vec4f32 back3   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.back_3_3, 1.0f);
	result.back_1_0 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(back0, cube.transform));
	result.back_1_1 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(back1, cube.transform));
	result.back_1_2 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(back2, cube.transform));
	result.back_3_3 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(back3, cube.transform));

	Vec4f32 front0   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.front_5_4, 1.0f);
	Vec4f32 front1   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.front_5_5, 1.0f);
	Vec4f32 front2   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.front_5_6, 1.0f);
	Vec4f32 front3   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.front_7_7, 1.0f);
	result.front_5_4 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(front0, cube.transform));
	result.front_5_5 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(front1, cube.transform));
	result.front_5_6 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(front2, cube.transform));
	result.front_7_7 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(front3, cube.transform));

	Vec4f32 left0   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.left_3_7, 1.0f);
	Vec4f32 left1   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.left_3_3, 1.0f);
	Vec4f32 left2   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.left_3_0, 1.0f);
	Vec4f32 left3   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.left_4_4, 1.0f);
	result.left_3_7 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(left0, cube.transform));
	result.left_3_3 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(left1, cube.transform));
	result.left_3_0 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(left2, cube.transform));
	result.left_4_4 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(left3, cube.transform));

	Vec4f32 right0   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.right_2_6, 1.0f);
	Vec4f32 right1   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.right_2_2, 1.0f);
	Vec4f32 right2   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.right_2_1, 1.0f);
	Vec4f32 right3   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.right_5_5, 1.0f);
	result.right_2_6 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(right0, cube.transform));
	result.right_2_2 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(right1, cube.transform));
	result.right_2_1 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(right2, cube.transform));
	result.right_5_5 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(right3, cube.transform));

	Vec4f32 bottom0   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.bottom_1_0, 1.0f);
	Vec4f32 bottom1   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.bottom_1_1, 1.0f);
	Vec4f32 bottom2   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.bottom_1_5, 1.0f);
	Vec4f32 bottom3   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.bottom_4_4, 1.0f);
	result.bottom_1_0 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(bottom0, cube.transform));
	result.bottom_1_1 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(bottom1, cube.transform));
	result.bottom_1_5 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(bottom2, cube.transform));
	result.bottom_4_4 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(bottom3, cube.transform));
	
	Vec4f32 top0   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.top_2_3, 1.0f);
	Vec4f32 top1   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.top_2_2, 1.0f);
	Vec4f32 top2   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.top_2_6, 1.0f);
	Vec4f32 top3   = vec4f32_from_vec3f32(CubeVerticesLocalSpace.top_7_7, 1.0f);
	result.top_2_3 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(top0, cube.transform));
	result.top_2_2 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(top1, cube.transform));
	result.top_2_6 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(top2, cube.transform));
	result.top_7_7 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(top3, cube.transform));

	return result;
}