// Vertex Shader
const char *VertexShaderCode = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 aColor;\n"
"layout (location = 2) in vec3 bPos;\n"
"layout (location = 3) in vec4 bColor;\n"
"\n"
"out vec4 vertex_color;\n"
"\n"
"uniform int render_triangles;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main() {\n"
"  if (render_triangles == 1) {\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    vertex_color = aColor;\n"
"  } else {\n"
"    gl_Position = projection * view * model * vec4(bPos, 1.0);\n"
"    vertex_color = bColor;\n"
"  }\n"
"}";

// Fragment Shader
const char *FragmentShaderCode = 
"#version 330 core\n"
"layout (location = 0) out vec4 FragColor;\n"
"\n"
"in vec4 vertex_color;\n"
"\n"
"void main() {\n"
"  FragColor = vertex_color;\n"
"}";

// Screen Vertex Shader
const char *ScreenVertexShaderCode = 
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"\n"
"void main() {\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}";

// Screen Fragment Shader
const char *ScreenFragmentShaderCode = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D screen_texture;\n"
"uniform int window_width;\n"
"uniform int window_height;\n"
"\n"
"void main() {\n"
"    vec2 tex_coords = gl_FragCoord.xy / vec2(window_width, window_height);\n"
"    FragColor = texture(screen_texture, tex_coords);\n"
"}";
