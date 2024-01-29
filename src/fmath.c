
//////////////////////////////////////////////
// Vector

Vec4 vector4_new(f32 x, f32 y, f32 z) {
  Vec4 vec;
  vec.x = x;
  vec.y = y;
  vec.z = z;
  vec.w = 1.0f;
  return vec;
}


//////////////////////////////////////////////
// Matrix

Mat4 matrix_identity() {
  Mat4 mat = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  return mat;
}


//////////////////////////////////////////////
// Operations

Mat4 matrix_translate(Mat4 mat, Vec4 vec) {
  Mat4 translation = mat;
  translation.raw[3]  = vec.x;
  translation.raw[7]  = vec.y;
  translation.raw[11] = vec.z;
  translation.raw[15] = vec.w;
  return translation;
}

Vec4 mul_matrix_vector(Mat4 mat, Vec4 vec) {
  Vec4 result = {0};
  result.x = 
    mat.data[0][0]*vec.x +
    mat.data[0][1]*vec.y +
    mat.data[0][2]*vec.z +
    mat.data[0][3]*vec.w;
  result.y = 
    mat.data[1][0]*vec.x +
    mat.data[1][1]*vec.y +
    mat.data[1][2]*vec.z +
    mat.data[1][3]*vec.w;
  result.z = 
    mat.data[2][0]*vec.x +
    mat.data[2][1]*vec.y +
    mat.data[2][2]*vec.z +
    mat.data[2][3]*vec.w;
  result.w = 
    mat.data[3][0]*vec.x +
    mat.data[3][1]*vec.y +
    mat.data[3][2]*vec.z +
    mat.data[3][3]*vec.w;
  return result;
}
