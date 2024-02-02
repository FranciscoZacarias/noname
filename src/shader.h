/* date = January 28th 2024 8:00 pm */

#ifndef SHADER_H
#define SHADER_H

#define VERTEX_SHADER(code)   "#version 330 core\n" #code
#define FRAGMENT_SHADER(code) "#version 330 core\n" #code

typedef struct Shader {
  u32 id;
} Shader;

void shader_create(Shader* shader, const char* vertex_path, const char* fragment_path);
void shader_use(Shader shader);

void shader_set_uniform_mat4fv(Shader shader, const char* uniform, Mat4 mat);

#endif //SHADER_H
