/* date = May 21st 2024 10:24 pm */

#ifndef GAME_H
#define GAME_H

//~ TODO(fz): Mouse_State and Program_State shouldn't be in game.h.
// Need to find a better place to put them, that is not the main file.
typedef struct Mouse_State {
	f32 screen_space_x;
	f32 screen_space_y;
  
	f32 ndc_x;
	f32 ndc_y;
  
  f32 last_x;
  f32 last_y;
} Mouse_State;

typedef struct Program_State {
  f64 current_time;
  f64 delta_time;
  f64 last_frame;
  
  s32 window_width;
  s32 window_height;
  b32 show_debug_stats;
  f32 near_plane;
  f32 far_plane;
  
  Camera camera;
  Mouse_State mouse;
  Vec3f32 raycast;
  
} Program_State;


typedef struct Cube_Under_Cursor {
	Cube_Face hovered_face;
	u32 index;
	f32 distance_to_camera;
} Cube_Under_Cursor;

typedef struct Game_State {
  Arena* arena;
  Cube* cubes;
  u32 total_cubes;
  u32 max_cubes;
  
  u32* empty_cube_slots;
  u32 total_empty_cube_slots;
  
  Cube_Under_Cursor cube_under_cursor;
} Game_State;

global Game_State GameState;

internal void game_init();
internal void game_update(Camera* camera, Vec3f32 raycast, b32 add_cube, b32 remove_cube);

internal u32 game_cubes_alive_count();

internal void game_push_cube(Cube cube);
internal void game_remove_cube(u32 index);
internal b32 find_cube_under_cursor(Camera camera, Vec3f32 raycast, Cube_Under_Cursor* result);

#endif //GAME_H
