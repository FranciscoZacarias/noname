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

#define vec3f32(x,y,z) _v3f32((x),(y),(z))
function Vec3f32 _v3f32(f32 x, f32 y, f32 z);
function Vec3f32 add_v3f32_v3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 sub_v3f32_v3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 mul_v3f32_v3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 div_v3f32_v3f32(Vec3f32 a, Vec3f32 b);
function Vec3f32 scale_v3f32(Vec3f32 a, f32 s);
function Vec3f32 normalize_v3f32(Vec3f32 v);
function Vec3f32 cross_v3f32(Vec3f32 a, Vec3f32 b);
function f32     len_v3f32(Vec3f32 v);
function f32     dot_v3f32(Vec3f32 a, Vec3f32 b);

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

#define vec4f32(x,y,z)    _v4f32((x),(y),(z),1)
#define vec4f32w(x,y,z,w) _v4f32((x),(y),(z),(w))
function Vec4f32 _v4f32(f32 x, f32 y, f32 z, f32 w);
function Vec4f32 add_v4f32_v4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 sub_v4f32_v4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 mul_v4f32_v4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 div_v4f32_v4f32(Vec4f32 a, Vec4f32 b);
function Vec4f32 scale_v4f32(Vec4f32 a, f32 s);
function Vec4f32 normalize_v4f32(Vec4f32 v);
function f32     len_v4f32(Vec4f32 v);
function f32     dot_v4f32(Vec4f32 a, Vec4f32 b);

//////////////////////////////////////////////
// Matrix 4

typedef struct Mat4f32 {
    f32 v[4][4];
} Mat4f32;

function Mat4f32 mat4f32(f32 diagonal);
function Mat4f32 mat4f32_make_translate(Vec3f32 v);
function Mat4f32 mat4f32_make_scale(Vec3f32 v);
function Mat4f32 mat4f32_make_perspective(f32 fov, f32 asp_ratio, f32 near_z, f32 far_z);
function Mat4f32 mat4f32_make_look_at(Vec3f32 eye, Vec3f32 target, Vec3f32 up);
function Mat4f32 mat4f32_make_rotate(Vec3f32 axis, f32 degrees);
function Mat4f32 mul_mat4f32_mat4f32(Mat4f32 a, Mat4f32 b);

#endif // FMATH_H