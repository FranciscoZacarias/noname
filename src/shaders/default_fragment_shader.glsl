#version 330 core

layout (location = 0) out vec4 FragColor;

in vec4  vertex_color;
in vec2  vertex_uv;
in float vertex_texture_index;
flat in int  vertex_has_texture;

uniform sampler2D u_texture[8];

void main() {
  if (vertex_has_texture > 0) {
    switch (int(vertex_texture_index)) {
      case 0: FragColor = vertex_color * texture(u_texture[0], vertex_uv); break;
      case 1: FragColor = vertex_color * texture(u_texture[1], vertex_uv); break;
      case 2: FragColor = vertex_color * texture(u_texture[2], vertex_uv); break;
      case 3: FragColor = vertex_color * texture(u_texture[3], vertex_uv); break;
      case 4: FragColor = vertex_color * texture(u_texture[4], vertex_uv); break;
      case 5: FragColor = vertex_color * texture(u_texture[5], vertex_uv); break;
      case 6: FragColor = vertex_color * texture(u_texture[6], vertex_uv); break;
      case 7: FragColor = vertex_color * texture(u_texture[7], vertex_uv); break;
      default: discard;
    }
  } else {
    FragColor = vertex_color;
  }
}