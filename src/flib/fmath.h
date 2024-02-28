#ifndef FMATH_H
#define FMATH_H

#define PI 3.1415926535897f
#define degrees_from_radians(r) (r * (180 / PI))
#define radians_from_degrees(d) (d * (PI / 180))

//////////////////////////////////////////////
// Vector 3

typedef struct Vec3f32 {
  union {
    f32 v[3];
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
	};
} Vec3f32;

function Vec3f32 vec3f32(f32 x, f32 y, f32 z);
function Vec3f32 vec3f32_scale(Vec3f32 a, f32 s);
function Vec3f32 vec3f32_normalize(Vec3f32 v);
function f32     vec3f32_len(Vec3f32 v);
function f32     vec3f32_dot(Vec3f32 a, Vec3f32 b);
function Vec3f32 cross_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 add_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 sub_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 mul_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 div_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b);

//////////////////////////////////////////////
// Vector 4

typedef struct Vec4f32 {
  union {
    f32 v[4];
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
	};
} Vec4f32;

function Vec4f32 vec4f32(f32 x, f32 y, f32 z, f32 w);
function Vec4f32 vec4f32_scale(Vec4f32 a, f32 s);
function Vec4f32 vec4f32_normalize(Vec4f32 v);
function f32     vec4f32_len(Vec4f32 v);
function f32     vec4f32_dot(Vec4f32 a, Vec4f32 b);
function Vec4f32 add_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 sub_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 mul_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 div_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b);

//////////////////////////////////////////////
// Matrix 4

typedef struct Mat4f32 {
    f32 v[4][4];
} Mat4f32;

function Mat4f32 mat4f32(f32 diagonal);
function Mat4f32 mat4f32_make_scale(f32 x, f32 y, f32 z);
function Mat4f32 mat4f32_scale(Mat4f32 m, f32 s);
function Mat4f32 mat4f32_make_translate(f32 x, f32 y, f32 z);
function Mat4f32 mat4f32_translate(Mat4f32 m, f32 x, f32 y, f32 z);
function Mat4f32 mat4f32_make_rotate(f32 x, f32 y, f32 z, f32 degrees);
function Mat4f32 mat4f32_rotate(Mat4f32 m, f32 x, f32 y, f32 z, f32 degrees);
function Mat4f32 mat4f32_perspective(f32 fov_degrees, f32 aspect, f32 n, f32 f);
function Mat4f32 mat4f32_look_at(Vec3f32 eye, Vec3f32 target, Vec3f32 up);
function Mat4f32 mat4f32_transpose(Mat4f32 m);
function Mat4f32 mat4f32_invert(Mat4f32 m);

function Mat4f32 mul_mat4f32_mat4f32(Mat4f32 a, Mat4f32 b);
function Mat4f32 add_m4f32_m4f32(Mat4f32 a, Mat4f32 b);
function Mat4f32 sub_m4f32_m4f32(Mat4f32 a, Mat4f32 b);
function Mat4f32 outer_vec3f32(Vec3f32 a, Vec3f32 b);

#endif // FMATH_H