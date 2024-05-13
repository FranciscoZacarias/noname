#version 330 core
out vec4 FragColor;

uniform sampler2D screen_texture;
uniform int window_width;
uniform int window_height;

void main() {
  vec2 tex_coords = gl_FragCoord.xy / vec2(window_width, window_height);
  FragColor = texture(screen_texture, tex_coords);
}