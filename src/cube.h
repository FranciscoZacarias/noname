#ifndef CUBE_H
#define CUBE_H

typedef struct CubeProgram {
  Shader shader_program;
  u32 VAO;
  u32 VBO;
  u32 EBO;
} CubeProgram;

global CubeProgram CubeProgramObject = { 0 };

global f32 CubeObjectVertices[] = {
  // Front face
  -1.f, -1.f,  1.f,  // 0
   1.f, -1.f,  1.f,  // 1
   1.f,  1.f,  1.f,  // 2
  -1.f,  1.f,  1.f,  // 3
  // Back face
  -1.f, -1.f, -1.f,  // 4
   1.f, -1.f, -1.f,  // 5
   1.f,  1.f, -1.f,  // 6
  -1.f,  1.f, -1.f   // 7
};
global u32 CubeObjectIndices[] = {
  0, 1, 2, 2, 3, 0,  // Front face
  1, 5, 6, 6, 2, 1,  // Right face
  5, 4, 7, 7, 6, 5,  // Back face
  4, 0, 3, 3, 7, 4,  // Left face
  3, 2, 6, 6, 7, 3,  // Top face
  4, 5, 1, 1, 0, 4   // Bottom face
};

typedef struct Cube { 
  Mat4f32 transform;
  Vec3f32 color;
} Cube;

function void cube_program_init();
function void cube_program_draw(Cube cube, Mat4f32 view, Mat4f32 projection);
function void cube_program_clean();

function Cube cube_create(Vec3f32 position, Vec3f32 color);
function void cube_translate(Cube* cube, Vec3f32 translation);
function void cube_rotate(Cube* cube, Vec3f32 axis, f32 radians);
function void cube_scale(Cube* cube, Vec3f32 scale);

#endif // CUBE_H