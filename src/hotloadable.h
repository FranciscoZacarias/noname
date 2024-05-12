#ifndef HOTLOAD_VARIABLES_H
#define HOTLOAD_VARIABLES_H

// Shader programs ---
#define DEFAULT_SHADER_VERTEX   "D:\\work\\noname\\src\\shaders\\default_shader.vertex"
#define DEFAULT_SHADER_FRAGMENT "D:\\work\\noname\\src\\shaders\\default_shader.fragment"
#define SCREEN_SHADER_VERTEX    "D:\\work\\noname\\src\\shaders\\screen_shader.vertex"
#define SCREEN_SHADER_FRAGMENT  "D:\\work\\noname\\src\\shaders\\screen_shader.fragment"

global u64 DefaultShaderVertexLastModified   = 0;
global u64 DefaultShaderFragmentLastModified = 0;
global u64 ScreenShaderVertexLastModified    = 0;
global u64 ScreenShaderFragmentLastModified  = 0;

function void hotload_shader_programs(Arena* arena);

// Variables ---
#define VARIABLES_TWEAK_FILE "D:\\work\\noname\\Variables.hotload"
global u64 VariablesTweakFileLastModified = 0;

function void hotload_variables(Arena* arena);
function f32 cast_string_to_f32(String str);

#endif // HOTLOAD_VARIABLES_H