/* date = January 29th 2024 6:29 pm */

#ifndef FMATH_H
#define FMATH_H

#define PI 3.14159265

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


Vec4 vector4_new(f32 x, f32 y, f32 z);

//////////////////////////////////////////////
// Matrix

typedef struct Mat4 {
  union {
    f32 data[4][4];
    f32 raw[16];
  };
} Mat4;

Mat4 matrix_identity();

//////////////////////////////////////////////
// Operations

Mat4 matrix_translate(Mat4 mat, Vec4 vec);
Vec4 mul_matrix_vector(Mat4 mat, Vec4 vec);

#endif //FMATH_H
