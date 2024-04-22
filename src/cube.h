#ifndef CUBE_H
#define CUBE_H

typedef struct CubeProgram {
  Shader shader_program;
  u32 VAO;
  u32 VBO;
} CubeProgram;

global CubeProgram CubeProgramObject = { 0 };
global CubeProgram CubeOutlineObject = { 0 };

typedef struct Cube { 
  Mat4f32 transform;
  Vec3f32 color;
  b32 dead; // HACK: to delete cubes.
} Cube;

typedef struct CubeVertices {
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

/*  3--------2
   /|       /|
  7-+------6 |
  | 0------|-1
  |/       |/
  4--------5  */
#define _point(x,y,z) (x),(y),(z)
#define P0 _point(-1.0f, -1.0f, -1.0f)
#define P1 _point( 1.0f, -1.0f, -1.0f)
#define P2 _point( 1.0f,  1.0f, -1.0f)
#define P3 _point(-1.0f,  1.0f, -1.0f)
#define P4 _point(-1.0f, -1.0f,  1.0f)
#define P5 _point( 1.0f, -1.0f,  1.0f)
#define P6 _point( 1.0f,  1.0f,  1.0f)
#define P7 _point(-1.0f,  1.0f,  1.0f)

global CubeVertices CubeVerticesLocalSpace = {
  /* Bck */ P0, P1, P2, P2, P3, P0,
  /* Frt */ P4, P5, P6, P6, P7, P4,
  /* Lft */ P7, P3, P0, P0, P4, P7,
  /* Rgt */ P6, P2, P1, P1, P5, P6,
  /* Bot */ P0, P1, P5, P5, P4, P0,
  /* Top */ P3, P2, P6, P6, P7, P3
};

typedef struct CubeOutline {
  union {
    f32 data[72];
    Vec3f32 vertices[24];
    struct {
      Vec3f32 p0p1_p0, p0p1_p1;
      Vec3f32 p1p2_p1, p1p2_p2;
      Vec3f32 p2p3_p2, p2p3_p3;
      Vec3f32 p3p0_p3, p3p0_p0;

      Vec3f32 p4p5_p4, p4p5_p5;
      Vec3f32 p5p6_p5, p5p6_p6;
      Vec3f32 p6p7_p6, p6p7_p7;
      Vec3f32 p7p4_p7, p7p4_p4;

      Vec3f32 p0p4_p0, p0p4_p4;
      Vec3f32 p1p5_p1, p1p6_p5;
      Vec3f32 p2p6_p2, p2p6_p6;
      Vec3f32 p3p7_p3, p3p7_p7;
    };
  };
} CubeOutline; 

global CubeOutline CubeOutlineLocalSpace = {
  /* Bck */ P0, P1, P1, P2, P2, P3, P3, P0,
  /* Frt */ P4, P5, P5, P6, P6, P7, P7, P4,
  /* /// */ P0, P4, P1, P5, P2, P6, P3, P7
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