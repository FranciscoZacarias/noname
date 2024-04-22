#define SHADER_SOURCE(code) "#version 330 core\n" #code

// Vertex Shader
const char *VertexShaderCode = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"\n"
"out vec3 line_color;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main() {\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    line_color = aColor;\n"
"}";

// Fragment Shader
const char *FragmentShaderCode = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"uniform vec3 color;\n"
"\n"
"void main() {\n"
"    FragColor = vec4(color, 1.0f);\n"
"}";

// Fragment Shader Line Color from Vertex
const char *FragmentShaderLineColorFromVertexCode = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 line_color;\n"
"\n"
"void main() {\n"
"    FragColor = vec4(line_color, 1.0f);\n"
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
