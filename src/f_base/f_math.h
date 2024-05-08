
#ifdef F_MATH_HEADER

#define PI 3.14159265358979323846f
#define Degrees(r) (r * (180 / PI))
#define Radians(d) (d * (PI / 180))
 
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

typedef struct Quad {
  Vec3f32 p0;
  Vec3f32 p1;
  Vec3f32 p2;
  Vec3f32 p3;
} Quad;


typedef struct Linef32 {
  Vec3f32 point;
  Vec3f32 direction;
} Linef32;

function Quad transform_quad(Quad q, Mat4f32 m);
function Linef32 linef32(Vec3f32 point, Vec3f32 direction);

function Vec3f32 vec3f32(f32 x, f32 y, f32 z);
function Vec3f32 vec3f32_from_vec4f32(Vec4f32 v); /* Discards the w value */
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

function Mat4f32 mat4f32(f32 diag);
function Mat4f32 add_mat4f32(Mat4f32 left, Mat4f32 right);
function Mat4f32 sub_mat4f32(Mat4f32 left, Mat4f32 right); /* Apply the left matrix to the right matrix*/
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

function f32 clampf32(f32 value, f32 min, f32 max);
function f32 lerpf32(f32 start, f32 end, f32 t);
function b32 is_vector_inside_rectangle(Vec3f32 p, Vec3f32 a, Vec3f32 b, Vec3f32 c);
function Vec3f32 intersect_line_with_plane(Linef32 line, Vec3f32 point1, Vec3f32 point2, Vec3f32 point3);

#undef F_MATH_HEADER
#endif // F_MATH_HEADER

#ifdef F_MATH_IMPLEMENTATION

function Quad transform_quad(Quad q, Mat4f32 m) {
	Quad result = {
		mul_vec3f32_mat4f32(q.p0, m),
		mul_vec3f32_mat4f32(q.p1, m),
		mul_vec3f32_mat4f32(q.p2, m),
		mul_vec3f32_mat4f32(q.p3, m),
	};
	return result;
}

function Linef32 linef32(Vec3f32 point, Vec3f32 direction) {
	Linef32 result = {point, direction};
	return result;
}

function Vec3f32 vec3f32(f32 x, f32 y, f32 z) {
	Vec3f32 result = {x, y, z};
	return result;
}

function Vec3f32 vec3f32_from_vec4f32(Vec4f32 v) {
	Vec3f32 result = { v.x, v.y, v.z };
	return result;
}

#include "stdio.h" 
function void print_vec3f32(Vec3f32 v, const char* label) {
	printf("%sVec3f32(%.2ff, %.2ff, %.2ff)\n", label, v.x, v.y, v.z);
}

function Vec3f32 add_vec3f32(Vec3f32 a, Vec3f32 b) {
	Vec3f32 result = {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
	};
	return result;
}

function Vec3f32 sub_vec3f32(Vec3f32 a, Vec3f32 b) {
	Vec3f32 result = {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	};
	return result;
}

function Vec3f32 mul_vec3f32(Vec3f32 a, Vec3f32 b) {
	Vec3f32 result = {
		a.x * b.x,
		a.y * b.y,
		a.z * b.z
	};
	return result;
}

function Vec3f32 mul_vec3f32_mat4f32(Vec3f32 v, Mat4f32 m) {
	Vec4f32 mult = { 
		m.m0*v.x + m.m4*v.y + m.m8 *v.z + m.m12*1.0f,
		m.m1*v.x + m.m5*v.y + m.m9 *v.z + m.m13*1.0f,
		m.m2*v.x + m.m6*v.y + m.m10*v.z + m.m14*1.0f,
		m.m3*v.x + m.m7*v.y + m.m11*v.z + m.m15*1.0f
	};
	Vec3f32 result = {
		mult.x,
		mult.y,
		mult.z
	};
	return result;
}

function Vec3f32 div_vec3f32(Vec3f32 a, Vec3f32 b) {
	Vec3f32 result = {
		a.x / b.x,
		a.y / b.y,
		a.z / b.z
	};
	return result;
}

function Vec4f32 mul_vec4f32_mat4f32(Vec4f32 v, Mat4f32 m) {
	Vec4f32 result = { 
		m.m0*v.x + m.m4*v.y + m.m8 *v.z + m.m12*v.w,
		m.m1*v.x + m.m5*v.y + m.m9 *v.z + m.m13*v.w,
		m.m2*v.x + m.m6*v.y + m.m10*v.z + m.m14*v.w,
		m.m3*v.x + m.m7*v.y + m.m11*v.z + m.m15*v.w
	};
	return result;
}

function Vec3f32 cross_vec3f32(Vec3f32 a, Vec3f32 b) {
	Vec3f32 result = {
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
	};
	return result;
}

function Vec3f32 scale_vec3f32(Vec3f32 v, f32 scalar) {
	Vec3f32 result = {
		v.x*scalar,
		v.y*scalar,
		v.z*scalar
	};
	return result;
}

function Vec3f32 scale_vec3f32_xyz(Vec3f32 v, f32 scale_x, f32 scale_y, f32 scale_z) {
	Vec3f32 result = {
		v.x * scale_x,
		v.y * scale_y,
		v.z * scale_z
	};
	return result;	
}

function Vec3f32 normalize_vec3f32(Vec3f32 v) {
	Vec3f32 result = v;
	f32 length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	if (length != 0.0f) {
		f32 ilength = 1.0f/length;
		result.x *= ilength;
		result.y *= ilength;
		result.z *= ilength;
	}
	return result;
}

function Vec3f32 transform_vec3f32_mat4f32(Vec3f32 v, Mat4f32 m) {
	Vec3f32 result = {
		m.m0*v.x + m.m4*v.y + m.m8* v.z + m.m12,
		m.m1*v.x + m.m5*v.y + m.m9* v.z + m.m13,
		m.m2*v.x + m.m6*v.y + m.m10*v.z + m.m14
	};
	return result;
}

function Vec3f32 rotate_by_axis_vec3f32(Vec3f32 v, Vec3f32 axis, f32 radians) {
	// Using Euler-Rodrigues Formula
	// Ref.: https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula

	Vec3f32 result = v;

	// Vector3Normalize(axis);
	f32 length = sqrtf(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
	if (length == 0.0f) length = 1.0f;
	f32 ilength = 1.0f/length;
	axis.x *= ilength;
	axis.y *= ilength;
	axis.z *= ilength;

	radians /= 2.0f;
	f32 a = sinf(radians);
	f32 b = axis.x*a;
	f32 c = axis.y*a;
	f32 d = axis.z*a;
	a = cosf(radians);
	Vec3f32 w = { b, c, d };

	// Vector3CrossProduct(w, v)
	Vec3f32 wv = { w.y*v.z - w.z*v.y, w.z*v.x - w.x*v.z, w.x*v.y - w.y*v.x };

	// Vector3CrossProduct(w, wv)
	Vec3f32 wwv = { w.y*wv.z - w.z*wv.y, w.z*wv.x - w.x*wv.z, w.x*wv.y - w.y*wv.x };

	// Vector3Scale(wv, 2*a)
	a *= 2;
	wv.x *= a;
	wv.y *= a;
	wv.z *= a;

	// Vector3Scale(wwv, 2)
	wwv.x *= 2;
	wwv.y *= 2;
	wwv.z *= 2;

	result.x += wv.x;
	result.y += wv.y;
	result.z += wv.z;

	result.x += wwv.x;
	result.y += wwv.y;
	result.z += wwv.z;

	return result;
}

function Vec3f32 lerp_vec3f32(Vec3f32 a, Vec3f32 b, f32 t) {
	Vec3f32 result = {
		a.x + t*(b.x - a.x),
		a.y + t*(b.y - a.y),
		a.z + t*(b.z - a.z)
	};
	return result;
}

function Vec3f32 unproject_vec3f32(Vec3f32 source, Mat4f32 projection, Mat4f32 view) {
		Vec3f32 result = { 0 };

    // Calculate unprojected matrix (multiply view matrix by projection matrix) and invert it
    Mat4f32 matViewProj = {      // MatrixMultiply(view, projection);
        view.m0*projection.m0 + view.m1*projection.m4 + view.m2*projection.m8 + view.m3*projection.m12,
        view.m0*projection.m1 + view.m1*projection.m5 + view.m2*projection.m9 + view.m3*projection.m13,
        view.m0*projection.m2 + view.m1*projection.m6 + view.m2*projection.m10 + view.m3*projection.m14,
        view.m0*projection.m3 + view.m1*projection.m7 + view.m2*projection.m11 + view.m3*projection.m15,
        view.m4*projection.m0 + view.m5*projection.m4 + view.m6*projection.m8 + view.m7*projection.m12,
        view.m4*projection.m1 + view.m5*projection.m5 + view.m6*projection.m9 + view.m7*projection.m13,
        view.m4*projection.m2 + view.m5*projection.m6 + view.m6*projection.m10 + view.m7*projection.m14,
        view.m4*projection.m3 + view.m5*projection.m7 + view.m6*projection.m11 + view.m7*projection.m15,
        view.m8*projection.m0 + view.m9*projection.m4 + view.m10*projection.m8 + view.m11*projection.m12,
        view.m8*projection.m1 + view.m9*projection.m5 + view.m10*projection.m9 + view.m11*projection.m13,
        view.m8*projection.m2 + view.m9*projection.m6 + view.m10*projection.m10 + view.m11*projection.m14,
        view.m8*projection.m3 + view.m9*projection.m7 + view.m10*projection.m11 + view.m11*projection.m15,
        view.m12*projection.m0 + view.m13*projection.m4 + view.m14*projection.m8 + view.m15*projection.m12,
        view.m12*projection.m1 + view.m13*projection.m5 + view.m14*projection.m9 + view.m15*projection.m13,
        view.m12*projection.m2 + view.m13*projection.m6 + view.m14*projection.m10 + view.m15*projection.m14,
        view.m12*projection.m3 + view.m13*projection.m7 + view.m14*projection.m11 + view.m15*projection.m15 };

    // Calculate inverted matrix -> MatrixInvert(matViewProj);
    // Cache the matrix values (speed optimization)
    f32 a00 = matViewProj.m0, a01 = matViewProj.m1, a02 = matViewProj.m2, a03 = matViewProj.m3;
    f32 a10 = matViewProj.m4, a11 = matViewProj.m5, a12 = matViewProj.m6, a13 = matViewProj.m7;
    f32 a20 = matViewProj.m8, a21 = matViewProj.m9, a22 = matViewProj.m10, a23 = matViewProj.m11;
    f32 a30 = matViewProj.m12, a31 = matViewProj.m13, a32 = matViewProj.m14, a33 = matViewProj.m15;

    f32 b00 = a00*a11 - a01*a10;
    f32 b01 = a00*a12 - a02*a10;
    f32 b02 = a00*a13 - a03*a10;
    f32 b03 = a01*a12 - a02*a11;
    f32 b04 = a01*a13 - a03*a11;
    f32 b05 = a02*a13 - a03*a12;
    f32 b06 = a20*a31 - a21*a30;
    f32 b07 = a20*a32 - a22*a30;
    f32 b08 = a20*a33 - a23*a30;
    f32 b09 = a21*a32 - a22*a31;
    f32 b10 = a21*a33 - a23*a31;
    f32 b11 = a22*a33 - a23*a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    f32 invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

    Mat4f32 matViewProjInv = {
        (a11*b11 - a12*b10 + a13*b09)*invDet,
        (-a01*b11 + a02*b10 - a03*b09)*invDet,
        (a31*b05 - a32*b04 + a33*b03)*invDet,
        (-a21*b05 + a22*b04 - a23*b03)*invDet,
        (-a10*b11 + a12*b08 - a13*b07)*invDet,
        (a00*b11 - a02*b08 + a03*b07)*invDet,
        (-a30*b05 + a32*b02 - a33*b01)*invDet,
        (a20*b05 - a22*b02 + a23*b01)*invDet,
        (a10*b10 - a11*b08 + a13*b06)*invDet,
        (-a00*b10 + a01*b08 - a03*b06)*invDet,
        (a30*b04 - a31*b02 + a33*b00)*invDet,
        (-a20*b04 + a21*b02 - a23*b00)*invDet,
        (-a10*b09 + a11*b07 - a12*b06)*invDet,
        (a00*b09 - a01*b07 + a02*b06)*invDet,
        (-a30*b03 + a31*b01 - a32*b00)*invDet,
        (a20*b03 - a21*b01 + a22*b00)*invDet };

    // Create quaternion from source point
    Vec4f32 quat = { source.x, source.y, source.z, 1.0f };

    // Multiply quat point by unprojecte matrix
    Vec4f32 qtransformed = {     // QuaternionTransform(quat, matViewProjInv)
        matViewProjInv.m0*quat.x + matViewProjInv.m4*quat.y + matViewProjInv.m8*quat.z + matViewProjInv.m12*quat.w,
        matViewProjInv.m1*quat.x + matViewProjInv.m5*quat.y + matViewProjInv.m9*quat.z + matViewProjInv.m13*quat.w,
        matViewProjInv.m2*quat.x + matViewProjInv.m6*quat.y + matViewProjInv.m10*quat.z + matViewProjInv.m14*quat.w,
        matViewProjInv.m3*quat.x + matViewProjInv.m7*quat.y + matViewProjInv.m11*quat.z + matViewProjInv.m15*quat.w };

    // Normalized world points in vectors
    result.x = qtransformed.x/qtransformed.w;
    result.y = qtransformed.y/qtransformed.w;
    result.z = qtransformed.z/qtransformed.w;

    return result;
}

function f32 dot_vec3f32(Vec3f32 a, Vec3f32 b) {
	f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
	return result;
}

function f32 len_vec3f32(Vec3f32 v) {
	f32 result = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	return result;
}

function f32 distance_vec3f32(Vec3f32 a, Vec3f32 b) {
	f32 result = 0.0f;
	f32 dx = b.x - a.x;
	f32 dy = b.y - a.y;
	f32 dz = b.z - a.z;
	result = sqrtf(dx*dx + dy*dy + dz*dz);
	return result;
}

function f32 angle_vec3f32(Vec3f32 a, Vec3f32 b) {
	f32 result = 0.0f;
	Vec3f32 cross = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
	f32 len = sqrtf(cross.x*cross.x + cross.y*cross.y + cross.z*cross.z);
	f32 dot = (a.x*b.x + a.y*b.y + a.z*b.z);
	result  = atan2f(len, dot);
	return result;
}

function Vec4f32 vec4f32 (f32 x, f32 y, f32 z) {
	Vec4f32 result = {x, y, z, 1.0f};
	return result;
}

function Vec4f32 vec4f32w(f32 x, f32 y, f32 z, f32 w) {
	Vec4f32 result = {x, y, z, w};
	return result;
}

function Vec4f32 vec4f32_from_vec3f32(Vec3f32 v) {
	Vec4f32 result = {v.x, v.y, v.z};
	return result;
}

function Vec4f32 add_vec4f32(Vec4f32 a, Vec4f32 b) {
	Vec4f32 result = {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w
	};
	return result;
}

function Vec4f32 sub_vec4f32(Vec4f32 a, Vec4f32 b) {
	Vec4f32 result = {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w
	};
	return result;
}

function Vec4f32 mul_vec4f32(Vec4f32 a, Vec4f32 b) {
	Vec4f32 result = {
		a.x * b.x,
		a.y * b.y,
		a.z * b.z,
		a.w * b.w
	};
	return result;
}

function Vec4f32 div_vec4f32(Vec4f32 a, Vec4f32 b) {
	Vec4f32 result = {
		a.x / b.x,
		a.y / b.y,
		a.z / b.z,
		a.w / b.w
	};
	return result;
}

function Vec4f32 scale_vec4f32(Vec4f32 v, f32 scalar) {
	Vec4f32 result = {
		v.x*scalar,
		v.y*scalar,
		v.z*scalar,
		v.w*scalar
	};
	return result;
}

function Vec4f32 normalize_vec4f32(Vec4f32 v) {
	Vec4f32 result = v;
	f32 length = sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
	if (length > 0) {
			f32 ilength = 1.0f/length;
			result.x = v.x*ilength;
			result.y = v.y*ilength;
			result.z = v.z*ilength;
			result.w = v.w*ilength;
	}
	return result;
}

function Vec4f32 lerp_vec4f32(Vec4f32 a, Vec4f32 b, f32 t) {
	Vec4f32 result = {
		a.x + t*(b.x - a.x),
		a.y + t*(b.y - a.y),
		a.z + t*(b.z - a.z),
		a.w + t*(b.w - a.w)
	};
	return result;
}

function f32 dot_vec4f32(Vec4f32 a, Vec4f32 b) {
	f32 result = (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
	return result;
}

function f32 len_vec4f32(Vec4f32 v) {
	f32 result = sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
	return result;
}

function f32 distance_vec4f32(Vec4f32 a, Vec4f32 b) {
	f32 result = 0.0f;
	f32 dx = a.x - b.x;
	f32 dy = a.y - b.y;
	f32 dz = a.z - b.z;
	f32 dw = a.w - b.w;
	result = sqrtf(dx*dx + dy*dy + dz*dz + dw*dw);
	return result;
}

//////////////////////////////////////////////
// Matrix4 f32
function Mat4f32 mat4f32(f32 diag) {
	Mat4f32 result = {
		diag, 0.0f, 0.0f, 0.0f,
		0.0f, diag, 0.0f, 0.0f,
		0.0f, 0.0f, diag, 0.0f,
		0.0f, 0.0f, 0.0f, diag };

	return result;
}

function Mat4f32 add_mat4f32(Mat4f32 left, Mat4f32 right) {
	Mat4f32 result = { 0 };
	result.m0  = left.m0  + right.m0;
	result.m1  = left.m1  + right.m1;
	result.m2  = left.m2  + right.m2;
	result.m3  = left.m3  + right.m3;
	result.m4  = left.m4  + right.m4;
	result.m5  = left.m5  + right.m5;
	result.m6  = left.m6  + right.m6;
	result.m7  = left.m7  + right.m7;
	result.m8  = left.m8  + right.m8;
	result.m9  = left.m9  + right.m9;
	result.m10 = left.m10 + right.m10;
	result.m11 = left.m11 + right.m11;
	result.m12 = left.m12 + right.m12;
	result.m13 = left.m13 + right.m13;
	result.m14 = left.m14 + right.m14;
	result.m15 = left.m15 + right.m15;
	return result;
}

function Mat4f32 sub_mat4f32(Mat4f32 left, Mat4f32 right) {
	Mat4f32 result = { 0 };
	result.m0  = left.m0  - right.m0;
	result.m1  = left.m1  - right.m1;
	result.m2  = left.m2  - right.m2;
	result.m3  = left.m3  - right.m3;
	result.m4  = left.m4  - right.m4;
	result.m5  = left.m5  - right.m5;
	result.m6  = left.m6  - right.m6;
	result.m7  = left.m7  - right.m7;
	result.m8  = left.m8  - right.m8;
	result.m9  = left.m9  - right.m9;
	result.m10 = left.m10 - right.m10;
	result.m11 = left.m11 - right.m11;
	result.m12 = left.m12 - right.m12;
	result.m13 = left.m13 - right.m13;
	result.m14 = left.m14 - right.m14;
	result.m15 = left.m15 - right.m15;
	return result;
}

function Mat4f32 mul_mat4f32(Mat4f32 left, Mat4f32 right) {
	Mat4f32 result = { 0 };
	result.m0  = left.m0 *right.m0 + left.m1 *right.m4 + left.m2 *right.m8  + left.m3 *right.m12;
	result.m1  = left.m0 *right.m1 + left.m1 *right.m5 + left.m2 *right.m9  + left.m3 *right.m13;
	result.m2  = left.m0 *right.m2 + left.m1 *right.m6 + left.m2 *right.m10 + left.m3 *right.m14;
	result.m3  = left.m0 *right.m3 + left.m1 *right.m7 + left.m2 *right.m11 + left.m3 *right.m15;
	result.m4  = left.m4 *right.m0 + left.m5 *right.m4 + left.m6 *right.m8  + left.m7 *right.m12;
	result.m5  = left.m4 *right.m1 + left.m5 *right.m5 + left.m6 *right.m9  + left.m7 *right.m13;
	result.m6  = left.m4 *right.m2 + left.m5 *right.m6 + left.m6 *right.m10 + left.m7 *right.m14;
	result.m7  = left.m4 *right.m3 + left.m5 *right.m7 + left.m6 *right.m11 + left.m7 *right.m15;
	result.m8  = left.m8 *right.m0 + left.m9 *right.m4 + left.m10*right.m8  + left.m11*right.m12;
	result.m9  = left.m8 *right.m1 + left.m9 *right.m5 + left.m10*right.m9  + left.m11*right.m13;
	result.m10 = left.m8 *right.m2 + left.m9 *right.m6 + left.m10*right.m10 + left.m11*right.m14;
	result.m11 = left.m8 *right.m3 + left.m9 *right.m7 + left.m10*right.m11 + left.m11*right.m15;
	result.m12 = left.m12*right.m0 + left.m13*right.m4 + left.m14*right.m8  + left.m15*right.m12;
	result.m13 = left.m12*right.m1 + left.m13*right.m5 + left.m14*right.m9  + left.m15*right.m13;
	result.m14 = left.m12*right.m2 + left.m13*right.m6 + left.m14*right.m10 + left.m15*right.m14;
	result.m15 = left.m12*right.m3 + left.m13*right.m7 + left.m14*right.m11 + left.m15*right.m15;
	return result;
}

function Mat4f32 translate_mat4f32(f32 x, f32 y, f32 z) {
	Mat4f32 result = {
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}

function Mat4f32 rotate_axis_mat4f32(Vec3f32 axis, f32 radians) {
	Mat4f32 result = { 0 };

	f32 x = axis.x;
	f32 y = axis.y;
	f32 z = axis.z;
	f32 lengthSquared = x*x + y*y + z*z;

	if ((lengthSquared != 1.0f) && (lengthSquared != 0.0f)) {
		f32 ilength = 1.0f/sqrtf(lengthSquared);
		x *= ilength;
		y *= ilength;
		z *= ilength;
	}

	f32 sinres = sinf(radians);
	f32 cosres = cosf(radians);
	f32 t = 1.0f - cosres;

	result.m0 = x*x*t + cosres;
	result.m1 = y*x*t + z*sinres;
	result.m2 = z*x*t - y*sinres;
	result.m3 = 0.0f;

	result.m4 = x*y*t - z*sinres;
	result.m5 = y*y*t + cosres;
	result.m6 = z*y*t + x*sinres;
	result.m7 = 0.0f;

	result.m8 = x*z*t + y*sinres;
	result.m9 = y*z*t - x*sinres;
	result.m10 = z*z*t + cosres;
	result.m11 = 0.0f;

	result.m12 = 0.0f;
	result.m13 = 0.0f;
	result.m14 = 0.0f;
	result.m15 = 1.0f;

	return result;
}

function Mat4f32 rotate_x_mat4f32(f32 radians) {
		Mat4f32 result = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		f32 cosres = cosf(radians);
		f32 sinres = sinf(radians);
		result.m5  = cosres;
		result.m6  = sinres;
		result.m9  = -sinres;
		result.m10 = cosres;
		return result;
}

function Mat4f32 rotate_y_mat4f32(f32 radians) {
		Mat4f32 result = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		f32 cosres = cosf(radians);
		f32 sinres = sinf(radians);
		result.m0 = cosres;
		result.m2 = -sinres;
		result.m8 = sinres;
		result.m10 = cosres;
		return result;
}

function Mat4f32 rotate_z_mat4f32(f32 radians) {
		Mat4f32 result = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		f32 cosres = cosf(radians);
		f32 sinres = sinf(radians);
		result.m0 = cosres;
		result.m1 = sinres;
		result.m4 = -sinres;
		result.m5 = cosres;
		return result;
}

function Mat4f32 rotate_xyz_mat4f32(Vec3f32 radians) {
	Mat4f32 result = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	f32 cosz = cosf(-radians.z);
	f32 sinz = sinf(-radians.z);
	f32 cosy = cosf(-radians.y);
	f32 siny = sinf(-radians.y);
	f32 cosx = cosf(-radians.x);
	f32 sinx = sinf(-radians.x);

	result.m0 = cosz*cosy;
	result.m1 = (cosz*siny*sinx) - (sinz*cosx);
	result.m2 = (cosz*siny*cosx) + (sinz*sinx);

	result.m4 = sinz*cosy;
	result.m5 = (sinz*siny*sinx) + (cosz*cosx);
	result.m6 = (sinz*siny*cosx) - (cosz*sinx);

	result.m8 = -siny;
	result.m9 = cosy*sinx;
	result.m10= cosy*cosx;

	return result;
}

function Mat4f32 rotate_zyx_mat4f32(Vec3f32 radians) {
	Mat4f32 result = { 0 };

	f32 cz = cosf(radians.z);
	f32 sz = sinf(radians.z);
	f32 cy = cosf(radians.y);
	f32 sy = sinf(radians.y);
	f32 cx = cosf(radians.x);
	f32 sx = sinf(radians.x);

	result.m0 = cz*cy;
	result.m4 = cz*sy*sx - cx*sz;
	result.m8 = sz*sx + cz*cx*sy;
	result.m12 = 0;

	result.m1 = cy*sz;
	result.m5 = cz*cx + sz*sy*sx;
	result.m9 = cx*sz*sy - cz*sx;
	result.m13 = 0;

	result.m2 = -sy;
	result.m6 = cy*sx;
	result.m10 = cy*cx;
	result.m14 = 0;

	result.m3 = 0;
	result.m7 = 0;
	result.m11 = 0;
	result.m15 = 1;

	return result;
}

function Mat4f32 transpose_mat4f32(Mat4f32 m) {
	Mat4f32 result = { 0 };
	result.m0  = m.m0;
	result.m1  = m.m4;
	result.m2  = m.m8;
	result.m3  = m.m12;
	result.m4  = m.m1;
	result.m5  = m.m5;
	result.m6  = m.m9;
	result.m7  = m.m13;
	result.m8  = m.m2;
	result.m9  = m.m6;
	result.m10 = m.m10;
	result.m11 = m.m14;
	result.m12 = m.m3;
	result.m13 = m.m7;
	result.m14 = m.m11;
	result.m15 = m.m15;
	return result;
}

function Mat4f32 scale_mat4f32(f32 x, f32 y, f32 z) {
	Mat4f32 result = {
		x,    0.0f, 0.0f, 0.0f,
		0.0f, y,    0.0f, 0.0f,
		0.0f, 0.0f, z,    0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}

function Mat4f32 frustum_mat4f32(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane) {
	Mat4f32 result = { 0 };

	f32 rl = (f32)(right - left);
	f32 tb = (f32)(top - bottom);
	f32 fn = (f32)(far_plane - near_plane);

	result.m0 = ((f32)near_plane*2.0f)/rl;
	result.m1 = 0.0f;
	result.m2 = 0.0f;
	result.m3 = 0.0f;

	result.m4 = 0.0f;
	result.m5 = ((f32)near_plane*2.0f)/tb;
	result.m6 = 0.0f;
	result.m7 = 0.0f;

	result.m8 = ((f32)right + (f32)left)/rl;
	result.m9 = ((f32)top + (f32)bottom)/tb;
	result.m10 = -((f32)far_plane + (f32)near_plane)/fn;
	result.m11 = -1.0f;

	result.m12 = 0.0f;
	result.m13 = 0.0f;
	result.m14 = -((f32)far_plane*(f32)near_plane*2.0f)/fn;
	result.m15 = 0.0f;

	return result;
}

function Mat4f32 perspective_mat4f32(f64 fovy, f64 window_width, f64 window_height, f64 near_plane, f64 far_plane) {
	Mat4f32 result = { 0 };

	f64 top = near_plane*tan(fovy*0.5);
	f64 bottom = -top;
	f64 right = top*(window_width/window_height);
	f64 left = -right;

	// MatrixFrustum(-right, right, -top, top, near, far);
	f32 rl = (f32)(right - left);
	f32 tb = (f32)(top - bottom);
	f32 fn = (f32)(far_plane - near_plane);

	result.m0 = ((f32)near_plane*2.0f)/rl;
	result.m5 = ((f32)near_plane*2.0f)/tb;
	result.m8 = ((f32)right + (f32)left)/rl;
	result.m9 = ((f32)top + (f32)bottom)/tb;
	result.m10 = -((f32)far_plane + (f32)near_plane)/fn;
	result.m11 = -1.0f;
	result.m14 = -((f32)far_plane*(f32)near_plane*2.0f)/fn;

	return result;
}

function Mat4f32 ortographic_mat4f32(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane) {
	Mat4f32 result = { 0 };

	f32 rl = (f32)(right - left);
	f32 tb = (f32)(top - bottom);
	f32 fn = (f32)(far_plane - near_plane);

	result.m0 = 2.0f/rl;
	result.m1 = 0.0f;
	result.m2 = 0.0f;
	result.m3 = 0.0f;
	result.m4 = 0.0f;
	result.m5 = 2.0f/tb;
	result.m6 = 0.0f;
	result.m7 = 0.0f;
	result.m8 = 0.0f;
	result.m9 = 0.0f;
	result.m10 = -2.0f/fn;
	result.m11 = 0.0f;
	result.m12 = -((f32)left + (f32)right)/rl;
	result.m13 = -((f32)top + (f32)bottom)/tb;
	result.m14 = -((f32)far_plane + (f32)near_plane)/fn;
	result.m15 = 1.0f;

	return result;
}

function Mat4f32 look_at_mat4f32(Vec3f32 eye, Vec3f32 target, Vec3f32 up) {
	Mat4f32 result = { 0 };

	f32 length = 0.0f;
	f32 ilength = 0.0f;

	// Vector3Subtract(eye, target)
	Vec3f32 vz = { eye.x - target.x, eye.y - target.y, eye.z - target.z };

	// Vector3Normalize(vz)
	Vec3f32 v = vz;
	length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	if (length == 0.0f) length = 1.0f;
	ilength = 1.0f/length;
	vz.x *= ilength;
	vz.y *= ilength;
	vz.z *= ilength;

	// Vector3CrossProduct(up, vz)
	Vec3f32 vx = { up.y*vz.z - up.z*vz.y, up.z*vz.x - up.x*vz.z, up.x*vz.y - up.y*vz.x };

	// Vector3Normalize(x)
	v = vx;
	length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	if (length == 0.0f) length = 1.0f;
	ilength = 1.0f/length;
	vx.x *= ilength;
	vx.y *= ilength;
	vx.z *= ilength;

	// Vector3CrossProduct(vz, vx)
	Vec3f32 vy = { vz.y*vx.z - vz.z*vx.y, vz.z*vx.x - vz.x*vx.z, vz.x*vx.y - vz.y*vx.x };

	result.m0 = vx.x;
	result.m1 = vy.x;
	result.m2 = vz.x;
	result.m3 = 0.0f;
	result.m4 = vx.y;
	result.m5 = vy.y;
	result.m6 = vz.y;
	result.m7 = 0.0f;
	result.m8 = vx.z;
	result.m9 = vy.z;
	result.m10 = vz.z;
	result.m11 = 0.0f;
	result.m12 = -(vx.x*eye.x + vx.y*eye.y + vx.z*eye.z);   // Vector3DotProduct(vx, eye)
	result.m13 = -(vy.x*eye.x + vy.y*eye.y + vy.z*eye.z);   // Vector3DotProduct(vy, eye)
	result.m14 = -(vz.x*eye.x + vz.y*eye.y + vz.z*eye.z);   // Vector3DotProduct(vz, eye)
	result.m15 = 1.0f;

	return result;
}

function f32 clampf32(f32 value, f32 min, f32 max) {
	f32 result = value;
	if (result < min) {
		result = min;
	} else if (result > max) {
		result = max;
	}
	return result;
}

function f32 lerpf32(f32 start, f32 end, f32 t) {
	f32 result = start + t * (end - start);
	return result;
}

function b32 is_vector_inside_rectangle(Vec3f32 p, Vec3f32 a, Vec3f32 b, Vec3f32 c) {
	b32 result = 0;

	Vec3f32 ab = sub_vec3f32(a, b);
	Vec3f32 bc = sub_vec3f32(b, c);
	Vec3f32 ap = sub_vec3f32(a, p);
	Vec3f32 bp = sub_vec3f32(b, p);

	f32 abap = dot_vec3f32(ab, ap);
	f32 abab = dot_vec3f32(ab, ab);
	f32 bcbp = dot_vec3f32(bc, bp);
	f32 bcbc = dot_vec3f32(bc, bc);

	if (0 <= abap && abap <= abab && 0 <= bcbp && bcbp <= bcbc) {
		result = 1;
	}

	return result;
}

function Vec3f32 intersect_line_with_plane(Linef32 line, Vec3f32 point1, Vec3f32 point2, Vec3f32 point3) {
	Vec3f32 result   = vec3f32(F32_MAX, F32_MAX, F32_MAX);
	Vec3f32 plane_v1 = vec3f32(point2.x-point1.x, point2.y-point1.y, point2.z-point1.z);
	Vec3f32 plane_v2 = vec3f32(point3.x-point1.x, point3.y-point1.y, point3.z-point1.z);
	Vec3f32 plane_normal = vec3f32(plane_v1.y*plane_v2.z - plane_v1.z*plane_v2.y,
																 plane_v1.z*plane_v2.x - plane_v1.x*plane_v2.z,
																 plane_v1.x*plane_v2.y - plane_v1.y*plane_v2.x);
	f32 dot = dot_vec3f32(line.direction, plane_normal);

	// If the dot product is close to zero, the line is parallel to the plane
	if (fabs(dot) < 0.000001f) {
			return result;
	}

	// Calculate the vector from a point on the line to a point on the plane
	Vec3f32 lineToPlane = vec3f32(point1.x-line.point.x, point1.y-line.point.y, point1.z-line.point.z);

	// Calculate the distance along the line to the intersection point
	f32 t = (lineToPlane.x*plane_normal.x + lineToPlane.y*plane_normal.y + lineToPlane.z*plane_normal.z) / dot;

	// Calculate the intersection point
	result = vec3f32(line.point.x + t * line.direction.x,
										line.point.y + t * line.direction.y,
										line.point.z + t * line.direction.z);

	return result;
}

#undef  F_MATH_IMPLEMENTATION
#endif // F_MATH_IMPLEMENTATION