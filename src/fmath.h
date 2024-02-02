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

#define vec4_magnitude(vec4) {sqrt(vec4.x*vec4.x+ vec4.y*vec4.y+vec4.z*vec4.z)}

Vec4 vec4_new(f32 x, f32 y, f32 z);
Vec4 vec4_normalize(Vec4 vec);

//////////////////////////////////////////////
// Matrix

typedef struct Mat4 {
  union {
    // NOTE: data[col][row]
    // OpenGL wants column major matrices
    f32 data[4][4];
    f32 raw[16];
  };
} Mat4;

#define mat4_make_identity() {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}

Mat4 mat4_translate(Mat4 mat, Vec4 vec);
Mat4 mat4_scaling(Mat4 mat, Vec4 scale);
Mat4 mat4_rotate(Mat4 mat, Vec4 axis, f32 degrees);

//////////////////////////////////////////////
// Operations

Mat4 mul_mat4_mat4(Mat4 matA, Mat4 matB);
Vec4 mul_mat4_vec4(Mat4 mat,  Vec4 vec);

#endif //FMATH_H
