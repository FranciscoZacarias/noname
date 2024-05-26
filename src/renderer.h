
#ifndef RENDERER_H
#define RENDERER_H

#define MSAA_SAMPLES 8

#define DEFAULT_VERTEX_SHADER   "D:\\work\\noname\\src\\shaders\\default_vertex_shader.glsl"
#define DEFAULT_FRAGMENT_SHADER "D:\\work\\noname\\src\\shaders\\default_fragment_shader.glsl"
#define SCREEN_VERTEX_SHADER    "D:\\work\\noname\\src\\shaders\\screen_vertex_shader.glsl"
#define SCREEN_FRAGMENT_SHADER  "D:\\work\\noname\\src\\shaders\\screen_fragment_shader.glsl"

global b32 HotloadableEnableWireframeMode = 0;
global b32 HotloadableEnableCulling = 1;

typedef struct Renderer_Font_Info {
	u32 font_texture;
  stbtt_packedchar cdata[95];
  f32 scale;
  f32 font_size;
  s32 ascent;
  s32 descent;
  s32 baseline;
} Renderer_Font_Info;

typedef struct Renderer_Vertex {
  Vec3f32 position;
  Vec4f32 color;
  Vec2f32 uv;
  f32 texture_index;
  f32 has_texture;
} Renderer_Vertex;

typedef struct Renderer {
  Program_State* program_state;
  Renderer_Font_Info font_info;
  
  // Default shader program
  u32 shader_program;
  u32 vao;
  u32 triangle_vbo;
  
  /* Default program renders to the MSAA offscreen buffer, 
     to apply anti aliasing */
  
  // MSAA offscreen buffer
  u32 msaa_frame_buffer_object;
  u32 msaa_texture_color_buffer_multisampled;
  u32 msaa_render_buffer_object;
  
  /* Blit (copy) from MSAA buffer into the pos processing (intermidiate)
     frame buffer, to apply post processing */
  
  // Post processing offscreen buffer
  u32 postprocessing_fbo;
  u32 screen_texture;
  
  /* Then we render again to a texture that fills the screen */
  
  // Screen shader program
  u32 screen_program;
  u32 screen_vao;
  u32 screen_vbo;
  
  // Data
  Arena* arena;
  Renderer_Vertex* triangles_data;
  u32 triangles_count;
  u32 triangles_max;
  
  Renderer_Vertex* triangles_front_data;
  u32 triangles_front_count;
  u32 triangles_front_max;
  
	u32* textures;
  u32 textures_count;
  u32 textures_max;
} Renderer;

global Renderer ProgramRenderer;

internal Renderer renderer_init(Program_State* program_state);
internal void renderer_free(Renderer* renderer);

//~ Helpers
internal void renderer_generate_msaa_and_intermidiate_buffers(Renderer* renderer);
internal void renderer_recompile_default_shader(Arena* arena, Renderer* renderer);
internal void renderer_recompile_screen_shader(Arena* arena, Renderer* renderer);

//~ Setup
internal b32  renderer_font_load(Renderer_Font_Info* font_info, String file_path, f32 font_size);
internal u32  renderer_texture_load(String file_path);
internal void renderer_begin_frame(Renderer* renderer, Vec4f32 background_color);
internal void renderer_end_frame(Renderer* renderer, Mat4f32 view, Mat4f32 projection);

//~ Push 3D to Renderer
internal void renderer_push_triangle(Renderer* renderer, Vec3f32 a_position, Vec4f32 a_color, Vec3f32 b_position, Vec4f32 b_color, Vec3f32 c_position, Vec4f32 c_color, b32 bring_to_front);
internal void renderer_push_triangle_texture(Renderer* renderer, Vec3f32 a_position, Vec2f32 a_uv, Vec3f32 b_position, Vec2f32 b_uv, Vec3f32 c_position, Vec2f32 c_uv, u32 texture, b32 bring_to_front);
internal void renderer_push_arrow(Renderer* renderer, Arrow arrow, b32 bring_to_front);
internal void renderer_push_quad(Renderer* renderer, Quad quad, Vec4f32 color, b32 bring_to_front);
internal void renderer_push_quad_texture(Renderer* renderer, Quad quad, u32 texture, b32 bring_to_front);

//~ Push 3D structures to renderer
internal void renderer_push_cube(Renderer* renderer, Cube cube, b32 bring_to_front);
internal void renderer_push_cube_highlight_face(Renderer* renderer, Cube cube, Cube_Face highlight, Vec4f32 highlight_color, b32 bring_to_front);

internal void renderer_push_translation_gizmo(Renderer* renderer, GizmoTranslation gizmo, b32 bring_to_front);

//~ Push 2D to Renderer
internal void renderer_push_string(Renderer* renderer, String text, Vec2f32 position, Vec4f32 color, b32 bring_to_front);

//~ Shader Uniforms
internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat);
internal void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s);

#endif // RENDERER_H