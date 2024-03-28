#ifndef CUBE_H
#define CUBE_H

typedef struct Cube {
  Mat4f32 transform;
  Vec3f32 color;
} Cube;

typedef struct CubeFace {
  Vec3f32 a;
  Vec3f32 b;
  Vec3f32 c;
  Vec3f32 d;
} CubeFace;

// Vertices and indices that define cubes in local space ----
f32 vertices[] = {
  // Front face
  -0.5f, -0.5f,  0.5f,  // 0
   0.5f, -0.5f,  0.5f,  // 1
   0.5f,  0.5f,  0.5f,  // 2
  -0.5f,  0.5f,  0.5f,  // 3
  // Back face
  -0.5f, -0.5f, -0.5f,  // 4
   0.5f, -0.5f, -0.5f,  // 5
   0.5f,  0.5f, -0.5f,  // 6
  -0.5f,  0.5f, -0.5f   // 7
};
u32 indices[] = {
  0, 1, 2, 2, 3, 0,  // Front face
  1, 5, 6, 6, 2, 1,  // Right face
  5, 4, 7, 7, 6, 5,  // Back face
  4, 0, 3, 3, 7, 4,  // Left face
  3, 2, 6, 6, 7, 3,  // Top face
  4, 5, 1, 1, 0, 4   // Bottom face
};

function Cube cube_create(Vec3f32 position, Vec3f32 color);
function void cube_translate(Cube* cube, Vec3f32 translation);
function void cube_rotate(Cube* cube, Vec3f32 axis, f32 radians);
function void cube_scale(Cube* cube, Vec3f32 scale);

function CubeFace cube_get_face(Cube cube, u32 face_nr);

#endif // CUBE_H