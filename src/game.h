/* date = May 21st 2024 10:24 pm */

#ifndef GAME_H
#define GAME_H

typedef struct Cube_Under_Cursor {
	Cube_Face hovered_face;
	u32 index;
	f32 distance_to_camera;
} Cube_Under_Cursor;

typedef struct Game_State {
  Arena* arena;
  Cube* cubes;
  u32 total_cubes;
  
  Cube_Under_Cursor cube_under_cursor;
} Game_State;

global Game_State GameState;

internal void game_init();
internal void game_update(Camera camera, Vec3f32 raycast, b32 add_cube);

internal void game_push_cube(Cube cube);
internal b32 find_cube_under_cursor(Camera camera, Vec3f32 raycast, Cube_Under_Cursor* result);

#endif //GAME_H
