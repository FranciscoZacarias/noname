
#ifndef CUBE_H
#define CUBE_H

/*
    3--------2
   /|       /|
  7--------6 |
  | 0------|-1
  |/       |/
  4--------5
  Each vertex number should correspond to an index
  in any implementation logic for this Cube structure
*/

global f32 CubeBorderThickness = 0.05f;

typedef enum CubeFace {
  CubeFace_Back,
  CubeFace_Front,
  CubeFace_Left,
  CubeFace_Right,
  CubeFace_Bottom,
  CubeFace_Top
} CubeFace;

typedef struct CubeVertices {
  Vec3f32 p0;
  Vec3f32 p1;
  Vec3f32 p2;
  Vec3f32 p3;
  Vec3f32 p4;
  Vec3f32 p5;
  Vec3f32 p6;
  Vec3f32 p7;
} CubeVertices;

global CubeVertices CubeVerticesLocalSpace = {
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
} Cube;

function Cube cube_new(Vec3f32 transform, Vec4f32 color);
function Vec3f32 cube_get_position(Cube cube);
function void cube_print(Cube cube);

function Quad cube_vertices_get_face(CubeVertices vertices, CubeFace face);

function CubeVertices cube_vertices_apply_transform(CubeVertices vertices, Mat4f32 transform);

#endif // CUBE_H