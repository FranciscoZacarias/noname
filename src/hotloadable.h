#ifndef HOTLOAD_VARIABLES_H
#define HOTLOAD_VARIABLES_H

// Shader programs ---

global b32 FirstEntry = 1; // To setup the hotload_shader_programs function.
global u64 DefaultVertexShaderLastModified   = 0;
global u64 DefaultFragmentShaderLastModified = 0;
global u64 ScreenVertexShaderLastModified    = 0;
global u64 ScreenFragmentShaderLastModified  = 0;

internal void hotload_shader_programs(Renderer* renderer, f64 current_time);

// Variables ---
#define VARIABLES_TWEAK_FILE "D:\\work\\noname\\Variables.hotload"
global u64 VariablesTweakFileLastModified = 0;
global b32 StartupVariablesLoaded = 0;

internal void hotload_variables(Program_State* program_state);

#endif // HOTLOAD_VARIABLES_H