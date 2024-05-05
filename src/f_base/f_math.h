#ifndef FMATH_H
#define FMATH_H

#define PI 3.14159265358979323846f
#define Degrees(r) (r * (180 / PI))
#define Radians(d) (d * (PI / 180))

#include <math.h> // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()
 
//////////////////////////////////////////////
// Vector3 f32

typedef struct Vec3f32 {
  union {
    f32 data[3];
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
  };
} Vec3f32;

//////////////////////////////////////////////
// Vector4 f32

typedef struct Vec4f32 {
  union {
    f32 data[4];
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };
} Vec4f32;

//////////////////////////////////////////////
// Matrix4 f32
typedef struct Mat4f32 {
  union {
    f32 data[4][4];
    struct {
      f32 m0, m4, m8,  m12,
          m1, m5, m9,  m13,
          m2, m6, m10, m14,
          m3, m7, m11, m15;
    };
  };
} Mat4f32;

//////////////////////////////////////////////
// Generic Math 

typedef struct Quad {
  Vec3f32 p0;
  Vec3f32 p1;
  Vec3f32 p2;
  Vec3f32 p3;
} Quad;

function Quad transform_quad(Quad q, Mat4f32 m);

typedef struct Linef32 {
  Vec3f32 point;
  Vec3f32 direction;
} Linef32;

function Linef32 linef32(Vec3f32 point, Vec3f32 direction);

//////////////////////////////////////////////
// Vector3 f32 

function Vec3f32 vec3f32(f32 x, f32 y, f32 z);
function Vec3f32 vec3f32_from_vec4f32(Vec4f32 v);
function void print_vec3f32(Vec3f32 v, const char* label);

function Vec3f32 add_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 sub_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 mul_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 mul_vec3f32_mat4f32(Vec3f32 v, Mat4f32 m);
function Vec3f32 div_vec3f32(Vec3f32 a, Vec3f32 b);

function Vec3f32 cross_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 scale_vec3f32(Vec3f32 v, f32 scalar);
function Vec3f32 scale_vec3f32_xyz(Vec3f32 v, f32 scale_x, f32 scale_y, f32 scale_z);
function Vec3f32 normalize_vec3f32(Vec3f32 v);
function Vec3f32 transform_vec3f32_mat4f32(Vec3f32 v, Mat4f32 m);
function Vec3f32 rotate_by_axis_vec3f32(Vec3f32 v, Vec3f32 axis, f32 angle);
function Vec3f32 lerp_vec3f32(Vec3f32 a, Vec3f32 b, f32 t);
function Vec3f32 unproject_vec3f32(Vec3f32 source, Mat4f32 projection, Mat4f32 view);

function f32 dot_vec3f32(Vec3f32 a, Vec3f32 b);
function f32 len_vec3f32(Vec3f32 v);
function f32 distance_vec3f32(Vec3f32 a, Vec3f32 b);
function f32 angle_vec3f32(Vec3f32 a, Vec3f32 b);

//////////////////////////////////////////////
// Vector4 f32

function Vec4f32 vec4f32 (f32 x, f32 y, f32 z);
function Vec4f32 vec4f32w(f32 x, f32 y, f32 z, f32 w);
function Vec4f32 vec4f32_from_vec3f32(Vec3f32 v);

function Vec4f32 add_vec4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 sub_vec4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 mul_vec4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 div_vec4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 mul_vec4f32_mat4f32(Vec4f32 v, Mat4f32 m);

function Vec4f32 scale_vec4f32(Vec4f32 v, f32 scalar);
function Vec4f32 normalize_vec4f32(Vec4f32 v);
function Vec4f32 lerp_vec4f32(Vec4f32 a, Vec4f32 b, f32 t);

function f32 dot_vec4f32(Vec4f32 a, Vec4f32 b);
function f32 len_vec4f32(Vec4f32 v);
function f32 distance_vec4f32(Vec4f32 a, Vec4f32 b);

//////////////////////////////////////////////
// Matrix4 f32
function Mat4f32 mat4f32(f32 diag);

function Mat4f32 add_mat4f32(Mat4f32 left, Mat4f32 right);
function Mat4f32 sub_mat4f32(Mat4f32 left, Mat4f32 right);
/* Apply the left matrix to the right matrix*/
function Mat4f32 mul_mat4f32(Mat4f32 left, Mat4f32 right); 

function Mat4f32 translate_mat4f32(f32 x, f32 y, f32 z);
function Mat4f32 rotate_axis_mat4f32(Vec3f32 axis, f32 radians);
function Mat4f32 rotate_x_mat4f32(f32 radians);
function Mat4f32 rotate_y_mat4f32(f32 radians);
function Mat4f32 rotate_z_mat4f32(f32 radians);
function Mat4f32 rotate_xyz_mat4f32(Vec3f32 radians);
function Mat4f32 rotate_zyx_mat4f32(Vec3f32 radians);

function Mat4f32 transpose_mat4f32(Mat4f32 m);
function Mat4f32 scale_mat4f32(f32 x, f32 y, f32 z);
function Mat4f32 frustum_mat4f32(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
function Mat4f32 perspective_mat4f32(f64 fovy, f64 window_width, f64 window_height, f64 near_plane, f64 far_plane);
function Mat4f32 ortographic_mat4f32(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
function Mat4f32 look_at_mat4f32(Vec3f32 eye, Vec3f32 target, Vec3f32 up);

//////////////////////////////////////////////
// Math utils
function f32 clampf32(f32 value, f32 min, f32 max);
function f32 lerpf32(f32 start, f32 end, f32 t);
function b32 is_vector_inside_rectangle(Vec3f32 p, Vec3f32 a, Vec3f32 b, Vec3f32 c);
function Vec3f32 intersect_line_with_plane(Linef32 line, Vec3f32 point1, Vec3f32 point2, Vec3f32 point3);

#endif // FMATH_H