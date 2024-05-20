#version 330 core

layout (location = 0) in vec3  a_pos;
layout (location = 1) in vec4  a_color;
layout (location = 2) in vec2  a_uv;
layout (location = 3) in float a_texture_index;
layout (location = 4) in float a_has_texture;

out vec4       vertex_color;
out vec2       vertex_uv;
out float      vertex_texture_index;
flat out float vertex_has_texture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  // NOTE(fz): This is a hack to keep text in screen space. 
  // If we start using textures beyond text, we need to find a better way of doing this
  if (a_has_texture > 0) {
    gl_Position  = vec4(a_pos, 1.0);
  } else {
    gl_Position  = projection * view * model * vec4(a_pos, 1.0);
  }
  
  // out
  vertex_color         = a_color;
  vertex_uv            = a_uv;
  vertex_texture_index = a_texture_index;
  vertex_has_texture   = a_has_texture;
}