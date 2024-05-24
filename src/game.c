
internal void game_init() {
  AssertNoReentry();
  
  MemoryZeroStruct(&GameState);
  
  GameState.arena = arena_init();
  
  GameState.max_cubes = Kilobytes(16);
  GameState.cubes = (Cube*)PushArray(GameState.arena, Cube, GameState.max_cubes);
  GameState.total_cubes = 0;
  
  GameState.selected_cubes =  (u32*)PushArray(GameState.arena, u32, GameState.max_cubes);
  MemorySet(GameState.selected_cubes, U32_MAX, GameState.max_cubes);
  GameState.total_selected_cubes = 0;
  
  GameState.empty_cube_slots = (u32*)PushArray(GameState.arena, u32, GameState.max_cubes);
  GameState.total_empty_cube_slots = 0;;
  
  game_push_cube(cube_new(vec3f32( 0.0f,  0.0f,  0.0f), PALLETE_COLOR_A, 0.05f));
  game_push_cube(cube_new(vec3f32( 2.0f,  0.0f, -8.0f), PALLETE_COLOR_B, 0.05f));
  game_push_cube(cube_new(vec3f32( 4.0f,  2.0f, -8.0f), PALLETE_COLOR_B, 0.05f));
  game_push_cube(cube_new(vec3f32( 6.0f,  0.0f, -8.0f), PALLETE_COLOR_B, 0.05f));
  game_push_cube(cube_new(vec3f32( 0.0f,  0.0f, -8.0f), PALLETE_COLOR_B, 0.05f));
  game_push_cube(cube_new(vec3f32( 0.0f, -0.0f,  8.0f), PALLETE_COLOR_C, 0.05f));
  game_push_cube(cube_new(vec3f32( 0.0f,  8.0f,  0.0f), PALLETE_COLOR_C, 0.05f));
  game_push_cube(cube_new(vec3f32( 0.0f, -8.0f,  0.0f), PALLETE_COLOR_A, 0.05f));
  game_push_cube(cube_new(vec3f32( 8.0f,  0.0f,  0.0f), PALLETE_COLOR_B, 0.05f));
  game_push_cube(cube_new(vec3f32(-8.0f,  0.0f,  0.0f), PALLETE_COLOR_C, 0.05f));
  game_push_cube(cube_new(vec3f32( 8.0f,  8.0f,  8.0f), PALLETE_COLOR_C, 0.05f));
  game_push_cube(cube_new(vec3f32(-8.0f, -8.0f, -8.0f), PALLETE_COLOR_A, 0.05f));
  game_push_cube(cube_new(vec3f32( 8.0f, -8.0f, -8.0f), PALLETE_COLOR_B, 0.05f));
}

internal void game_update(Camera* camera, Vec3f32 raycast) {
  
  //~ Find cube under cursor
  if (!find_cube_under_cursor(*camera, raycast, &GameState.cube_under_cursor)) {
    GameState.cube_under_cursor.index = U32_MAX;
  }
  
  if (GameState.cube_under_cursor.index != U32_MAX) {
    if (input_is_key_pressed(KeyboardKey_F)) {
      //~ NOTE(fz): Add a cube
      Quad face = cube_get_local_space_face_quad(GameState.cube_under_cursor.hovered_face);
      face      = transform_quad(face, GameState.cubes[GameState.cube_under_cursor.index].transform);
      Vec3f32 center = cube_get_center(GameState.cubes[GameState.cube_under_cursor.index]);
      Vec3f32 direction = sub_vec3f32(quad_get_center(face), center);
      Vec3f32 new_cube_center = add_vec3f32(center, scale_vec3f32(direction, 2.0f));
      
      game_push_cube(cube_new(new_cube_center, PALLETE_COLOR_B, 0.05f));
    } else if (input_is_key_pressed(KeyboardKey_G)) {
      //~ NOTE(fz): Delete a cube
      
      for(u32 i = 0; i < GameState.total_selected_cubes ; i += 1) {
        if (i == GameState.selected_cubes[i]) {
          if (GameState.total_selected_cubes > 1) {
            GameState.selected_cubes[i] = GameState.selected_cubes[GameState.total_selected_cubes-1];
            GameState.selected_cubes[GameState.total_selected_cubes-1] = U32_MAX;
            GameState.total_selected_cubes -= 1;
          } else {
            GameState.selected_cubes[0] = U32_MAX;
            GameState.total_selected_cubes = 0;
          }
          break;
        }
      }
      
      game_remove_cube(GameState.cube_under_cursor.index);
    }
    
    if (input_is_button_pressed(MouseButton_Left)) {
      b32 is_already_selected = 0;
      for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
        if (GameState.cube_under_cursor.index == GameState.selected_cubes[i]) {
          is_already_selected = 1;
        }
      }
      
      if (!is_already_selected) {
        if (input_is_key_down(KeyboardKey_LEFT_CONTROL)) {
          GameState.cubes[GameState.cube_under_cursor.index].is_selected = 1;
          GameState.selected_cubes[GameState.total_selected_cubes] = GameState.cube_under_cursor.index;
          GameState.total_selected_cubes += 1;
        } else {
          for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
            GameState.cubes[GameState.selected_cubes[i]].is_selected = 0;
            GameState.selected_cubes[i] = U32_MAX;
          }
          GameState.cubes[GameState.cube_under_cursor.index].is_selected = 1;
          GameState.selected_cubes[0] = GameState.cube_under_cursor.index;
          GameState.total_selected_cubes = 1;
        }
      }
    }
  }
  
  
  // NOTE(fz): Unselect all cubes
  if (input_is_key_pressed(KeyboardKey_TAB)) {
    for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
      GameState.cubes[GameState.selected_cubes[i]].is_selected = 0;
      GameState.selected_cubes[i] = U32_MAX;
    }
    GameState.total_selected_cubes = 0;
  }
  
  // NOTE(fz): Delete selected cubes
  if (input_is_key_pressed(KeyboardKey_DELETE)) {
    for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
      game_remove_cube(GameState.selected_cubes[i]);
      GameState.selected_cubes[i] = U32_MAX;
    }
    GameState.total_selected_cubes = 0;
  }
}

internal u32 game_cubes_alive_count() {
  u32 result = GameState.total_cubes - GameState.total_empty_cube_slots;
  return result;
}

internal void game_push_cube(Cube cube) {
  if (GameState.total_empty_cube_slots > 0) {
    GameState.total_empty_cube_slots -= 1;
    GameState.cubes[GameState.empty_cube_slots[GameState.total_empty_cube_slots]] = cube;
  } else {
    GameState.cubes[GameState.total_cubes] = cube;
    GameState.total_cubes += 1;
  } 
}

internal void game_remove_cube(u32 index) {
  GameState.cubes[index].is_dead = 1;
  GameState.empty_cube_slots[GameState.total_empty_cube_slots] = index;
  GameState.total_empty_cube_slots += 1;
}

internal b32 find_cube_under_cursor(Camera camera, Vec3f32 raycast, Cube_Under_Cursor* result) {
  b32 match = 0;
  for (u32 i = 0; i < GameState.total_cubes; i++) {
    Cube cube = GameState.cubes[i];
    if (cube.is_dead) {
      continue;
    }
    
    for(u32 j = 0; j < 6; j++) {
      Quad face = transform_quad(cube_get_local_space_face_quad(j), cube.transform);
      Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, raycast), face.p0, face.p1, face.p2);
      if (is_vector_inside_rectangle(intersection, face.p0, face.p1, face.p2)) {
        if (!match) {
          result->hovered_face = j;
          result->index = i;
          result->distance_to_camera = distance_vec3f32(intersection, camera.position);
          match = 1;
        } else {
          f32 distance = distance_vec3f32(intersection, camera.position);
          if (distance < result->distance_to_camera) {
            result->hovered_face = j;
            result->index = i;
            result->distance_to_camera = distance;
          }
        }
      }
    }
  }
  
  return match;
}