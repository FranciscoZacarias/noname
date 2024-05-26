
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
  
  //- TODO(Fz): This section should be abstracted away from the renderer. Renderer should not know about arrows.
  //- We need to set this in the GameState and let the renderer work with that. This is just here
  //- While we refactor
  {
    //~ Axis
    f32 size = 20.0f;
    renderer_push_arrow(&ProgramRenderer, vec3f32(-size,   0.0f,   0.0f), vec3f32(size,  0.0f,  0.0f), Color_Red, 0.1f, 0);
    renderer_push_arrow(&ProgramRenderer, vec3f32(  0.0f, -size,   0.0f), vec3f32( 0.0f, size,  0.0f), Color_Green, 0.1f, 0);
    renderer_push_arrow(&ProgramRenderer, vec3f32(  0.0f,   0.0f, -size), vec3f32( 0.0f,  0.0f, size), Color_Blue, 0.1f, 0);
    
    for(u32 i = 0; i < GameState.total_cubes; i += 1) {
      Cube cube = GameState.cubes[i];
      
      if (cube.is_dead) {
        continue;
      }
      
      if (cube.is_selected) {
        cube.border_thickness = 0.08;
        cube.border_color = vec4f32(0.5+0.5*sin(5*ProgramState.current_time), 0.5+0.5*sin(5*ProgramState.current_time), 0.0f);
        
        if (GameState.total_selected_cubes == 1) {
          // TODO(fz): gizmo should be a user level thing. renderer should not know what a gizmo is
          renderer_push_translation_gizmo(&ProgramRenderer, cube_get_center(cube), 1);
        }
      }
      
      if (GameState.cube_under_cursor.index == i) {
        f32 highlight_scale = 0.8f;
        renderer_push_cube_highlight_face(&ProgramRenderer, cube, GameState.cube_under_cursor.hovered_face, vec4f32(cube.color.x * highlight_scale, cube.color.y * highlight_scale, cube.color.z * highlight_scale), 0);
      } else {
        renderer_push_cube(&ProgramRenderer, cube, 0);
      }
    }
    
    if (ProgramRenderer.program_state->show_debug_stats) {
      String txt;
      s32 len;
      f32 y_pos = 0.95f;
      
      // TODO(Fz): We cant have shit like this
      // NOTE(fz): This is a hack to have less friction just writing strings stats while we dont have better things to work with
#define AddStat(fmt, tag, ...) do {\
char tag##_buffer[160] = {0}; \
len = stbsp_sprintf(tag##_buffer, fmt, __VA_ARGS__); \
txt.size = (u64)len; \
txt.str  = (u8*)tag##_buffer; \
renderer_push_string(&ProgramRenderer, txt, vec2f32(-0.998, y_pos), Color_Yellow); \
y_pos -= 0.05f; } while(0); 
      
      local_persist f64 fps_last_time = 0.0f;
      local_persist s64 frame_count   = 0.0f;
      local_persist u64 dt_fps        = 0.0f;
      
      frame_count += 1;
      if (ProgramRenderer.program_state->current_time - fps_last_time >= 0.1f) {
        dt_fps = frame_count / (ProgramRenderer.program_state->current_time - fps_last_time);
        frame_count  = 0;
        fps_last_time = ProgramRenderer.program_state->current_time;
      }
      
      AddStat("FPS: %d", fps, dt_fps);
      AddStat("Ms/Frame: %0.2f", msframe, (f32)ProgramRenderer.program_state->delta_time*1000);
      AddStat("Triangles: %d/%d", trigs, ProgramRenderer.triangles_count, ProgramRenderer.triangles_max);
      AddStat("Triangles Front: %d/%d", trigsfront, ProgramRenderer.triangles_front_count, ProgramRenderer.triangles_front_max);
      AddStat("Cube Count: %d", cubs, game_cubes_alive_count());
      AddStat("Hovered Cube Index: %d", hovered, (GameState.cube_under_cursor.index == U32_MAX) ? -1 : GameState.cube_under_cursor.index);
      AddStat("Total empty slots: %u", emptyslots, GameState.total_empty_cube_slots);
      AddStat("Selected Cubes Count: %u", selectcubes, GameState.total_selected_cubes)
    }
  }
  //- End TODO(fz)
  
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


internal Cube cube_new(Vec3f32 position, Vec4f32 color, f32 border_thickness) {
  Cube result = { 0 };
  result.transform        = translate_mat4f32(position.x, position.y, position.z);;
  result.color            = color;
  result.border_thickness = border_thickness;
  result.border_color     = Color_Black;
  result.is_selected      = 0;
  result.is_dead          = 0;
  return result;
}

internal Quad cube_get_local_space_face_quad(Cube_Face face) {
	Quad result = { 0 };
  
	switch(face) {
		case CubeFace_Back: {
			result = (Quad){ CubeVerticesLocalSpace.p0, CubeVerticesLocalSpace.p1, CubeVerticesLocalSpace.p2, CubeVerticesLocalSpace.p3 };
		} break;
		case CubeFace_Front: {
			result = (Quad){ CubeVerticesLocalSpace.p6, CubeVerticesLocalSpace.p5, CubeVerticesLocalSpace.p4, CubeVerticesLocalSpace.p7 };;
		} break;
		case CubeFace_Left: {
			result = (Quad){ CubeVerticesLocalSpace.p0, CubeVerticesLocalSpace.p3, CubeVerticesLocalSpace.p7, CubeVerticesLocalSpace.p4 };
		} break;
		case CubeFace_Right: {
			result = (Quad){ CubeVerticesLocalSpace.p5, CubeVerticesLocalSpace.p6, CubeVerticesLocalSpace.p2, CubeVerticesLocalSpace.p1 };
		} break;
		case CubeFace_Bottom: {
			result = (Quad){ CubeVerticesLocalSpace.p4, CubeVerticesLocalSpace.p5, CubeVerticesLocalSpace.p1, CubeVerticesLocalSpace.p0 };
		} break;
		case CubeFace_Top: {
			result = (Quad){ CubeVerticesLocalSpace.p3, CubeVerticesLocalSpace.p2, CubeVerticesLocalSpace.p6, CubeVerticesLocalSpace.p7 };
		} break;
	}
  
	return result;
}

internal Vec3f32 cube_get_center(Cube cube) {
  Vec3f32 result = {
    cube.transform.m12,
    cube.transform.m13,
    cube.transform.m14
  };
  return result;
}