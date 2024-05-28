/* date = May 21st 2024 10:24 pm */

#ifndef GAME_H
#define GAME_H

//~ Game types

//   3--------2
//  /|       /|
// 7--------6 |
// | 0------|-1
// |/       |/
// 4--------5
//
// Each vertex number should correspond to an index
// in any implementation logic for this Cube structure

typedef enum Cube_Face {
  CubeFace_Back,
  CubeFace_Front,
  CubeFace_Left,
  CubeFace_Right,
  CubeFace_Bottom,
  CubeFace_Top
} Cube_Face;

typedef struct Cube_Vertices {
  Vec3f32 p0;
  Vec3f32 p1;
  Vec3f32 p2;
  Vec3f32 p3;
  Vec3f32 p4;
  Vec3f32 p5;
  Vec3f32 p6;
  Vec3f32 p7;
} Cube_Vertices;

global Cube_Vertices CubeVerticesLocalSpace = {
  { -1.0f, -1.0f, -1.0f },
  {  1.0f, -1.0f, -1.0f },
  {  1.0f,  1.0f, -1.0f },
  { -1.0f,  1.0f, -1.0f },
  { -1.0f, -1.0f,  1.0f },
  {  1.0f, -1.0f,  1.0f },
  {  1.0f,  1.0f,  1.0f },
  { -1.0f,  1.0f,  1.0f }
};

// Local space only!
#define P0 CubeVerticesLocalSpace.p0
#define P1 CubeVerticesLocalSpace.p1
#define P2 CubeVerticesLocalSpace.p2
#define P3 CubeVerticesLocalSpace.p3
#define P4 CubeVerticesLocalSpace.p4
#define P5 CubeVerticesLocalSpace.p5
#define P6 CubeVerticesLocalSpace.p6
#define P7 CubeVerticesLocalSpace.p7

typedef struct Cube {
  Mat4f32 transform;
  Vec4f32 color;
  
  f32 border_thickness;
  Vec4f32 border_color;
  
  b32 is_selected;
  b32 is_dead;
} Cube;

internal Cube    cube_new(Vec3f32 position, Vec4f32 color, f32 border_thickness);
internal Quad    cube_get_local_space_face_quad(Cube_Face face);
internal Vec3f32 cube_get_center(Cube cube);

typedef struct Arrow {
  Vec3f32 base;
  Vec3f32 points_to;
  Vec4f32 color;
  f32 scale;
} Arrow;

internal Arrow arrow_new(Vec3f32 base, Vec3f32 points_to, Vec4f32 color, f32 scale);

typedef enum Axis {
  Axis_None,
  Axis_X,
  Axis_Y,
  Axis_Z
} Axis;

typedef struct GizmoTranslation {
  Vec3f32 position;
  
  Vec4f32 x_arrow_color;
  Vec4f32 y_arrow_color;
  Vec4f32 z_arrow_color;
  
  Vec4f32 xy_panel_color;
  Vec4f32 yz_panel_color;
  Vec4f32 zx_panel_color;
  
  f32 arrow_size;
  f32 arrow_scale;
  f32 drag_panel_size;
  f32 drag_panel_scale;
} GizmoTranslation; 

internal GizmoTranslation gizmo_translation_new(Vec3f32 position);
internal f32 find_gizmo_arrow_under_cursor(Camera camera, Vec3f32 raycast, Axis* axis);
internal f32 find_gizmo_quad_under_cursor(Camera camera, Vec3f32 raycast, Axis* axis);

//~ Game generic 
typedef struct Cube_Under_Cursor {
  Cube_Face hovered_face;
  u32 index;
  f32 distance_to_camera;
} Cube_Under_Cursor;

internal b32 find_cube_under_cursor(Camera camera, Vec3f32 raycast, Cube_Under_Cursor* result);

//~ Game State
typedef struct Game_State {
  Arena* arena;
  
  Cube* cubes;
  u32 total_cubes;
  u32 max_cubes;
  
  // These are to replace cubes in the arena, to remove fragmentation.
  u32* empty_cube_slots;
  u32 total_empty_cube_slots;
  
  // These are to know which cubes are selected.
  u32* selected_cubes;
  u32  total_selected_cubes;
  
  struct {
    GizmoTranslation selected_gizmo;
    u32 total_gizmos;
  } editor;
  
  Cube_Under_Cursor cube_under_cursor;
} Game_State;

global Game_State GameState;

internal void game_init();
internal void game_update(Camera* camera, Vec3f32 raycast);

internal void game_push_cube(Cube cube);
internal void game_remove_cube(u32 index);
internal u32  game_cubes_alive_count();

#endif //GAME_H
