
#ifndef RENDERER_H
#define RENDERER_H

#define MSAA_SAMPLES 8

#define MAX_TRIANGLES 2048
#define MAX_TRIANGLES_VERTICES (MAX_TRIANGLES * 3)
#define MAX_LINES 256
#define MAX_LINES_VERTICES (MAX_LINES * 2)

#define DEFAULT_VERTEX_SHADER   "D:\\work\\noname\\src\\shaders\\default_vertex_shader.glsl"
#define DEFAULT_FRAGMENT_SHADER "D:\\work\\noname\\src\\shaders\\default_fragment_shader.glsl"
#define SCREEN_VERTEX_SHADER    "D:\\work\\noname\\src\\shaders\\screen_vertex_shader.glsl"
#define SCREEN_FRAGMENT_SHADER  "D:\\work\\noname\\src\\shaders\\screen_fragment_shader.glsl"

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
} Renderer_Vertex;

typedef struct Renderer {
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

  Renderer_Vertex triangle_data[MAX_TRIANGLES_VERTICES];
  u32 triangle_count;
} Renderer;

function Renderer renderer_init(Arena* arena, s32 window_width, s32 window_height);
function void renderer_free(Renderer* renderer);

function void renderer_generate_msaa_and_intermidiate_buffers(Renderer* renderer, s32 window_width, s32 window_height);
function void renderer_recompile_default_shader(Arena* arena, Renderer* renderer);
function void renderer_recompile_screen_shader(Arena* arena, Renderer* renderer);

function void renderer_begin_frame(Renderer* renderer, Vec4f32 background_color);
function void renderer_end_frame(Renderer* renderer, s32 window_width, s32 window_height);

function void renderer_push_string(Renderer* renderer, Renderer_Font_Info* font_info, String str, Vec2f32 pos, Vec4f32 color);
function void renderer_push_triangle(Renderer* renderer, Vec3f32 a_position, Vec4f32 a_color, Vec3f32 b_position, Vec4f32 b_color, Vec3f32 c_position, Vec4f32 c_color);
function void renderer_push_arrow(Renderer* renderer, Vec3f32 a, Vec3f32 b, Vec4f32 color, f32 scale);
function void renderer_push_quad(Renderer* renderer, Quad quad, Vec4f32 color);
function void renderer_push_cube(Renderer* renderer, Cube cube, Vec4f32 border_color);
function void renderer_push_cube_highlight_face(Renderer* renderer, Cube cube, Vec4f32 border_color, Cube_Face highlight, Vec4f32 highlight_color);

function void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat);
function void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s);

#endif // RENDERER_H