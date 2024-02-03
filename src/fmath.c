
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

f32 vec4_magnitude(Vec4 v) {
  f32 result = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
  return result;
}

Vec4 vec4_normalize(Vec4 v) {
  f32 m = vec4_magnitude(v);
  Vec4 result = vec4_makew(v.x/m,
                           v.y/m,
                           v.z/m,
                           v.w);
  return result;  
}

//////////////////////////////////////////////
// Matrix

Mat4 mat4_make_translate(Vec4 t) {
  Mat4 result = { 
    1.0f, 0.0f, 0.0f, t.x,
    0.0f, 1.0f, 0.0f, t.y,
    0.0f, 0.0f, 1.0f, t.z,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  return result;
}

Mat4 mat4_make_scale(Vec4 s) {
  Mat4 result = { 
    s.x,  0.0f, 0.0f, 0.0f,
    0.0f, s.y,  0.0f, 0.0f,
    0.0f, 0.0f, s.z,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  return result;
}

Mat4 mat4_make_rotate(Vec4 axis, f32 radians) {
  axis = vec4_normalize(axis);
  f32 sin_theta = sin(radians);
  f32 cos_theta = cos(radians);
  f32 not_cos   = 1-cos_theta;
  Mat4 result   = { 
    (cos_theta+(axis.x*axis.x*not_cos)),          ((axis.x*axis.y*not_cos)-(axis.z*sin_theta)), ((axis.x*axis.z*not_cos)+(axis.y*sin_theta)), 0.0f,
    ((axis.y*axis.x*not_cos)+(axis.z*sin_theta)), (cos_theta+(axis.y*axis.y*not_cos)),          ((axis.y*axis.z*not_cos)-(axis.x*sin_theta)), 0.0f,
    ((axis.z*axis.x*not_cos)-(axis.y*sin_theta)), ((axis.z*axis.y*not_cos)+(axis.x*sin_theta)), (cos_theta+(axis.z*axis.z*not_cos)),          0.0f,
    0.0f,                                         0.0f,                                         0.0f,                                         1.0f
  };
  return result;
}

Mat4 mat4_make_perspective(f32 fov, f32 aspect_ratio, f32 near_plane, f32 far_plane) {
  float tan_half_fov = tanf(fov / 2.0f);
  float range = near_plane - far_plane;
  Mat4 result = {
      (1.0f/(tan_half_fov*aspect_ratio)),   0.0f,                 0.0f,                          0.0f,
      0.0f,                               (1.0f/tan_half_fov),   0.0f,                          0.0f,
      0.0f,                                0.0f,               ((near_plane+far_plane)/range), (2.0f*near_plane*far_plane/range),
      0.0f,                                0.0f,                -1.0f,                          0.0f
  };
  return result;
}

Mat4 mat4_look_at(Vec4 eye, Vec4 center, Vec4 up) {
  Vec4 f = {center.x - eye.x, center.y - eye.y, center.z - eye.z, 0.0f};
  Vec4 r = {0.0f};
  Vec4 u = {up.x, up.y, up.z, 0.0f};

  // Normalize the forward vector
  float f_length = sqrt(f.x * f.x + f.y * f.y + f.z * f.z);
  f.x /= f_length;
  f.y /= f_length;
  f.z /= f_length;

  // Calculate the right and up vectors
  r.x = u.y * f.z - u.z * f.y;
  r.y = u.z * f.x - u.x * f.z;
  r.z = u.x * f.y - u.y * f.x;

  float r_length = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
  r.x /= r_length;
  r.y /= r_length;
  r.z /= r_length;

  u.x = f.y * r.z - f.z * r.y;
  u.y = f.z * r.x - f.x * r.z;
  u.z = f.x * r.y - f.y * r.x;

  // Construct the view matrix
  Mat4 view_matrix = {{
      r.x,      r.y,      r.z,      -dot(r, eye),
      u.x,      u.y,      u.z,      -dot(u, eye),
      -f.x,     -f.y,     -f.z,     dot(f, eye),
      0.0f,     0.0f,     0.0f,     1.0f
  }};

  return view_matrix;
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
    Mat4 result = {
        // Row 0
        a.r0c0*b.r0c0 + a.r0c1*b.r1c0 + a.r0c2*b.r2c0 + a.r0c3*b.r3c0,
        a.r0c0*b.r0c1 + a.r0c1*b.r1c1 + a.r0c2*b.r2c1 + a.r0c3*b.r3c1,
        a.r0c0*b.r0c2 + a.r0c1*b.r1c2 + a.r0c2*b.r2c2 + a.r0c3*b.r3c2,
        a.r0c0*b.r0c3 + a.r0c1*b.r1c3 + a.r0c2*b.r2c3 + a.r0c3*b.r3c3,
        // Row 1
        a.r1c0*b.r0c0 + a.r1c1*b.r1c0 + a.r1c2*b.r2c0 + a.r1c3*b.r3c0,
        a.r1c0*b.r0c1 + a.r1c1*b.r1c1 + a.r1c2*b.r2c1 + a.r1c3*b.r3c1,
        a.r1c0*b.r0c2 + a.r1c1*b.r1c2 + a.r1c2*b.r2c2 + a.r1c3*b.r3c2,
        a.r1c0*b.r0c3 + a.r1c1*b.r1c3 + a.r1c2*b.r2c3 + a.r1c3*b.r3c3,
        // Row 2
        a.r2c0*b.r0c0 + a.r2c1*b.r1c0 + a.r2c2*b.r2c0 + a.r2c3*b.r3c0,
        a.r2c0*b.r0c1 + a.r2c1*b.r1c1 + a.r2c2*b.r2c1 + a.r2c3*b.r3c1,
        a.r2c0*b.r0c2 + a.r2c1*b.r1c2 + a.r2c2*b.r2c2 + a.r2c3*b.r3c2,
        a.r2c0*b.r0c3 + a.r2c1*b.r1c3 + a.r2c2*b.r2c3 + a.r2c3*b.r3c3,
        // Row 3
        a.r3c0*b.r0c0 + a.r3c1*b.r1c0 + a.r3c2*b.r2c0 + a.r3c3*b.r3c0,
        a.r3c0*b.r0c1 + a.r3c1*b.r1c1 + a.r3c2*b.r2c1 + a.r3c3*b.r3c1,
        a.r3c0*b.r0c2 + a.r3c1*b.r1c2 + a.r3c2*b.r2c2 + a.r3c3*b.r3c2,
        a.r3c0*b.r0c3 + a.r3c1*b.r1c3 + a.r3c2*b.r2c3 + a.r3c3*b.r3c3
    };
    return result;
}

Vec4 mul_vec4_f32(Vec4 v, f32 f) {
  Vec4 result = vec4_make(v.x*f,
                          v.y*f,
                          v.z*f);
  return result;
}