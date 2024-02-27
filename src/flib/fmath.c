//////////////////////////////////////////////
// Vector 3

function Vec3f32 vec3f32(f32 x, f32 y, f32 z) {
  Vec3f32 v={x, y, z};
  return v;
}
function Vec3f32 add_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b) {
  Vec3f32 v={a.x+b.x, a.y+b.y, a.z+b.z}; 
  return v;
}
function Vec3f32 sub_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b) {
  Vec3f32 v={a.x-b.x, a.y-b.y, a.z-b.z}; 
  return v;
}
function Vec3f32 mul_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b) {
  Vec3f32 v={a.x*b.x, a.y*b.y, a.z*b.z}; 
  return v;
}
function Vec3f32 div_vec3f32_vec3f32(Vec3f32 a, Vec3f32 b) {
  Vec3f32 v={a.x/b.x, a.y/b.y, a.z/b.z}; 
  return v;
}
function Vec3f32 scale_vec3f32(Vec3f32 a, f32 s) {
  Vec3f32 v={a.x*s, a.y*s, a.z*s};
  return v;
}
function Vec3f32 normalize_vec3f32(Vec3f32 a) {
  Vec3f32 v=scale_vec3f32(a, 1.f/len_vec3f32(a)); 
  return v;
}
function f32 len_vec3f32(Vec3f32 v) {
  f32 r=sqrt(v.x*v.x + v.y*v.y + v.z*v.z); 
  return r;
}
function f32 dot_vec3f32(Vec3f32 a, Vec3f32 b) {
  f32 r=(a.x*b.x + a.y*b.y + a.z*b.z); 
  return r;
}
function Vec3f32 cross_vec3f32(Vec3f32 a, Vec3f32 b) {
  Vec3f32 v={a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; 
  return v;
}

//////////////////////////////////////////////
// Vector 4

function Vec4f32 vec4f32(f32 x, f32 y, f32 z, f32 w) {
  Vec4f32 v={x, y, z, w};
  return v;
}
function Vec4f32 add_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b) {
  Vec4f32 v={a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
  return v;
}
function Vec4f32 sub_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b) {
  Vec4f32 v={a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};
  return v;
}
function Vec4f32 mul_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b) {
  Vec4f32 v={a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};
  return v;
}
function Vec4f32 div_vec4f32_vec4f32(Vec4f32 a, Vec4f32 b) {
  Vec4f32 v={a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w};
  return v;
}
function Vec4f32 scale_vec4f32(Vec4f32 a, f32 s) {
  Vec4f32 v={a.x*s, a.y*s, a.z*s, a.w*s};
  return v;
}
function Vec4f32 normalize_vec4f32(Vec4f32 v) {
  Vec4f32 r=scale_vec4f32(v, 1.f/len_vec4f32(v));
  return r;
}
function f32 len_vec4f32(Vec4f32 v) {
  f32 r=sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
  return r;
}
function f32 dot_vec4f32(Vec4f32 a, Vec4f32 b) {
  f32 r=(a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
  return r;
}

//////////////////////////////////////////////
// Matrix 4

function Mat4f32 mat4f32(f32 diagonal) {
  Mat4f32 r = {0};
  r.v[0][0] = diagonal;
  r.v[1][1] = diagonal;
  r.v[2][2] = diagonal;
  r.v[3][3] = diagonal;
  return r;
}

function Mat4f32 mat4f32_make_scale(f32 x, f32 y, f32 z) {
  Mat4f32 r = mat4f32(0.0f);
  r.v[0][0] = x;
  r.v[1][1] = y;
  r.v[2][2] = z;
  r.v[3][3] = 1.f;
  return r;
}

function Mat4f32 mat4f32_scale(Mat4f32 m, f32 s) {
  Mat4f32 r = m;
  for(u32 i = 0; i < 4; i++) {
    r.v[i][0] *= s;
    r.v[i][1] *= s;
    r.v[i][2] *= s;
    r.v[i][3] *= s;
  }
  return r;
}

function Mat4f32 mat4f32_make_translate(f32 x, f32 y, f32 z) {
  Mat4f32 r = mat4f32(1.f);
  r.v[3][0] = x;
  r.v[3][1] = y;
  r.v[3][2] = z;
  return r;
}

function Mat4f32 mat4f32_translate(Mat4f32 m, f32 x, f32 y, f32 z) {
  Mat4f32 r = m;
  Vec4f32 translate = vec4f32(x, y, z, 0);
  for (u32 i = 0; i < 4; i++) {
    Vec4f32 row = vec4f32(m.v[i][0], m.v[i][1], m.v[i][2], m.v[i][3]);
    r.v[3][i] += dot_vec4f32(row, translate);
  }
  return r;
}

function Mat4f32 mat4f32_make_rotate(f32 x, f32 y, f32 z, f32 degrees) {
  Mat4f32 m     = mat4f32(1.f);
  Vec3f32 axis  = normalize_vec3f32(vec3f32(x, y, z));
  f32 radians   = radians_from_degrees(degrees);
  f32 sin_theta = sin(radians);
  f32 cos_theta = cos(radians);
  f32 cos_value = 1.f - cos_theta;
  m.v[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
  m.v[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta);
  m.v[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta);
  m.v[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta);
  m.v[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
  m.v[1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta);
  m.v[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta);
  m.v[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta);
  m.v[2][2] = (axis.z * axis.z * cos_value) + cos_theta;
  return m;
}

function Mat4f32 mat4f32_rotate(Mat4f32 m, f32 x, f32 y, f32 z, f32 degrees) {
  Mat4f32 r;
  f32 radians = radians_from_degrees(degrees);

  Vec3f32 axis  = vec3f32(x, y, z);
  axis = normalize_vec3f32(axis);

  Mat4f32 T = outer_vec3f32(axis, axis);

  Mat4f32 S = mat4f32(0.0f);
  S.v[0][1] =  axis.z;
  S.v[0][2] = -axis.y;
  S.v[1][0] = -axis.z;
  S.v[1][2] =  axis.x;
  S.v[2][0] =  axis.y;
  S.v[2][1] = -axis.x;
  S = mat4f32_scale(S, sinf(radians));

  Mat4f32 C = mat4f32(1.0f);
  C = sub_m4f32_m4f32(C, T);
  C = mat4f32_scale(C, cosf(radians));

  T = add_m4f32_m4f32(T, C);
  T = add_m4f32_m4f32(T, S);
  T.v[3][3] = 1.0f;

  r = mul_mat4f32_mat4f32(m, T);
  return r;
}

function Mat4f32 mat4f32_perspective(f32 fov_degrees, f32 aspect, f32 n, f32 f) {
  Mat4f32 r;
  f32 radians = radians_from_degrees(fov_degrees);
  f32 a = 1.f / tanf(radians / 2.f);

  r.v[0][0] = a / aspect;
	r.v[0][1] = 0.f;
	r.v[0][2] = 0.f;
	r.v[0][3] = 0.f;

	r.v[1][0] = 0.f;
	r.v[1][1] = a;
	r.v[1][2] = 0.f;
	r.v[1][3] = 0.f;

	r.v[2][0] = 0.f;
	r.v[2][1] = 0.f;
	r.v[2][2] = -((f+n) / (f-n));
	r.v[2][3] = -1.f;

	r.v[3][0] = 0.f;
	r.v[3][1] = 0.f;
	r.v[3][2] = -((2.f*f*n) / (f-n));
	r.v[3][3] = 0.f;
  
  return r;
}

function Mat4f32 mat4f32_look_at(Vec3f32 eye, Vec3f32 target, Vec3f32 up) {
  Mat4f32 r = mat4f32(0.f);

  Vec3f32 f = sub_vec3f32_vec3f32(target, eye);
  f = normalize_vec3f32(f);
  Vec3f32 s = cross_vec3f32(f, up);
  s = normalize_vec3f32(s);
  Vec3f32 t = cross_vec3f32(s, f);

	r.v[0][0] =  s.x;
	r.v[0][1] =  t.x;
	r.v[0][2] = -f.x;
	r.v[0][3] =  0.f;

	r.v[1][0] =  s.y;
	r.v[1][1] =  t.y;
	r.v[1][2] = -f.y;
	r.v[1][3] =  0.f;

	r.v[2][0] =  s.z;
	r.v[2][1] =  t.z;
	r.v[2][2] = -f.z;
	r.v[2][3] =  0.f;

	r.v[3][0] = 0.f;
	r.v[3][1] = 0.f;
	r.v[3][2] = 0.f;
	r.v[3][3] = 1.f;

  Mat4f32 trans = mat4f32_make_translate(-eye.x, -eye.y, -eye.z);
  return mul_mat4f32_mat4f32(r, trans);
}

function Mat4f32 mat4f32_transpose(Mat4f32 m) {
  Mat4f32 r = m;
  for(u32 j = 0; j < 4; j++) {
    for(u32 i = 0; i < 4; i++) {
      r.v[i][j] = m.v[j][i];
    }
  }
  return r;
}

function Mat4f32 mat4f32_invert(Mat4f32 m) {
  Mat4f32 r;

  f32 s[6];
	s[0] = m.v[0][0]*m.v[1][1] - m.v[1][0]*m.v[0][1];
	s[1] = m.v[0][0]*m.v[1][2] - m.v[1][0]*m.v[0][2];
	s[2] = m.v[0][0]*m.v[1][3] - m.v[1][0]*m.v[0][3];
	s[3] = m.v[0][1]*m.v[1][2] - m.v[1][1]*m.v[0][2];
	s[4] = m.v[0][1]*m.v[1][3] - m.v[1][1]*m.v[0][3];
	s[5] = m.v[0][2]*m.v[1][3] - m.v[1][2]*m.v[0][3];

	f32 c[6];
	c[0] = m.v[2][0]*m.v[3][1] - m.v[3][0]*m.v[2][1];
	c[1] = m.v[2][0]*m.v[3][2] - m.v[3][0]*m.v[2][2];
	c[2] = m.v[2][0]*m.v[3][3] - m.v[3][0]*m.v[2][3];
	c[3] = m.v[2][1]*m.v[3][2] - m.v[3][1]*m.v[2][2];
	c[4] = m.v[2][1]*m.v[3][3] - m.v[3][1]*m.v[2][3];
	c[5] = m.v[2][2]*m.v[3][3] - m.v[3][2]*m.v[2][3];
	
	// Assumes it is invertible
	float idet = 1.0f/(s[0]*c[5]-s[1]*c[4]+s[2]*c[3]+s[3]*c[2]-s[4]*c[1]+s[5]*c[0]);
	
	r.v[0][0] = ( m.v[1][1] * c[5] - m.v[1][2] * c[4] + m.v[1][3] * c[3]) * idet;
	r.v[0][1] = (-m.v[0][1] * c[5] + m.v[0][2] * c[4] - m.v[0][3] * c[3]) * idet;
	r.v[0][2] = ( m.v[3][1] * s[5] - m.v[3][2] * s[4] + m.v[3][3] * s[3]) * idet;
	r.v[0][3] = (-m.v[2][1] * s[5] + m.v[2][2] * s[4] - m.v[2][3] * s[3]) * idet;

	r.v[1][0] = (-m.v[1][0] * c[5] + m.v[1][2] * c[2] - m.v[1][3] * c[1]) * idet;
	r.v[1][1] = ( m.v[0][0] * c[5] - m.v[0][2] * c[2] + m.v[0][3] * c[1]) * idet;
	r.v[1][2] = (-m.v[3][0] * s[5] + m.v[3][2] * s[2] - m.v[3][3] * s[1]) * idet;
	r.v[1][3] = ( m.v[2][0] * s[5] - m.v[2][2] * s[2] + m.v[2][3] * s[1]) * idet;

	r.v[2][0] = ( m.v[1][0] * c[4] - m.v[1][1] * c[2] + m.v[1][3] * c[0]) * idet;
	r.v[2][1] = (-m.v[0][0] * c[4] + m.v[0][1] * c[2] - m.v[0][3] * c[0]) * idet;
	r.v[2][2] = ( m.v[3][0] * s[4] - m.v[3][1] * s[2] + m.v[3][3] * s[0]) * idet;
	r.v[2][3] = (-m.v[2][0] * s[4] + m.v[2][1] * s[2] - m.v[2][3] * s[0]) * idet;

	r.v[3][0] = (-m.v[1][0] * c[3] + m.v[1][1] * c[1] - m.v[1][2] * c[0]) * idet;
	r.v[3][1] = ( m.v[0][0] * c[3] - m.v[0][1] * c[1] + m.v[0][2] * c[0]) * idet;
	r.v[3][2] = (-m.v[3][0] * s[3] + m.v[3][1] * s[1] - m.v[3][2] * s[0]) * idet;
	r.v[3][3] = ( m.v[2][0] * s[3] - m.v[2][1] * s[1] + m.v[2][2] * s[0]) * idet;

  return r;
}

function Mat4f32 mul_mat4f32_mat4f32(Mat4f32 a, Mat4f32 b) {
  Mat4f32 r = {0};
  for(int j = 0; j < 4; j++) {
    for(int i = 0; i < 4; i++) {
      r.v[i][j] = (a.v[0][j]*b.v[i][0] +
                   a.v[1][j]*b.v[i][1] +
                   a.v[2][j]*b.v[i][2] +
                   a.v[3][j]*b.v[i][3]);
    }
  }
  return r;
}

function Mat4f32 add_m4f32_m4f32(Mat4f32 a, Mat4f32 b) {
  Mat4f32 r;
  for(int i = 0; i < 4; i++) {
    r.v[i][0] = a.v[i][0] + b.v[i][0];
    r.v[i][1] = a.v[i][1] + b.v[i][1];
    r.v[i][2] = a.v[i][2] + b.v[i][2];
    r.v[i][3] = a.v[i][3] + b.v[i][3];
  }
  return r;
}

function Mat4f32 sub_m4f32_m4f32(Mat4f32 a, Mat4f32 b) {
  Mat4f32 r;
  for(int i = 0; i < 4; i++) {
    r.v[i][0] = a.v[i][0] - b.v[i][0];
    r.v[i][1] = a.v[i][1] - b.v[i][1];
    r.v[i][2] = a.v[i][2] - b.v[i][2];
    r.v[i][3] = a.v[i][3] - b.v[i][3];
  }
  return r;
}

function Mat4f32 outer_vec3f32(Vec3f32 a, Vec3f32 b) {
  Mat4f32 r;
  r.v[0][0] = a.x * b.x;
  r.v[0][1] = a.x * b.y;
  r.v[0][2] = a.x * b.z;
  r.v[0][3] = 0.0f;

  r.v[1][0] = a.y * b.x;
  r.v[1][1] = a.y * b.y;
  r.v[1][2] = a.y * b.z;
  r.v[1][3] = 0.0f;

  r.v[2][0] = a.z * b.x;
  r.v[2][1] = a.z * b.y;
  r.v[2][2] = a.z * b.z;
  r.v[2][3] = 0.0f;

  r.v[3][0] = 0.0f;
  r.v[3][1] = 0.0f;
  r.v[3][2] = 0.0f;
  r.v[3][3] = 0.0f;
  return r;
}