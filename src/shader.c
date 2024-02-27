/* date = January 28th 2024 8:00 pm */

//////////////////////////////////////////////
// Vertex Shader
const char* GET_VERTEX_SHADER() {
  return SHADER_SOURCE(
      //////////////////////////////////////////////
      // Vertex Shader start

      layout (location = 0) in vec3 aPos;

      uniform mat4 model;
      uniform mat4 view;
      uniform mat4 projection;

      void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
      }

      // Vertex Shader end
      //////////////////////////////////////////////
    );
}

//////////////////////////////////////////////
// Fragment Shader
const char* GET_FRAGMENT_SHADER() {
  return SHADER_SOURCE(
      //////////////////////////////////////////////
      // Fragment  Shader start

      out vec4 FragColor;
      void main() {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
      }

      // Fragment Shader End
      //////////////////////////////////////////////
    );
}

typedef enum ShaderErrorType {
  ShaderErrorType_LinkProgram,
  ShaderErrorType_CompileVertex,
  ShaderErrorType_CompileFragment
} ShaderErrorType;

void _shader_check_errors(u32 shader, ShaderErrorType shader_type) {
  int  success;
  char infoLog[1024];
  switch(shader_type) {
    case ShaderErrorType_LinkProgram: {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if(!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        printf("Error %d linking shader program. Log: %s", success, infoLog);
        return;
      }
    } break;
    case ShaderErrorType_CompileVertex:
    case ShaderErrorType_CompileFragment: {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        printf("Error %d compiling vertex shader. Log: %s", success, infoLog);
        return;
      }   
    } break;
  }
}

Shader shader_create(const char* vertex_path, const char* fragment_path) {
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_path, NULL);
  glCompileShader(vertex_shader);
  _shader_check_errors(vertex_shader, ShaderErrorType_CompileVertex);
  
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_path, NULL);
  glCompileShader(fragment_shader);
  _shader_check_errors(fragment_shader, ShaderErrorType_CompileFragment);
  
  Shader result = { 0 };
  result.id = glCreateProgram();
  glAttachShader(result.id, vertex_shader);
  glAttachShader(result.id, fragment_shader);
  glLinkProgram(result.id);
  _shader_check_errors(result.id, ShaderErrorType_LinkProgram);
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return result;
}

void shader_use(Shader shader) {
  glUseProgram(shader.id);
}

void shader_set_uniform_mat4fv(Shader shader, const char* uniform, Mat4f32 mat) {
  s32 uniform_location = glGetUniformLocation(shader.id, uniform);
  if (uniform_location == -1) {
    printf("Uniform %s not found\n", uniform);
  }
  glUniformMatrix4fv(uniform_location, 1, 0, &mat.v[0][0]);
}

void shader_set_uniform_vec4fv(Shader shader, const char* uniform, Vec4f32 vec) {
  s32 uniform_location = glGetUniformLocation(shader.id, uniform);
  if (uniform_location == -1) {
    printf("Uniform %s not found\n", uniform);
  }
  glUniform4fv(uniform_location, 1, &vec.v[0]);
}