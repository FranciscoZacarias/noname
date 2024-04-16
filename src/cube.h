#ifndef CUBE_H
#define CUBE_H

typedef struct CubeProgram {
  Shader shader_program;
  u32 VAO;
  u32 VBO;
  u32 EBO;
} CubeProgram;

global CubeProgram CubeProgramObject = { 0 };

global f32 CubeObjectVerticesLocalSpace[] = {
  -1.f, -1.f,  1.f, // 0 Back  face
   1.f, -1.f,  1.f, // 1 Back  face
   1.f,  1.f,  1.f, // 2 Back  face
  -1.f,  1.f,  1.f, // 3 Back  face
  -1.f, -1.f, -1.f, // 4 Front face
   1.f, -1.f, -1.f, // 5 Front face
   1.f,  1.f, -1.f, // 6 Front face
  -1.f,  1.f, -1.f  // 7 Front face
};

global u32 CubeObjectIndices[] = {
  // Do not change the order. A lot of assumptions are made 
  // throughout the code about it.
  0, 1, 2, 2, 3, 0, // Back
  1, 5, 6, 6, 2, 1, // Right
  5, 4, 7, 7, 6, 5, // Front
  4, 0, 3, 3, 7, 4, // Left
  3, 2, 6, 6, 7, 3, // Top
  4, 5, 1, 1, 0, 4  // Bottom
};

typedef struct Cube { 
  Mat4f32 transform;
  Vec3f32 color;
} Cube;

typedef struct CubeVertices {
/* In the same order as CubeObjectIndices presents
     3--------2
    /|       /|
   / |      / |
  7--+-----6  |
  |  0-----|--1
  | /      | /
  |/       |/
  4--------5
*/
  union {
    Vec3f32 v[8];
    struct {
      Vec3f32 v0;
      Vec3f32 v1;
      Vec3f32 v2;
      Vec3f32 v3;
      Vec3f32 v4;
      Vec3f32 v5;
      Vec3f32 v6;
      Vec3f32 v7;
    };
  };
} CubeVertices;

typedef Vec3f32 CubeFace[6];

function void cube_program_init();
function void cube_program_draw(Cube cube, Mat4f32 view, Mat4f32 projection);
function void cube_program_clean();

function Cube cube_create(Vec3f32 position, Vec3f32 color);
function void cube_translate(Cube* cube, Vec3f32 translation);
function void cube_rotate(Cube* cube, Vec3f32 axis, f32 radians);
function void cube_scale(Cube* cube, Vec3f32 scale);
function Vec3f32 cube_get_center(Cube cube);
function CubeVertices cube_get_transformed_vertices(Cube cube);

#endif // CUBE_H