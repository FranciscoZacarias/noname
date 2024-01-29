/* date = January 28th 2024 8:00 pm */

typedef enum ShaderCompileType {
  ShaderCompileType_Program,
  ShaderCompileType_Vertex,
  ShaderCompileType_Fragment
} ShaderCompileType;

internal void shader_check_errors(u32 shader, ShaderCompileType shader_type) {
  int  success;
  char infoLog[1024];
  
  switch(shader_type) {
    case ShaderCompileType_Program: {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if(!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        printf("Error %d linking shader program. Log: %s", success, infoLog);
        return;
      }
    } break;
    
    case ShaderCompileType_Vertex:
    case ShaderCompileType_Fragment: {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        printf("Error %d compiling vertex shader. Log: %s", success, infoLog);
        return;
      }   
    } break;    
  }
}

void shader_create(Shader* shader, const char* vertex_path, const char* fragment_path) {
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_path, NULL);
  glCompileShader(vertex_shader);
  shader_check_errors(vertex_shader, ShaderCompileType_Vertex);
  
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_path, NULL);
  glCompileShader(fragment_shader);
  shader_check_errors(fragment_shader, ShaderCompileType_Fragment);
  
  shader->id = glCreateProgram();
  glAttachShader(shader->id, vertex_shader);
  glAttachShader(shader->id, fragment_shader);
  glLinkProgram(shader->id);
  shader_check_errors(shader->id, ShaderCompileType_Program);
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void shader_use(Shader shader) {
  glUseProgram(shader.id);
}