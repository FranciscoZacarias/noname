
internal void game_init() {
  AssertNoReentry();
  
  MemoryZeroStruct(&GameState);
  
  GameState.arena = arena_init();
  
  GameState.max_cubes = Kilobytes(16);
  GameState.cubes = (Cube*)PushArray(GameState.arena, Cube, GameState.max_cubes);
  GameState.total_cubes = 0;
  GameState.empty_cube_slots = (u32*)PushArray(GameState.arena, u32, GameState.max_cubes);
  GameState.total_empty_cube_slots = 0;;
  
  GameState.selected_cubes =  (u32*)PushArray(GameState.arena, u32, GameState.max_cubes);
  MemorySet(GameState.selected_cubes, U32_MAX, GameState.max_cubes);
  GameState.total_selected_cubes = 0;
  
  MemoryZeroStruct(&GameState.cube_under_cursor);
}

internal void game_update(Camera* camera, Vec3f32 raycast) {
  //~ Axis
  // NOTE(fz): Here I'm calling the renderer directly because these are static arrows that will never change.
  // We either render them or not. So Im not gonna bother putting them in the game state.
  f32 size = 20.0f;
  renderer_push_arrow(&ProgramRenderer, arrow_new(vec3f32(-size,  0.0f,  0.0f), vec3f32(size,  0.0f,  0.0f), Color_Red,   0.1f), 0);
  renderer_push_arrow(&ProgramRenderer, arrow_new(vec3f32( 0.0f, -size,  0.0f), vec3f32( 0.0f, size,  0.0f), Color_Green, 0.1f), 0);
  renderer_push_arrow(&ProgramRenderer, arrow_new(vec3f32( 0.0f,  0.0f, -size), vec3f32( 0.0f,  0.0f, size), Color_Blue,  0.1f), 0);
  
  //- TODO(Fz): This section should be abstracted away from the renderer. Renderer should not know about arrows.
  //- We need to set this in the GameState and let the renderer work with that. This is just here
  //- While we refactor
  {
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
renderer_push_string(&ProgramRenderer, txt, vec2f32(-0.998, y_pos), Color_Yellow, 1); \
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
  
  GameState.cube_under_cursor.index = U32_MAX;
  
  Axis gizmo_arrow_result = Axis_None;
  f32 distance_arrow = find_gizmo_arrow_under_cursor(*camera, raycast, &gizmo_arrow_result);
  Axis gizmo_panel_result = Axis_None;
  f32 distance_panel = find_gizmo_quad_under_cursor(*camera, raycast, &gizmo_panel_result);
  
  if (gizmo_arrow_result == Axis_None &&  gizmo_panel_result == Axis_None && !find_cube_under_cursor(*camera, raycast, &GameState.cube_under_cursor)) {
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
  
  if (GameState.total_selected_cubes == 1) {
    u32 selected_cube_index = GameState.selected_cubes[0];
    Cube* selected_cube = &GameState.cubes[selected_cube_index];
    GameState.editor.selected_gizmo = gizmo_translation_new(cube_get_center(*selected_cube));
    GameState.editor.total_gizmos = 1;
    b32 true_if_arrow_false_if_panel = 0;
    
    f32 delta = 0.0f;
    if (gizmo_arrow_result != Axis_None || gizmo_panel_result != Axis_None) {
      glfwSetCursor(GlfwWindow, DragCursor);
    } else {
      glfwSetCursor(GlfwWindow, ArrowCursor);
    }
    
    if (input_is_button_pressed(MouseButton_Left)) {
      if (input_is_key_down(KeyboardKey_LEFT_SHIFT)) {
        delta -= 2.0f;
      } else {
        delta += 2.0f;
      }
    }
    
    if (gizmo_arrow_result != Axis_None && gizmo_panel_result != Axis_None) {
      if (distance_arrow <= distance_panel) {
        true_if_arrow_false_if_panel = 1;
      } else {
        true_if_arrow_false_if_panel = 0;
      }
    } else if (gizmo_arrow_result != Axis_None) {
      true_if_arrow_false_if_panel = 1;
    } else if (gizmo_panel_result != Axis_None) {
      true_if_arrow_false_if_panel = 0;
    }
    
    if (true_if_arrow_false_if_panel) {
      switch (gizmo_arrow_result) {
        case Axis_X: {
          GameState.editor.selected_gizmo.x_arrow_color = Color_Yellow;
          Mat4f32 translate = translate_mat4f32(delta, 0.0f, 0.0f);
          selected_cube->transform = mul_mat4f32(translate, selected_cube->transform);
        } break;
        case Axis_Y: {
          GameState.editor.selected_gizmo.y_arrow_color = Color_Yellow;
          Mat4f32 translate = translate_mat4f32(0.0f, delta, 0.0f);
          selected_cube->transform = mul_mat4f32(translate, selected_cube->transform);
        } break;
        case Axis_Z: {
          GameState.editor.selected_gizmo.z_arrow_color = Color_Yellow;
          Mat4f32 translate = translate_mat4f32(0.0f, 0.0f, delta);
          selected_cube->transform = mul_mat4f32(translate, selected_cube->transform);
        } break;
      }
    } else if (gizmo_panel_result != Axis_None) {
      switch (gizmo_panel_result) {
        case Axis_X: {
          GameState.editor.selected_gizmo.xy_panel_color.w = 0.7f;
          GameState.editor.selected_gizmo.x_arrow_color = Color_Yellow;
          GameState.editor.selected_gizmo.y_arrow_color = Color_Yellow;
          Mat4f32 translate = translate_mat4f32(delta, delta, 0.0f);
          selected_cube->transform = mul_mat4f32(translate, selected_cube->transform);
        } break;
        case Axis_Y: {
          GameState.editor.selected_gizmo.yz_panel_color.w = 0.7f;
          GameState.editor.selected_gizmo.y_arrow_color = Color_Yellow;
          GameState.editor.selected_gizmo.z_arrow_color = Color_Yellow;
          Mat4f32 translate = translate_mat4f32(0.0f, delta, delta);
          selected_cube->transform = mul_mat4f32(translate, selected_cube->transform);
        } break;
        case Axis_Z: {
          GameState.editor.selected_gizmo.zx_panel_color.w = 0.7f;
          GameState.editor.selected_gizmo.x_arrow_color = Color_Yellow;
          GameState.editor.selected_gizmo.z_arrow_color = Color_Yellow;
          Mat4f32 translate = translate_mat4f32(delta, 0.0f, delta);
          selected_cube->transform = mul_mat4f32(translate, selected_cube->transform);
        } break;
      }
    }
  } else {
    GameState.editor.total_gizmos = 0;
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
  
  if (input_is_key_pressed(KeyboardKey_F1)) {
    for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
      GameState.cubes[GameState.selected_cubes[i]].color = PALLETE_COLOR_A;
    }
  } else if (input_is_key_pressed(KeyboardKey_F2)) {
    for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
      GameState.cubes[GameState.selected_cubes[i]].color = PALLETE_COLOR_B;
    }
  } else if (input_is_key_pressed(KeyboardKey_F3)) {
    for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
      GameState.cubes[GameState.selected_cubes[i]].color = PALLETE_COLOR_C;
    }
  } else if (input_is_key_pressed(KeyboardKey_F4)) {
    for(u32 i = 0; i < GameState.total_selected_cubes; i += 1) {
      GameState.cubes[GameState.selected_cubes[i]].color = PALLETE_COLOR_D;
    }
  }
  
  if (input_is_key_down(KeyboardKey_LEFT_CONTROL)) {
    if (input_is_key_pressed(KeyboardKey_S)) {
      game_save();
    }
  }
}

internal void game_save() {
  // To save:
  // [] Camera 
  // [] Cubes
  
  Arena_Temp scratch = scratch_begin(0, 0);
  
  u32 save_game_size = sizeof(Camera) + (sizeof(Cube)*GameState.total_cubes);
  u32 offset = 0;
  u8* save_game_data = (u8*)PushArray(scratch.arena, u8, save_game_size);
  MemoryCopy(save_game_data, &ProgramState.camera, sizeof(Camera));
  offset += sizeof(Camera);
  for(u32 i = 0; i < GameState.total_cubes; i += 1) {
    MemoryCopy(save_game_data + offset, &GameState.cubes[i], sizeof(Cube));
    offset += sizeof(Cube);
  }
  
  os_file_write(SAVE_FILE_PATH, save_game_data, save_game_size);
  
  scratch_end(&scratch);
  printf("Saved!\n");
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

internal GizmoTranslation gizmo_translation_new(Vec3f32 position) {
  GizmoTranslation result = { 0 };
  
  result.x_arrow_color = Color_Red;
  result.y_arrow_color = Color_Green;
  result.z_arrow_color = Color_Blue;
  
  result.xy_panel_color = vec4f32(1.0f, 1.0f, 0.0f, 0.3);
  result.yz_panel_color = vec4f32(0.0f, 1.0f, 1.0f, 0.3);
  result.zx_panel_color = vec4f32(1.0f, 0.0f, 1.0f, 0.3);
  
  result.position    = position;
  result.arrow_size  = 3.0f;
  result.arrow_scale = 1.0f;
  result.drag_panel_size  = 3.0f;
  result.drag_panel_scale = 0.7f;
  return result;
}

internal Arrow arrow_new(Vec3f32 base, Vec3f32 points_to, Vec4f32 color, f32 scale) {
  Arrow result = { 0 };
  result.base = base;
  result.points_to = points_to;
  result.color = color;
  result.scale = scale;
  return result;
}

internal f32 find_gizmo_arrow_under_cursor(Camera camera, Vec3f32 raycast, Axis* axis) {
  if (GameState.total_selected_cubes != 1) {
    return 0;
  }
  
  Axis result = Axis_None;
  f32 axis_distance = F32_MAX;
  
  //~ Try arrow X
  {
    Cube cube = cube_new(GameState.editor.selected_gizmo.position, vec4f32(0.0f, 0.0f, 0.0f, 0.0f), 0.3f);
    Mat4f32 scale = scale_mat4f32(1.5f, 0.1f, 0.1f);
    Mat4f32 translate = translate_mat4f32(1.5f, 0.0f, 0.0f);
    cube.transform = mul_mat4f32(translate, cube.transform);
    cube.transform = mul_mat4f32(scale, cube.transform);
    for(u32 j = 0; j < 6; j++) {
      Quad face = transform_quad(cube_get_local_space_face_quad(j), cube.transform);
      Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, raycast), face.p0, face.p1, face.p2);
      if (is_vector_inside_rectangle(intersection, face.p0, face.p1, face.p2)) {
        // NOTE(fz): Because it's an arrow, we just care about matching once.
        result = Axis_X;
        axis_distance = distance_vec3f32(intersection, camera.position);
      }
    }
  }
  
  //~ Try arrow Y
  {
    Cube cube = cube_new(GameState.editor.selected_gizmo.position, vec4f32(0.0f, 0.0f, 0.0f, 0.0f), 0.3f);
    Mat4f32 scale = scale_mat4f32(0.1f, 1.5f, 0.1f);
    Mat4f32 translate = translate_mat4f32(0.0f, 1.5f, 0.0f);
    cube.transform = mul_mat4f32(translate, cube.transform);
    cube.transform = mul_mat4f32(scale, cube.transform);
    for(u32 j = 0; j < 6; j++) {
      Quad face = transform_quad(cube_get_local_space_face_quad(j), cube.transform);
      Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, raycast), face.p0, face.p1, face.p2);
      if (is_vector_inside_rectangle(intersection, face.p0, face.p1, face.p2)) {
        // NOTE(fz): Because it's an arrow, we just care about matching once.
        f32 distance = distance_vec3f32(intersection, camera.position);
        if (axis_distance >= distance) {
          result = Axis_Y;
          axis_distance = distance;
        }
      }
    }
  }
  
  //~ Try arrow Z
  {
    Cube cube = cube_new(GameState.editor.selected_gizmo.position, vec4f32(0.0f, 0.0f, 0.0f, 0.0f), 0.3f);
    Mat4f32 scale = scale_mat4f32(0.1f, 0.1f, 1.5f);
    Mat4f32 translate = translate_mat4f32(0.0f, 0.0f, 1.5f);
    cube.transform = mul_mat4f32(translate, cube.transform);
    cube.transform = mul_mat4f32(scale, cube.transform);
    for(u32 j = 0; j < 6; j++) {
      Quad face = transform_quad(cube_get_local_space_face_quad(j), cube.transform);
      Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, raycast), face.p0, face.p1, face.p2);
      if (is_vector_inside_rectangle(intersection, face.p0, face.p1, face.p2)) {
        // NOTE(fz): Because it's an arrow, we just care about matching once.
        f32 distance = distance_vec3f32(intersection, camera.position);
        if (axis_distance >= distance) {
          result = Axis_Z;
          axis_distance = distance;
        }
      }
    }
  }
  
  if (result != Axis_None) {
    *axis = result;
    return axis_distance;
  }
  
  return 0;
}

internal f32 find_gizmo_quad_under_cursor(Camera camera, Vec3f32 raycast, Axis* axis) {
  // NOTE(fz): For the resutl axis, let's say X is XY plane, Y is YZ plane and Z is ZX plane.
  if (GameState.total_selected_cubes != 1) {
    return 0;
  }
  
  Axis result = Axis_None;
  f32 axis_distance = F32_MAX;
  
  Vec3f32 x = add_vec3f32(GameState.editor.selected_gizmo.position, vec3f32(GameState.editor.selected_gizmo.drag_panel_size, 0.0f, 0.0f));
  Vec3f32 y = add_vec3f32(GameState.editor.selected_gizmo.position, vec3f32(0.0f, GameState.editor.selected_gizmo.drag_panel_size, 0.0f));
  Vec3f32 z = add_vec3f32(GameState.editor.selected_gizmo.position, vec3f32(0.0f, 0.0f, GameState.editor.selected_gizmo.drag_panel_size));
  
  //~ Try panel XY
  {
    Quad xy = {
      add_vec3f32(GameState.editor.selected_gizmo.position,                      vec3f32( GameState.editor.selected_gizmo.drag_panel_scale, GameState.editor.selected_gizmo.drag_panel_scale, 0.0f)),
      add_vec3f32(x,                                   vec3f32(-GameState.editor.selected_gizmo.drag_panel_scale, GameState.editor.selected_gizmo.drag_panel_scale, 0.0f)),
      add_vec3f32(vec3f32(x.x, y.y, GameState.editor.selected_gizmo.position.z), vec3f32(-GameState.editor.selected_gizmo.drag_panel_scale, -GameState.editor.selected_gizmo.drag_panel_scale, 0.0f)),
      add_vec3f32(y,                                   vec3f32( GameState.editor.selected_gizmo.drag_panel_scale, -GameState.editor.selected_gizmo.drag_panel_scale, 0.0f)),
    };
    Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, raycast), xy.p0, xy.p1, xy.p2);
    if (is_vector_inside_rectangle(intersection, xy.p0, xy.p1, xy.p2)) {
      result = Axis_X;
      axis_distance = distance_vec3f32(intersection, camera.position);
    }
  }
  
  //~ Try panel YZ
  {
    Quad yz = {
      add_vec3f32(GameState.editor.selected_gizmo.position,                      vec3f32( 0.0f, GameState.editor.selected_gizmo.drag_panel_scale, GameState.editor.selected_gizmo.drag_panel_scale)),
      add_vec3f32(z,                                   vec3f32( 0.0f, GameState.editor.selected_gizmo.drag_panel_scale, -GameState.editor.selected_gizmo.drag_panel_scale)),
      add_vec3f32(vec3f32(GameState.editor.selected_gizmo.position.x, y.y, z.z), vec3f32( 0.0f, -GameState.editor.selected_gizmo.drag_panel_scale, -GameState.editor.selected_gizmo.drag_panel_scale)),
      add_vec3f32(y,                                   vec3f32( 0.0f, -GameState.editor.selected_gizmo.drag_panel_scale, GameState.editor.selected_gizmo.drag_panel_scale)),
    };
    Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, raycast), yz.p0, yz.p1, yz.p2);
    if (is_vector_inside_rectangle(intersection, yz.p0, yz.p1, yz.p2)) {
      f32 distance = distance_vec3f32(intersection, camera.position);
      if (axis_distance >= distance) {
        result = Axis_Y;
        axis_distance = distance_vec3f32(intersection, camera.position);
      }
    }
  }
  
  //~ Try panel ZX
  {
    Quad zx = {
      add_vec3f32(GameState.editor.selected_gizmo.position,                      vec3f32( GameState.editor.selected_gizmo.drag_panel_scale, 0.0f,  GameState.editor.selected_gizmo.drag_panel_scale)),
      add_vec3f32(x,                                   vec3f32(-GameState.editor.selected_gizmo.drag_panel_scale, 0.0f,  GameState.editor.selected_gizmo.drag_panel_scale)),
      add_vec3f32(vec3f32(x.x, GameState.editor.selected_gizmo.position.y, z.z), vec3f32(-GameState.editor.selected_gizmo.drag_panel_scale, 0.0f, -GameState.editor.selected_gizmo.drag_panel_scale)),
      add_vec3f32(z,                                   vec3f32( GameState.editor.selected_gizmo.drag_panel_scale, 0.0f, -GameState.editor.selected_gizmo.drag_panel_scale)),
    };
    
    Vec3f32 intersection = intersect_line_with_plane(linef32(camera.position, raycast), zx.p0, zx.p1, zx.p2);
    if (is_vector_inside_rectangle(intersection, zx.p0, zx.p1, zx.p2)) {
      f32 distance = distance_vec3f32(intersection, camera.position);
      if (axis_distance >= distance) {
        result = Axis_Z;
        axis_distance = distance_vec3f32(intersection, camera.position);
      }
    }
  }
  
  if (result != Axis_None) {
    *axis = result;
    return axis_distance;
  }
  
  return 0;
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