
//////////////////////////////////////////////
// Vector

Vec4 vec4_make(f32 x, f32 y, f32 z) {
  Vec4 result = { x, y, z, 1.0f };
  return result;
}

Vec4 vec4_makew(f32 x, f32 y, f32 z, f32 w) {
  Vec4 result = { x, y, z, w };
  return result;
}

// @new
Vec4 vec4_scale(Vec4 v, f32 s) {
  Vec4 result = { v.x*s, v.y*s, v.z*s, v.w*s };
  return result;
}

f32 vec4_magnitude(Vec4 v) {
  f32 result = sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
  return result;
}

Vec4 vec4_normalize(Vec4 v) {
  Vec4 result = vec4_scale(v, 1.0f/vec4_magnitude(v));
  return result;
}

//////////////////////////////////////////////
// Matrix

Mat4 mat4_make_translate(Vec4 t) {
  Mat4 result = mat4_make_identity();
  result.data[3][0] = t.x;
  result.data[3][1] = t.y;
  result.data[3][2] = t.z;
  return result;
}

Mat4 mat4_make_scale(Vec4 s) {
  Mat4 result = mat4_make_identity();
  result.data[0][0] = s.x;
  result.data[1][1] = s.y;
  result.data[2][2] = s.z;
  return result;
}

Mat4 mat4_make_rotate(Vec4 axis, f32 radians) {
  Mat4 result = mat4_make_identity();
  axis = vec4_normalize(axis);
  f32 sin_theta = sin(radians);
  f32 cos_theta = cos(radians);
  f32 cos_value = 1.f - cos_theta;
  result.data[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
  result.data[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta);
  result.data[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta);
  result.data[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta);
  result.data[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
  result.data[1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta);
  result.data[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta);
  result.data[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta);
  result.data[2][2] = (axis.z * axis.z * cos_value) + cos_theta;
  return result;
}

Mat4 mat4_make_perspective(f32 fov, f32 aspect_ratio, f32 near_plane, f32 far_plane) {
  Mat4 result = mat4_make_identity();
  f32 tan_theta_over_2 = tan(fov / 2);
  result.data[0][0] = 1.f / tan_theta_over_2;
  result.data[1][1] = aspect_ratio / tan_theta_over_2;
  result.data[2][3] = 1.f;
  result.data[2][2] = -(near_plane + far_plane) / (near_plane - far_plane);
  result.data[3][2] = (2.f * near_plane * far_plane) / (near_plane - far_plane);
  result.data[3][3] = 0.f;
  return result;
}

Mat4 mat4_look_at(Vec4 eye, Vec4 target, Vec4 up) {
  Mat4 result;
  Vec4 f = vec4_normalize(sub_vec4_vec4(eye, target));
  Vec4 s = vec4_normalize(cross(f, up));
  Vec4 u = cross(s, f);
  result.data[0][0] = s.x;
  result.data[0][1] = u.x;
  result.data[0][2] = -f.x;
  result.data[1][0] = s.y;
  result.data[0][3] = 0.0f;
  result.data[1][1] = u.y;
  result.data[1][2] = -f.y;
  result.data[1][3] = 0.0f;
  result.data[2][0] = s.z;
  result.data[2][1] = u.z;
  result.data[2][2] = -f.z;
  result.data[2][3] = 0.0f;
  result.data[3][0] = eye.x;
  result.data[3][1] = eye.y;
  result.data[3][2] = eye.z;
  result.data[3][3] = 1.0f;
  return result;
}

//////////////////////////////////////////////
// Operations

f32 dot(Vec4 a, Vec4 b) {
  f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
  return result;
}

Vec4 cross(Vec4 a, Vec4 b) {
  Vec4 result = vec4_make(a.y*b.z - a.z*b.y,
                          a.z*b.x - a.x*b.z,
                          a.x*b.y - a.y*b.x);
  return result;
}

Vec4 add_vec4_vec4(Vec4 a, Vec4 b) {
  Vec4 result = vec4_make(a.x + b.x,
                          a.y + b.y,
                          a.z + b.z);
  return result;
}

Vec4 sub_vec4_vec4(Vec4 a, Vec4 b) {
  Vec4 result = vec4_make(a.x - b.x,
                          a.y - b.y,
                          a.z - b.z);
  return result;
}

Vec4 mul_mat4_vec4(Mat4 m, Vec4 v) {
  Vec4 result = {
      m.r0c0*v.x + m.r0c1*v.y + m.r0c2*v.z + m.r0c3*v.w,
      m.r1c0*v.x + m.r1c1*v.y + m.r1c2*v.z + m.r1c3*v.w,
      m.r2c0*v.x + m.r2c1*v.y + m.r2c2*v.z + m.r2c3*v.w,
      m.r3c0*v.x + m.r3c1*v.y + m.r3c2*v.z + m.r3c3*v.w
  };
  return result;
}

Mat4 mul_mat4_mat4(Mat4 a, Mat4 b) {
  Mat4 result;
  for (u32 row = 0; row < 4; ++row) {
    for (u32 col = 0; col < 4; ++col) {
      result.data[row][col] = a.data[row][0] * b.data[0][col] +
                              a.data[row][1] * b.data[1][col] +
                              a.data[row][2] * b.data[2][col] +
                              a.data[row][3] * b.data[3][col];
    }
  }

  return result;
}

Vec4 mul_vec4_f32(Vec4 v, f32 f) {
  Vec4 result = vec4_make(v.x*f,
                          v.y*f,
                          v.z*f);
  return result;
}