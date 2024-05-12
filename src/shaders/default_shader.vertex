#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 bPos;
layout (location = 3) in vec4 bColor;

out vec4 vertex_color;

uniform int render_triangles;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  if (render_triangles == 1) {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertex_color = aColor;
  } else {
    gl_Position = projection * view * model * vec4(bPos, 1.0);
    vertex_color = bColor;
  }
}