/* date = January 29th 2024 6:29 pm */

#ifndef FMATH_H
#define FMATH_H

#define PI 3.14159265359

#define degrees(r) (r * (180 / PI))
#define radians(d) (d * (PI / 180))

//////////////////////////////////////////////
// Vector

typedef struct Vec4 {
  union {
    f32 raw[4];
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };
} Vec4;

Vec4 vec4_make(f32 x, f32 y, f32 z);
Vec4 vec4w(f32 x, f32 y, f32 z, f32 w);
Vec4 vec4_normalize(Vec4 v);
f32  vec4_magnitude(Vec4 v);

//////////////////////////////////////////////
// Matrix

typedef struct Mat4 {
  union {
    f32 data[4][4];
    f32 raw[16];
    struct {
      f32 r0c0, r0c1, r0c2, r0c3,
          r1c0, r1c1, r1c2, r1c3,
          r2c0, r2c1, r2c2, r2c3,
          r3c0, r3c1, r3c2, r3c3;
    };
  };
} Mat4;

#define mat4_make_identity() { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }

Mat4 mat4_make_translate(Vec4 v);
Mat4 mat4_make_scale(Vec4 s);
Mat4 mat4_make_rotate(Vec4 axis, f32 degrees);
Mat4 mat4_make_perspective(f32 fov, f32 aspect_ratio, f32 near_plane, f32 far_plane);
Mat4 mat4_look_at(Vec4 eye, Vec4 target, Vec4 up);

//////////////////////////////////////////////
// Operations

f32  dot(Vec4 a, Vec4 b);
Vec4 cross(Vec4 a, Vec4 b);

Vec4 add_vec4_vec4(Vec4 a, Vec4 b);

Vec4 sub_vec4_vec4(Vec4 a, Vec4 b);

Mat4 mul_mat4_mat4(Mat4 a, Mat4 b);
Vec4 mul_mat4_vec4_make(Mat4 m, Vec4 v);
Vec4 mul_vec4_f32(Vec4 v, f32 f);

#endif //FMATH_H
