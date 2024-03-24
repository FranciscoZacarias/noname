#ifndef SHADER_H
#define SHADER_H

#include "shader_source.c"

function const char* GET_VERTEX_SHADER();
function const char* GET_FRAGMENT_SHADER();

typedef struct Shader {
  u32 id;
} Shader;

function Shader shader_create(const char* vertex_path, const char* fragment_path);
function void shader_use(Shader shader);
function void shader_set_uniform_mat4fv(Shader shader, const char* uniform, Mat4f32 mat);
function void shader_set_uniform_vec4fv(Shader shader, const char* uniform, Vec4f32 vec);
function void shader_set_uniform_vec3fv(Shader shader, const char* uniform, Vec3f32 vec);

#endif //SHADER_H
