#ifndef HOTLOAD_VARIABLES_H
#define HOTLOAD_VARIABLES_H

// Shader programs ---

global b32 FirstEntry = true; // To setup the hotload_shader_programs function.
global u64 DefaultVertexShaderLastModified   = 0;
global u64 DefaultFragmentShaderLastModified = 0;
global u64 ScreenVertexShaderLastModified    = 0;
global u64 ScreenFragmentShaderLastModified  = 0;

function void hotload_shader_programs(Arena* arena, Renderer* renderer);

// Variables ---
#define VARIABLES_TWEAK_FILE "D:\\work\\noname\\Variables.hotload"
global u64 VariablesTweakFileLastModified = 0;

function void hotload_variables(Arena* arena);
function f32 cast_string_to_f32(String str, f32* value);

#endif // HOTLOAD_VARIABLES_H