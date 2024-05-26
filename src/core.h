/* date = May 24th 2024 11:40 am */

#ifndef CORE_H
#define CORE_H

typedef struct Program_State {
  f64 current_time;
  f64 delta_time;
  f64 last_frame;
  
  s32 window_width;
  s32 window_height;
  b32 show_debug_stats;
  f32 near_plane;
  f32 far_plane;
  
  Mat4f32 view;
  Mat4f32 projection;
  
  Camera camera;
  Vec3f32 raycast;
  
  b32 program_is_running;
} Program_State;

global Program_State ProgramState;

internal void program_init();
internal void program_update();

#endif //CORE_H
