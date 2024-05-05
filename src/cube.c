
function Cube cube_new(Vec3f32 position, Vec4f32 color) {
  Cube result = { 0 };
  result.transform = translate_mat4f32(position.x, position.y, position.z);;
  result.color     = color;
  return result;
}

function Vec3f32 cube_get_position(Cube cube) {
	Vec4f32 transformed_center = mul_vec4f32_mat4f32(vec4f32(0.0f, 0.0f, 0.0f), cube.transform);
	Vec3f32 result = vec3f32_from_vec4f32(transformed_center);
	return result;
}

function Quad cube_vertices_get_face(CubeVertices vertices, CubeFace face) {
	Quad result = { 0 };

	switch(face) {
		case CubeFace_Back: {
			result = (Quad){ vertices.p0, vertices.p1, vertices.p2, vertices.p3 };
		} break;
		case CubeFace_Front: {
			result = (Quad){ vertices.p6, vertices.p5, vertices.p4, vertices.p7 };;
		} break;
		case CubeFace_Left: {
			result = (Quad){ vertices.p0, vertices.p3, vertices.p7, vertices.p4 };
		} break;
		case CubeFace_Right: {
			result = (Quad){ vertices.p5, vertices.p6, vertices.p2, vertices.p1 };
		} break;
		case CubeFace_Bottom: {
			result = (Quad){ vertices.p4, vertices.p5, vertices.p1, vertices.p0 };
		} break;
		case CubeFace_Top: {
			result = (Quad){ vertices.p3, vertices.p2, vertices.p6, vertices.p7 };
		} break;
	}

	return result;
}

function CubeVertices cube_vertices_apply_transform(CubeVertices vertices, Mat4f32 transform) {
	CubeVertices result;
	result.p0 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p0), transform));
	result.p1 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p1), transform));
	result.p2 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p2), transform));
	result.p3 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p3), transform));
	result.p4 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p4), transform));
	result.p5 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p5), transform));
	result.p6 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p6), transform));
	result.p7 = vec3f32_from_vec4f32(mul_vec4f32_mat4f32(vec4f32_from_vec3f32(vertices.p7), transform));
	return result;
}