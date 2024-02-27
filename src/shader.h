/* date = January 28th 2024 8:00 pm */

#ifndef SHADER_H
#define SHADER_H

#define SHADER_SOURCE(code) "#version 330 core\n" #code

typedef struct Shader {
  u32 id;
} Shader;

Shader shader_create(const char* vertex_path, const char* fragment_path);
void shader_use(Shader shader);

// void shader_set_uniform_mat4fv(Shader shader, const char* uniform, Mat4f32 mat);
// void shader_set_uniform_vec4fv(Shader shader, const char* uniform, Vec4f32 vec);

#endif //SHADER_H
