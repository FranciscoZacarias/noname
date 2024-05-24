
internal Cube cube_new(Vec3f32 position, Vec4f32 color, f32 border_thickness) {
  Cube result = { 0 };
  result.transform        = translate_mat4f32(position.x, position.y, position.z);;
  result.color            = color;
  result.border_thickness = border_thickness;
  result.is_dead          = 0;
  return result;
}

internal Quad cube_get_local_space_face_quad(Cube_Face face) {
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

internal Vec3f32 cube_get_center(Cube cube) {
  Vec3f32 result = {
    cube.transform.m12,
    cube.transform.m13,
    cube.transform.m14
  };
  return result;
}