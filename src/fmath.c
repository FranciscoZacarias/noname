
//////////////////////////////////////////////
// Vector

Vec4 vec4_new(f32 x, f32 y, f32 z) {
  Vec4 result;
  result.x = x;
  result.y = y;
  result.z = z;
  result.w = 1.0f;
  return result;
}

Vec4 vec4_normalize(Vec4 vec) {
  Vec4 result;
  f32 mag = vec4_magnitude(vec);
  result.x = vec.x / mag;
  result.y = vec.y / mag;
  result.z = vec.z / mag;
  result.w = vec.w;
  return result;  
}

//////////////////////////////////////////////
// Matrix

Mat4 mat4_identity() {
  Mat4 result = {
     1.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 1.0f
   };
  return result;
}

Mat4 mat4_translate(Mat4 mat, Vec4 vec) {
  Mat4 result       = mat;
  result.data[0][3] = vec.x;
  result.data[1][3] = vec.y;
  result.data[2][3] = vec.z;
  return result;
}

Mat4 mat4_scale(Mat4 mat, Vec4 scale) {
  Mat4 result       = mat4_identity();
  result.data[0][0] = scale.x;
  result.data[1][1] = scale.y;
  result.data[2][2] = scale.z;
  return result;
}

Mat4 mat4_rotate(Mat4 mat, Vec4 axis, f32 degrees) {
  Mat4 result   = mat4_identity();
  f32 radians   = radians_from_degrees(degrees);
  axis          = vec4_normalize(axis);
  f32 sin_theta = sin(radians);
  f32 cos_theta = cos(radians);
  f32 not_cos   = 1 - cos_theta;
  result.data[0][0] = (axis.x*axis.x*not_cos) + cos_theta;
  result.data[0][1] = (axis.x*axis.y*not_cos) + (axis.z*sin_theta);
  result.data[0][2] = (axis.x*axis.z*not_cos) - (axis.y*sin_theta);
  result.data[1][0] = (axis.y*axis.x*not_cos) - (axis.z*sin_theta);
  result.data[1][1] = (axis.y*axis.y*not_cos) + cos_theta;
  result.data[1][2] = (axis.y*axis.z*not_cos) + (axis.x*sin_theta);
  result.data[2][0] = (axis.z*axis.x*not_cos) + (axis.y*sin_theta);
  result.data[2][1] = (axis.z*axis.y*not_cos) - (axis.x*sin_theta);
  result.data[2][2] = (axis.z*axis.z*not_cos) + cos_theta;
  return result;
}

//////////////////////////////////////////////
// Operations

Vec4 mul_mat4_vec4(Mat4 mat, Vec4 vec) {
  Vec4 result = {0};
  result.x = mat.data[0][0]*vec.x + mat.data[0][1]*vec.y + mat.data[0][2]*vec.z + mat.data[0][3]*vec.w;
  result.y = mat.data[1][0]*vec.x + mat.data[1][1]*vec.y + mat.data[1][2]*vec.z + mat.data[1][3]*vec.w;
  result.z = mat.data[2][0]*vec.x + mat.data[2][1]*vec.y + mat.data[2][2]*vec.z + mat.data[2][3]*vec.w;
  result.w = mat.data[3][0]*vec.x + mat.data[3][1]*vec.y + mat.data[3][2]*vec.z + mat.data[3][3]*vec.w;
  return result;
}

Mat4 mul_mat4_mat4(Mat4 a, Mat4 b) {
  Mat4 result;
  // TODO: Confirm if the iteration order is correct I.e. If we're being cache friendly
  for(u32 row = 0; row < 4; row++) {
    for(u32 col = 0; col < 4; col++) {
      result.data[col][row] = (a.data[0][row]*b.data[col][0] +
                               a.data[1][row]*b.data[col][1] +
                               a.data[2][row]*b.data[col][2] +
                               a.data[3][row]*b.data[col][3]);
    }
  }
  return result;
}
