#ifndef HOTLOAD_VARIABLES_H
#define HOTLOAD_VARIABLES_H

// Shader programs ---

global u64 DefaultShaderVertexLastModified   = 0;
global u64 DefaultShaderFragmentLastModified = 0;
global u64 ScreenShaderVertexLastModified    = 0;
global u64 ScreenShaderFragmentLastModified  = 0;

function void hotload_shader_programs(Arena* arena);

// Variables ---
#define VARIABLES_TWEAK_FILE "D:\\work\\noname\\Variables.hotload"
global u64 VariablesTweakFileLastModified = 0;

function void hotload_variables(Arena* arena);
function f32 cast_string_to_f32(String str, f32* value);

#endif // HOTLOAD_VARIABLES_H