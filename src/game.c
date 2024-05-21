
internal void game_init() {
  AssertNoReentry();
  
  MemoryZeroStruct(&GameState);
  
  GameState.cubes_arena = arena_init();
  GameState.cubes = (Cube*)PushArray(GameState.cubes_arena, Cube, 8196);
  GameState.total_cubes = 0;
  
}

// TODO(fz): Argument add_cube is a bit hacked
// This is a temporary thing until we have a more robust
// input system
internal void game_update(Camera camera, Vec3f32 raycast, b32 add_cube) {
  
  //~ Find cube under cursor
  for(u32 i = 0; i < GameState.total_cubes; i++) {
    if (find_cube_under_cursor(camera, raycast, &GameState.cube_under_cursor)) {
      break;
    }
    if (i == GameState.total_cubes-1) {
      MemoryZeroStruct(&GameState.cube_under_cursor);
      GameState.cube_under_cursor.index = U32_MAX;
    }
  }
  
  // Add cube if add_cube pushed
  if (GameState.cube_under_cursor.index != U32_MAX) {
    Quad face = cube_get_local_space_face_quad(GameState.cube_under_cursor.hovered_face);
    face = transform_quad(face, GameState.cubes[GameState.cube_under_cursor.index].transform);
    
    Vec3f32 center = cube_get_center(GameState.cubes[GameState.cube_under_cursor.index]);
    Vec3f32 direction = sub_vec3f32(quad_get_center(face), center);
    Vec3f32 new_cube_center = add_vec3f32(center, scale_vec3f32(direction, 2.0f));
    
    if (add_cube) {
      game_push_cube(cube_new(new_cube_center, PALLETE_COLOR_B));
    }
  }
  
}

internal void game_push_cube(Cube cube) {
  GameState.cubes[GameState.total_cubes] = cube;
  GameState.total_cubes += 1;
}


internal b32 find_cube_under_cursor(Camera camera, Vec3f32 raycast, Cube_Under_Cursor* result) {
  b32 match = 0;
  for (u32 i = 0; i < GameState.total_cubes; i++) {
    Cube it = GameState.cubes[i];
    for(u32 j = 0; j < 6; j++) {
      Quad face = transform_quad(cube_get_local_space_face_quad(j), it.transform);
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