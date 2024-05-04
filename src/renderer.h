
#ifndef RENDERER_H
#define RENDERER_H

#define MSAA_SAMPLES 8

#define MAX_TRIANGLES 2048
#define MAX_VERTICES (MAX_TRIANGLES * 2)

typedef struct RendererVertex {
  Vec3f32 position;
  Vec4f32 color;
} RendererVertex;

typedef struct Renderer {
  // Default shader program
  u32 shader_program;
  u32 vao;
  u32 vbo;

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

  RendererVertex triangle_data[MAX_VERTICES];
  u32 triangle_count;
} Renderer;

function Renderer renderer_init(s32 window_width, s32 window_height);
function void renderer_free(Renderer* renderer);
function void renderer_begin_frame(Renderer* renderer, Vec4f32 background_color);
function void renderer_end_frame(Renderer* renderer, s32 window_width, s32 window_height);
function void renderer_push_triangle(Renderer* renderer, Vec3f32 a_position, Vec4f32 a_color, Vec3f32 b_position, Vec4f32 b_color, Vec3f32 c_position, Vec4f32 c_color);
function void renderer_push_quad(Renderer* renderer, Quad quad, Vec4f32 color);
function void renderer_push_cube(Renderer* renderer, Cube cube, Vec4f32 border_color);

function void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat);
function void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s);

#endif // RENDERER_H