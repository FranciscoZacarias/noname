//////////////////////////////////////////////
// Vector 3

function Vec3f32 _v3f32(f32 x, f32 y, f32 z)            {Vec3f32 v={x, y, z};                        return v;}
function Vec3f32 add_v3f32_v3f32(Vec3f32 a, Vec3f32 b)  {Vec3f32 v={a.x+b.x, a.y+b.y, a.z+b.z};      return v;}
function Vec3f32 sub_v3f32_v3f32(Vec3f32 a, Vec3f32 b)  {Vec3f32 v={a.x-b.x, a.y-b.y, a.z-b.z};      return v;}
function Vec3f32 mul_v3f32_v3f32(Vec3f32 a, Vec3f32 b)  {Vec3f32 v={a.x*b.x, a.y*b.y, a.z*b.z};      return v;}
function Vec3f32 div_v3f32_v3f32(Vec3f32 a, Vec3f32 b)  {Vec3f32 v={a.x/b.x, a.y/b.y, a.z/b.z};      return v;}
function Vec3f32 scale_v3f32(Vec3f32 a, f32 s)          {Vec3f32 v={a.x*s,   a.y*s,   a.z*s};        return v;}
function Vec3f32 normalize_v3f32(Vec3f32 v)             {Vec3f32 r=scale_v3f32(v, 1.f/len_v3f32(v)); return r;}
function f32     len_v3f32(Vec3f32 v)                   {f32 r=sqrt(v.x*v.x + v.y*v.y + v.z*v.z);    return r;}
function f32     dot_v3f32(Vec3f32 a, Vec3f32 b)        {f32 r=(a.x*b.x + a.y*b.y + a.z*b.z);        return r;}
function Vec3f32 cross_v3f32(Vec3f32 a, Vec3f32 b)      {Vec3f32 v={a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; return v;}

//////////////////////////////////////////////
// Vector 4

function Vec4f32 _vec_4f32(f32 x, f32 y, f32 z, f32 w)  {Vec4f32 v={x, y, z, w};                            return v;}
function Vec4f32 add_v4f32_v4f32(Vec4f32 a, Vec4f32 b)  {Vec4f32 v={a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};    return v;}
function Vec4f32 sub_v4f32_v4f32(Vec4f32 a, Vec4f32 b)  {Vec4f32 v={a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};    return v;}
function Vec4f32 mul_v4f32_v4f32(Vec4f32 a, Vec4f32 b)  {Vec4f32 v={a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};    return v;}
function Vec4f32 div_v4f32_v4f32(Vec4f32 a, Vec4f32 b)  {Vec4f32 v={a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w};    return v;}
function Vec4f32 scale_v4f32(Vec4f32 a, f32 s)          {Vec4f32 v={a.x*s,   a.y*s,   a.z*s,   a.w*s};      return v;}
function Vec4f32 normalize_v4f32(Vec4f32 v)             {Vec4f32 r=scale_v4f32(v, 1.f/len_v4f32(v));        return r;}
function f32     len_v4f32(Vec4f32 v)                   {f32 r=sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w); return r;}
function f32     dot_v4f32(Vec4f32 a, Vec4f32 b)        {f32 r=(a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);     return r;}

//////////////////////////////////////////////
// Matrix 4

function Mat4f32 mat4f32(f32 diagonal) {
  Mat4f32 m = {0};
  m.v[0][0] = diagonal;
  m.v[1][1] = diagonal;
  m.v[2][2] = diagonal;
  m.v[3][3] = diagonal;
  return m;
}

function Mat4f32 mat4f32_make_translate(Vec3f32 v) {
  Mat4f32 m = mat4f32(1.f);
  m.v[3][0] = v.x;
  m.v[3][1] = v.y;
  m.v[3][2] = v.z;
  return m;
}

function Mat4f32 mat4f32_make_scale(Vec3f32 v) {
  Mat4f32 m = mat4f32(1.f);
  m.v[0][0] = v.x;
  m.v[1][1] = v.y;
  m.v[2][2] = v.z;
  return m;
}

function Mat4f32 mat4f32_make_perspective(f32 fov_degrees, f32 aspect, f32 near_z, f32 far_z) {
  Mat4f32 m = mat4f32(1.f);
  f32 tan_theta_over_2 = tanf(radians_from_degrees(fov_degrees) / 2);
  m.v[0][0] = 1.f / tan_theta_over_2;
  m.v[1][1] = aspect / tan_theta_over_2;
  m.v[2][3] = 1.f;
  m.v[2][2] = -(near_z + far_z) / (near_z - far_z);
  m.v[3][2] = (2.f * near_z * far_z) / (near_z - far_z);
  m.v[3][3] = 0.f;
  return m;
}

function Mat4f32 mat4f32_make_look_at(Vec3f32 eye, Vec3f32 target, Vec3f32 up) {
  Vec3f32 f = normalize_v3f32(sub_v3f32_v3f32(eye, target));
  Vec3f32 s = normalize_v3f32(cross_v3f32(f, up));
  Vec3f32 u = cross_v3f32(s, f);
  Mat4f32 m = {
    s.x,                u.x,              -f.x,               0.f,
    s.y,                u.y,              -f.y,               0.f,
    s.z,                u.z,              -f.z,               0.f,
   -dot_v3f32(s, eye), -dot_v3f32(u, eye), dot_v3f32(f, eye), 1.f
  };
  return m;
}

function Mat4f32 mat4f32_make_rotate(Vec3f32 axis, f32 degrees) {
  Mat4f32 m = mat4f32(1.f);
  Vec3f32 a = normalize_v3f32(axis);
  f32 radians   = radians_from_degrees(degrees);
  f32 sin_theta = sin(radians);
  f32 cos_theta = cos(radians);
  f32 cos_value = 1.f - cos_theta;
  m.v[0][0] = (a.x * a.x * cos_value) + cos_theta;
  m.v[0][1] = (a.x * a.y * cos_value) + (a.z * sin_theta);
  m.v[0][2] = (a.x * a.z * cos_value) - (a.y * sin_theta);
  m.v[1][0] = (a.y * a.x * cos_value) - (a.z * sin_theta);
  m.v[1][1] = (a.y * a.y * cos_value) + cos_theta;
  m.v[1][2] = (a.y * a.z * cos_value) + (a.x * sin_theta);
  m.v[2][0] = (a.z * a.x * cos_value) + (a.y * sin_theta);
  m.v[2][1] = (a.z * a.y * cos_value) - (a.x * sin_theta);
  m.v[2][2] = (a.z * a.z * cos_value) + cos_theta;
  return m;
}

function Mat4f32 mul_mat4f32_mat4f32(Mat4f32 a, Mat4f32 b) {
  Mat4f32 m = {0};
  for(int j = 0; j < 4; j++) {
    for(int i = 0; i < 4; i++) {
      m.v[i][j] = (a.v[0][j]*b.v[i][0] +
                   a.v[1][j]*b.v[i][1] +
                   a.v[2][j]*b.v[i][2] +
                   a.v[3][j]*b.v[i][3]);
    }
  }
  return m;
}