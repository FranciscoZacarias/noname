
#ifndef CUBE_H
#define CUBE_H

//   3--------2
//  /|       /|
// 7--------6 |
// | 0------|-1
// |/       |/
// 4--------5
//
// Each vertex number should correspond to an index
// in any implementation logic for this Cube structure

typedef enum Cube_Face {
  CubeFace_Back,
  CubeFace_Front,
  CubeFace_Left,
  CubeFace_Right,
  CubeFace_Bottom,
  CubeFace_Top
} Cube_Face;

typedef struct Cube_Vertices {
  Vec3f32 p0;
  Vec3f32 p1;
  Vec3f32 p2;
  Vec3f32 p3;
  Vec3f32 p4;
  Vec3f32 p5;
  Vec3f32 p6;
  Vec3f32 p7;
} Cube_Vertices;

global Cube_Vertices CubeVerticesLocalSpace = {
  { -1.0f, -1.0f, -1.0f },
  {  1.0f, -1.0f, -1.0f },
  {  1.0f,  1.0f, -1.0f },
  { -1.0f,  1.0f, -1.0f },
  { -1.0f, -1.0f,  1.0f },
  {  1.0f, -1.0f,  1.0f },
  {  1.0f,  1.0f,  1.0f },
  { -1.0f,  1.0f,  1.0f }
};

// Local space only!
#define P0 CubeVerticesLocalSpace.p0
#define P1 CubeVerticesLocalSpace.p1
#define P2 CubeVerticesLocalSpace.p2
#define P3 CubeVerticesLocalSpace.p3
#define P4 CubeVerticesLocalSpace.p4
#define P5 CubeVerticesLocalSpace.p5
#define P6 CubeVerticesLocalSpace.p6
#define P7 CubeVerticesLocalSpace.p7

typedef struct Cube {
  Mat4f32 transform;
  Vec4f32 color;
  
  f32 border_thickness;
  Vec4f32 border_color;
  
  b32 is_selected;
  b32 is_dead;
} Cube;

internal Cube cube_new(Vec3f32 position, Vec4f32 color, f32 border_thickness);
internal Quad cube_get_local_space_face_quad(Cube_Face face);
internal Vec3f32 cube_get_center(Cube cube);

#endif // CUBE_H