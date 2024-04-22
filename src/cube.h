#ifndef CUBE_H
#define CUBE_H

typedef struct CubeProgram {
  Shader shader_program;
  u32 VAO;
  u32 VBO;
  u32 EBO;
} CubeProgram;

global CubeProgram CubeProgramObject = { 0 };

typedef struct Cube { 
  Mat4f32 transform;
  Vec3f32 color;
} Cube;

typedef struct CubeVertices {
  /* 3--------2
    /|       /|
   7-+------6 |
   | 0------|-1
   |/       |/
   4--------5 */
  union {
    f32 data[108];
    Vec3f32 vertices[36];
    struct {
        // Naming scheme: {face}_{rect-vertex}_{vertex-index}
        Vec3f32 back_1_0,  back_1_1,  back_1_2;
        Vec3f32 back_3_2,  back_3_3,  back_3_4;

        Vec3f32 front_5_4, front_5_5, front_5_6;
        Vec3f32 front_7_6, front_7_7, front_7_4;

        Vec3f32 left_3_7,  left_3_3,  left_3_0;
        Vec3f32 left_4_0,  left_4_4,  left_4_7;

        Vec3f32 right_2_6, right_2_2, right_2_1;
        Vec3f32 right_5_1, right_5_5, right_5_6;

        Vec3f32 bottom_1_0, bottom_1_1, bottom_1_5;
        Vec3f32 bottom_4_5, bottom_4_4, bottom_4_0;

        Vec3f32 top_2_3, top_2_2, top_2_6;
        Vec3f32 top_7_6, top_7_7, top_7_3;
    };
  };
} CubeVertices;

global CubeVertices CubeVerticesLocalSpace = {
  // Back
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
  // Front
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,
  // Left
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  // Right
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  // Bottom
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  // Top
  -1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,
};

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