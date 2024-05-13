
function Cube cube_new(Vec3f32 position, Vec4f32 color) {
  Cube result = { 0 };
  result.transform = translate_mat4f32(position.x, position.y, position.z);;
  result.color     = color;
  return result;
}

function Quad cube_get_local_space_face_quad(Cube_Face face) {
	Quad result = { 0 };

	switch(face) {
		case CubeFace_Back: {
			result = (Quad){ CubeVerticesLocalSpace.p0, CubeVerticesLocalSpace.p1, CubeVerticesLocalSpace.p2, CubeVerticesLocalSpace.p3 };
		} break;
		case CubeFace_Front: {
			result = (Quad){ CubeVerticesLocalSpace.p6, CubeVerticesLocalSpace.p5, CubeVerticesLocalSpace.p4, CubeVerticesLocalSpace.p7 };;
		} break;
		case CubeFace_Left: {
			result = (Quad){ CubeVerticesLocalSpace.p0, CubeVerticesLocalSpace.p3, CubeVerticesLocalSpace.p7, CubeVerticesLocalSpace.p4 };
		} break;
		case CubeFace_Right: {
			result = (Quad){ CubeVerticesLocalSpace.p5, CubeVerticesLocalSpace.p6, CubeVerticesLocalSpace.p2, CubeVerticesLocalSpace.p1 };
		} break;
		case CubeFace_Bottom: {
			result = (Quad){ CubeVerticesLocalSpace.p4, CubeVerticesLocalSpace.p5, CubeVerticesLocalSpace.p1, CubeVerticesLocalSpace.p0 };
		} break;
		case CubeFace_Top: {
			result = (Quad){ CubeVerticesLocalSpace.p3, CubeVerticesLocalSpace.p2, CubeVerticesLocalSpace.p6, CubeVerticesLocalSpace.p7 };
		} break;
	}

	return result;
}