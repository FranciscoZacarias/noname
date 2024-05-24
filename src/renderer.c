
internal Renderer renderer_init(Program_State* program_state) {
	AssertNoReentry();
  
  Renderer result;
  MemoryZeroStruct(&result);
  
  result.program_state = program_state;
  result.arena  = arena_init();
  
  result.triangles_max   = Kilobytes(16);
  result.triangles_data  = (Renderer_Vertex*)PushArray(result.arena, Renderer_Vertex, result.triangles_max*3);
  result.triangles_count = 0;
  
  result.textures_max   = 8;
  result.textures       = (u32*)PushArray(result.arena, u32, result.textures_max);
  result.textures_count = 0;
  
  Arena_Temp scratch = scratch_begin(0, 0);
  
  // --- Compile shader
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(DEFAULT_VERTEX_SHADER));
    glShaderSource(vertex_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(vertex_shader);
    {
      s32 success;
      glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
        printf("Error %d compiling default vertex shader. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    OS_File fragment_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(DEFAULT_FRAGMENT_SHADER));
    glShaderSource(fragment_shader, 1, &fragment_shader_source.data, &(GLint)fragment_shader_source.size);
    glCompileShader(fragment_shader);
    {
      s32 success;
      glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
        printf("Error %d compiling default fragment shader. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  result.shader_program = glCreateProgram();
  {
    glAttachShader(result.shader_program, vertex_shader);
    glAttachShader(result.shader_program, fragment_shader);
    glLinkProgram(result.shader_program);
    {
      s32 success;
      glGetProgramiv(result.shader_program, GL_LINK_STATUS, &success);
      if(!success) {
        char infoLog[1024];
        glGetProgramInfoLog(result.shader_program, 1024, NULL, infoLog);
        printf("Error %d linking shader program. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  glDetachShader(result.shader_program, vertex_shader);
  glDetachShader(result.shader_program, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  // --- VAO, VBO
  glGenVertexArrays(1, &result.vao);
  glBindVertexArray(result.vao);
  
  // Triangles vbo
  {
    glGenBuffers(1, &result.triangle_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, result.triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, result.triangles_max * 3 * sizeof(Renderer_Vertex), NULL, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, sizeof(Renderer_Vertex), (void*) OffsetOfMember(Renderer_Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_False, sizeof(Renderer_Vertex), (void*) OffsetOfMember(Renderer_Vertex, color));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_False, sizeof(Renderer_Vertex), (void*) OffsetOfMember(Renderer_Vertex, uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_False, sizeof(Renderer_Vertex), (void*) OffsetOfMember(Renderer_Vertex, texture_index));
    glEnableVertexAttribArray(3);
    
    glVertexAttribPointer(4, 1, GL_INT, GL_False, sizeof(Renderer_Vertex), (void*) OffsetOfMember(Renderer_Vertex, has_texture));
    glEnableVertexAttribArray(4);
  }
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  // --- MSAA: FBO and RBO setup
  glGenFramebuffers(1, &result.msaa_frame_buffer_object);
  
  glGenTextures(1, &result.msaa_texture_color_buffer_multisampled);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, result.msaa_texture_color_buffer_multisampled);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, program_state->window_width, program_state->window_height, GL_True);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  
  glGenRenderbuffers(1, &result.msaa_render_buffer_object);
  glBindRenderbuffer(GL_RENDERBUFFER, result.msaa_render_buffer_object);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, program_state->window_width, program_state->window_height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, result.msaa_frame_buffer_object);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, result.msaa_texture_color_buffer_multisampled, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, result.msaa_render_buffer_object);
  
  u32 msaa_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (msaa_fbo_status != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", msaa_fbo_status);
    Assert(0);
  }
  
  // --- Intermidiate fbo for post processing
  glGenFramebuffers(1, &result.postprocessing_fbo);
  
  // create color attachment texture
  glGenTextures(1, &result.screen_texture);
  glBindTexture(GL_TEXTURE_2D, result.screen_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, program_state->window_width, program_state->window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // attach to intermidiate_fbo
  glBindFramebuffer(GL_FRAMEBUFFER, result.postprocessing_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result.screen_texture, 0);
  
  u32 postprocessing_fbo = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (postprocessing_fbo != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", postprocessing_fbo);
    Assert(0);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  // --- Screen shader
  u32 screen_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(SCREEN_VERTEX_SHADER));
    glShaderSource(screen_vertex_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(screen_vertex_shader);
    {
      s32 success;
      glGetShaderiv(screen_vertex_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(screen_vertex_shader, 1024, NULL, infoLog);
        printf("Error %d compiling screen vertex shader. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  u32 screen_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(SCREEN_FRAGMENT_SHADER));
    glShaderSource(screen_fragment_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(screen_fragment_shader);
    {
      s32 success;
      glGetShaderiv(screen_fragment_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(screen_fragment_shader, 1024, NULL, infoLog);
        printf("Error %d compiling screen fragment shader. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  result.screen_program = glCreateProgram();
  {
    glAttachShader(result.screen_program, screen_vertex_shader);
    glAttachShader(result.screen_program, screen_fragment_shader);
    glLinkProgram(result.screen_program);
    {
      s32 success;
      glGetProgramiv(result.screen_program, GL_LINK_STATUS, &success);
      if(!success) {
        char infoLog[1024];
        glGetProgramInfoLog(result.screen_program, 1024, NULL, infoLog);
        printf("Error %d linking shader program. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  f32 quad_vertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f, -1.0f,
    1.0f,  1.0f
  };
  
  glGenVertexArrays(1, &result.screen_vao);
  glGenBuffers(1, &result.screen_vbo);
  
  glBindVertexArray(result.screen_vao);
  glBindBuffer(GL_ARRAY_BUFFER, result.screen_vbo);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_False, 2 * sizeof(f32), (void*)0);
  glEnableVertexAttribArray(0);
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  u32 texture_location = glGetUniformLocation(result.shader_program, "u_texture");
  for (s32 i = 0; i < result.textures_max; i += 1) {
    result.textures[i]= i;
  }
  glUniform1iv(texture_location, result.textures_max, result.textures);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  MemoryZeroStruct(&result.font_info);
  if (!renderer_font_load(&result.font_info, StringLiteral("D:\\work\\noname\\res\\Karmina.Otf"), 16)) {
    printf("Unable to load font!\n");
    Assert(0);
  }
  
  scratch_end(&scratch);
  
  return result;
}

internal void renderer_update(Game_State game_state, Renderer* renderer, Mat4f32 view, Mat4f32 projection) {
  //~ Perspective
  renderer_set_uniform_mat4fv(renderer->shader_program, "model", mat4f32(1.0f));
  renderer_set_uniform_mat4fv(renderer->shader_program, "view", view);
  renderer_set_uniform_mat4fv(renderer->shader_program, "projection", projection);
  
  //~ Axis
  f32 size = 20.0f;
  renderer_push_arrow(renderer, vec3f32(-size,   0.0f,   0.0f), vec3f32(size,  0.0f,  0.0f), Color_Red, 0.5f);
  renderer_push_arrow(renderer, vec3f32(  0.0f, -size,   0.0f), vec3f32( 0.0f, size,  0.0f), Color_Green, 0.5f);
  renderer_push_arrow(renderer, vec3f32(  0.0f,   0.0f, -size), vec3f32( 0.0f,  0.0f, size), Color_Blue, 0.5f);
  
  //~ Program State
  for(u32 i = 0; i < game_state.total_cubes; i += 1) {
    Cube cube = game_state.cubes[i];
    
    if (cube.is_dead) {
      continue;
    }
    
    if (cube.is_selected) {
      cube.border_thickness = 0.08;
      cube.border_color = vec4f32(0.5+0.5*sin(5*ProgramState.current_time), 0.5+0.5*sin(5*ProgramState.current_time), 0.0f);
    }
    
    if (game_state.cube_under_cursor.index == i) {
      f32 highlight_scale = 0.8f;
      renderer_push_cube_highlight_face(renderer, cube, game_state.cube_under_cursor.hovered_face, vec4f32(cube.color.x * highlight_scale, cube.color.y * highlight_scale, cube.color.z * highlight_scale));
    } else {
      renderer_push_cube(renderer, cube);
    }
  }
  
  //~ Text
  if (renderer->program_state->show_debug_stats) {
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
renderer_push_string(renderer, txt, vec2f32(-0.998, y_pos), Color_Yellow); \
y_pos -= 0.05f; } while(0); 
    
    local_persist f64 fps_last_time = 0.0f;
    local_persist s64 frame_count   = 0.0f;
    local_persist u64 dt_fps        = 0.0f;
    
    frame_count += 1;
    if (renderer->program_state->current_time - fps_last_time >= 0.1f) {
      dt_fps = frame_count / (renderer->program_state->current_time - fps_last_time);
      frame_count  = 0;
      fps_last_time = renderer->program_state->current_time;
    }
    
    AddStat("FPS: %d", fps, dt_fps);
    AddStat("Ms/Frame: %0.2f", msframe, (f32)renderer->program_state->delta_time*1000);
    AddStat("Triangles Count/Max: %d/%d", trigs, renderer->triangles_count, renderer->triangles_max);
    AddStat("Cube Count: %d", cubs, game_cubes_alive_count());
    AddStat("Hovered Cube Index: %d", hovered, (game_state.cube_under_cursor.index == U32_MAX) ? -1 : game_state.cube_under_cursor.index);
    AddStat("Total empty slots: %u", emptyslots, game_state.total_empty_cube_slots);
    AddStat("Selected Cubes Count: %u", selectcubes, game_state.total_selected_cubes)
  }
  
  renderer_end_frame(renderer);
}

internal void renderer_generate_msaa_and_intermidiate_buffers(Renderer* renderer) {
  // Delete and recreate MSAA framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->msaa_frame_buffer_object);
  glDeleteTextures(1, &renderer->msaa_texture_color_buffer_multisampled);
  glDeleteRenderbuffers(1, &renderer->msaa_render_buffer_object);
  
  // Regen MSAA buffer
  glGenFramebuffers(1, &renderer->msaa_frame_buffer_object);
  
  glGenTextures(1, &renderer->msaa_texture_color_buffer_multisampled);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderer->msaa_texture_color_buffer_multisampled);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, renderer->program_state->window_width, renderer->program_state->window_height, GL_True);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  
  glGenRenderbuffers(1, &renderer->msaa_render_buffer_object);
  glBindRenderbuffer(GL_RENDERBUFFER, renderer->msaa_render_buffer_object);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, renderer->program_state->window_width, renderer->program_state->window_height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->msaa_frame_buffer_object);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, renderer->msaa_texture_color_buffer_multisampled, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderer->msaa_render_buffer_object);
  
  u32 msaa_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (msaa_fbo_status != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", msaa_fbo_status);
    Assert(0);
  }
  
  // --- Intermidiate fbo for post processing
  glGenFramebuffers(1, &renderer->postprocessing_fbo);
  glDeleteTextures(1, &renderer->screen_texture);
  
  // create color attachment texture
  glGenTextures(1, &renderer->screen_texture);
  glBindTexture(GL_TEXTURE_2D, renderer->screen_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer->program_state->window_width, renderer->program_state->window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // attach to intermidiate_fbo
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->postprocessing_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->screen_texture, 0);
  
  u32 postprocessing_fbo = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (postprocessing_fbo != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", postprocessing_fbo);
    Assert(0);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

internal void renderer_recompile_default_shader(Arena* arena, Renderer* renderer) {
  Arena_Temp scratch = scratch_begin(0, 0);
  
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(DEFAULT_VERTEX_SHADER));
    glShaderSource(vertex_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(vertex_shader);
    {
      s32 success;
      glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
        printf("Error %d while re-compiling default vertex shader. Log: %s", success, infoLog);
        return;
      }
    }
  }
  
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    OS_File fragment_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(DEFAULT_FRAGMENT_SHADER));
    glShaderSource(fragment_shader, 1, &fragment_shader_source.data, &(GLint)fragment_shader_source.size);
    glCompileShader(fragment_shader);
    {
      s32 success;
      glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
        printf("Error %d while re-compiling default fragment shader. Log: %s", success, infoLog);
        glDeleteShader(vertex_shader);
        return;
      }
    }
  }
  
  u32 shader_program = glCreateProgram();
  {
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    {
      s32 success;
      glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
      if(!success) {
        char infoLog[1024];
        glGetProgramInfoLog(shader_program, 1024, NULL, infoLog);
        printf("Error %d linking re-compiled default shader program. Log: %s", success, infoLog);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return;
      }
    }
  }
  
  // If we got to this point, everything was recompiled and attached successfully.
  // Just replace them in the renderer.
  glDeleteProgram(renderer->shader_program);
  
  renderer->shader_program = shader_program;
  
  glDetachShader(renderer->shader_program, vertex_shader);
  glDetachShader(renderer->shader_program, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  printf("Default shader re-compiled!\n");
  
  scratch_end(&scratch);
}

internal void renderer_recompile_screen_shader(Arena* arena, Renderer* renderer) {
  Arena_Temp scratch = scratch_begin(0, 0);
  
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(SCREEN_VERTEX_SHADER));
    glShaderSource(vertex_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(vertex_shader);
    {
      s32 success;
      glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
        printf("Error %d re-compiling screen vertex shader. Log: %s", success, infoLog);
        return;
      }
    }
  }
  
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(SCREEN_FRAGMENT_SHADER));
    glShaderSource(fragment_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(fragment_shader);
    {
      s32 success;
      glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
        printf("Error %d compiling screen fragment shader. Log: %s", success, infoLog);
        glDeleteShader(vertex_shader);
        return;
      }
    }
  }
  
  u32 screen_program = glCreateProgram();
  {
    glAttachShader(screen_program, vertex_shader);
    glAttachShader(screen_program, fragment_shader);
    glLinkProgram(screen_program);
    {
      s32 success;
      glGetProgramiv(screen_program, GL_LINK_STATUS, &success);
      if(!success) {
        char infoLog[1024];
        glGetProgramInfoLog(screen_program, 1024, NULL, infoLog);
        printf("Error %d linking shader program. Log: %s", success, infoLog);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return;
      }
    }
  }
  
  // If we got to this point, everything was recompiled and attached successfully.
  // Just replace them in the renderer.
  glDeleteProgram(renderer->screen_program);
  
  renderer->screen_program = screen_program;
  
  glDetachShader(renderer->screen_program, vertex_shader);
  glDetachShader(renderer->screen_program, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  printf("Screen shader re-compiled!\n");
  
  scratch_end(&scratch);
}

internal void renderer_free(Renderer* renderer) {
  glDeleteVertexArrays(1, &renderer->vao);
  glDeleteBuffers(1, &renderer->triangle_vbo);
  glDeleteProgram(renderer->shader_program);
  
  glDeleteVertexArrays(1, &renderer->screen_vao);
  glDeleteBuffers(1, &renderer->screen_vbo);
  glDeleteProgram(renderer->screen_program);
}

internal b32 renderer_font_load(Renderer_Font_Info* font_info, String file_path, f32 font_size) {
  Arena_Temp scratch = scratch_begin(0, 0);
  
  OS_File font_file = os_file_load_entire_file(scratch.arena, file_path);
  
  u8 temp_bitmap[512 * 512];
  stbtt_fontinfo finfo;
  stbtt_pack_context packctx;
  if (stbtt_InitFont(&finfo, (const char*)font_file.data, 0) == 0) {
    printf("Error loading font: %s\n", file_path.str);
    Assert(0);
  }
  
  stbtt_PackBegin(&packctx, temp_bitmap, 512, 512, 0, 1, 0);
  stbtt_PackSetOversampling(&packctx, 1, 1);
  stbtt_PackFontRange(&packctx, (const char*)font_file.data, 0, font_size, 32, 95, font_info->cdata);
  stbtt_PackEnd(&packctx);
  
  glGenTextures(1, &font_info->font_texture);
  glBindTexture(GL_TEXTURE_2D, font_info->font_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  GLint swizzles[4] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
  glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
  
  font_info->scale = stbtt_ScaleForPixelHeight(&finfo, font_size);
  stbtt_GetFontVMetrics(&finfo, &font_info->ascent, &font_info->descent, NULL);
  font_info->baseline = (s32) (font_info->ascent * font_info->scale);
  font_info->font_size = font_size;
  
  scratch_end(&scratch);
  
  // TODO(fz): We should actually check if everything went ok instead of just return truel
  return 1;
}

internal u32 renderer_texture_load(String file_path) {
  s32 width, height, channels;
  stbi_set_flip_vertically_on_load(1);
  u8* data = stbi_load((const char*)file_path.str, &width, &height, &channels, 0);
  
  u32 id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  
  if (channels == 3) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  } else  if (channels == 4) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  } else {
    printf("Error :: Unexpected number of channels when loading a texture.\n");
    Assert(0);
  }
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  if (channels == 3) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
  } else if (channels == 4) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
  }
  
  stbi_image_free(data);
  return id;
}

internal void renderer_begin_frame(Renderer* renderer, Vec4f32 background_color) {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->msaa_frame_buffer_object);
  glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  renderer->triangles_count = 0;
  renderer->textures_count  = 0;
  
  glUseProgram(renderer->shader_program);
}

void renderer_end_frame(Renderer* renderer) {
  for (u32 i = 0; i < renderer->textures_count; i += 1) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, renderer->textures[i]);
  }
  
  glBindVertexArray(renderer->vao);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->triangle_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->triangles_count * 3 * sizeof(Renderer_Vertex), renderer->triangles_data);
  
  if (HotloadableEnableCulling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
  }
  if (HotloadableEnableWireframeMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glDrawArrays(GL_TRIANGLES, 0, renderer->triangles_count * 3);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->msaa_frame_buffer_object);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->postprocessing_fbo);	
  glBlitFramebuffer(0, 0, renderer->program_state->window_width, renderer->program_state->window_height, 0, 0, renderer->program_state->window_width, renderer->program_state->window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  
  glUseProgram(renderer->screen_program);
  glBindVertexArray(renderer->screen_vao);glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->screen_texture);
  renderer_set_uniform_s32(renderer->screen_program, "window_width", renderer->program_state->window_width);
  renderer_set_uniform_s32(renderer->screen_program, "window_height", renderer->program_state->window_height);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  
  glUseProgram(0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

internal void renderer_push_triangle(Renderer* renderer, Vec3f32 a_position, Vec4f32 a_color, Vec3f32 b_position, Vec4f32 b_color, Vec3f32 c_position, Vec4f32 c_color) {
  if ((renderer->triangles_count + 1) >= renderer->triangles_max) {
    printf("Error :: Renderer :: Too many triangles!");
    Assert(0);
  }
  
  s64 index = renderer->triangles_count * 3;
  renderer->triangles_data[index+0].position      = a_position;
  renderer->triangles_data[index+0].color         = a_color;
  renderer->triangles_data[index+0].uv            = vec2f32(0.0f, 0.0f);
  renderer->triangles_data[index+0].texture_index = F32_MAX;
  renderer->triangles_data[index+0].has_texture   = 0.0;
  
  renderer->triangles_data[index+1].position      = b_position;
  renderer->triangles_data[index+1].color         = b_color;
  renderer->triangles_data[index+1].uv            = vec2f32(0.0f, 0.0f);
  renderer->triangles_data[index+1].texture_index = F32_MAX;
  renderer->triangles_data[index+1].has_texture   = 0.0;
  
  renderer->triangles_data[index+2].position      = c_position;
  renderer->triangles_data[index+2].color         = c_color;
  renderer->triangles_data[index+2].uv            = vec2f32(0.0f, 0.0f);
  renderer->triangles_data[index+2].texture_index = F32_MAX;
  renderer->triangles_data[index+2].has_texture   = 0.0;
  
  renderer->triangles_count += 1;
}

internal void renderer_push_triangle_texture_color(Renderer* renderer, Vec3f32 a_position, Vec2f32 a_uv, Vec3f32 b_position, Vec2f32 b_uv, Vec3f32 c_position, Vec2f32 c_uv, Vec4f32 color, u32 texture) {
  u64 texture_index = U64_MAX;
  for (u64 i = 0; i < renderer->textures_count; i += 1) {
    if (renderer->textures[i] == texture) {
      texture_index = i;
      break;
    }
  }
  
  // TODO(fz): If we add more textures than MAX_TEXTURES, we still have to handle that.
  // TODO(fz): This should probably be in renderer_load_texture.
  // TODO(fz): Do we need to clean them up? Didn't implement it yet because I think now they are kept during the lifetime of the program
  if (texture_index == U64_MAX && renderer->textures_count < renderer->textures_max) {
    renderer->textures[renderer->textures_count] = texture;
    texture_index = renderer->textures_count;
    renderer->textures_count += 1;
  }
  
  Assert(texture_index >= 0 && texture_index <= renderer->textures_max);
  
  s64 index = renderer->triangles_count * 3;
  renderer->triangles_data[index+0].position      = a_position;
  renderer->triangles_data[index+0].color         = color;
  renderer->triangles_data[index+0].uv            = a_uv;
  renderer->triangles_data[index+0].texture_index = texture_index;
  renderer->triangles_data[index+0].has_texture   = 1.0;
  
  renderer->triangles_data[index+1].position      = b_position;
  renderer->triangles_data[index+1].color         = color;
  renderer->triangles_data[index+1].uv            = b_uv;
  renderer->triangles_data[index+1].texture_index = texture_index;
  renderer->triangles_data[index+1].has_texture   = 1.0;
  
  renderer->triangles_data[index+2].position      = c_position;
  renderer->triangles_data[index+2].color         = color;
  renderer->triangles_data[index+2].uv            = c_uv;
  renderer->triangles_data[index+2].texture_index = texture_index;
  renderer->triangles_data[index+2].has_texture   = 1.0;
  
  renderer->triangles_count += 1;
}

internal void renderer_push_triangle_texture(Renderer* renderer, Vec3f32 a_position, Vec2f32 a_uv, Vec3f32 b_position, Vec2f32 b_uv, Vec3f32 c_position, Vec2f32 c_uv, u32 texture) {
  renderer_push_triangle_texture_color(renderer, a_position, a_uv, b_position, b_uv, c_position, c_uv, Color_White, texture);
}

internal void renderer_push_arrow(Renderer* renderer, Vec3f32 a, Vec3f32 b, Vec4f32 color, f32 scale) {
  scale = scale * 0.1; // Makes this scale factor less sensitive on user level
  Vec3f32 direction = normalize_vec3f32(sub_vec3f32(b, a));
  Vec3f32 up   = vec3f32(0.0f, 1.0f, 0.0f);
  Vec3f32 axis = cross_vec3f32(direction, up);
  f64 angle = acos(dot_vec3f32(direction, up) / (length_vec3f32(direction) * length_vec3f32(up)));
  Mat4f32 r = rotate_axis_mat4f32(axis, -angle);
  Mat4f32 t = translate_mat4f32(b.x, b.y, b.z);
  f32 arrow_height = 0.5;
  
  {
    Vec3f32 arrow_top = vec3f32(0.0f, scale, 0.0f);
    Quad base_a = {
      vec3f32(-scale, -scale,  scale),
      vec3f32( scale, -scale,  scale),
      vec3f32( scale, -scale, -scale),
      vec3f32(-scale, -scale, -scale),
    };
    base_a = transform_quad(base_a, r);
    base_a = transform_quad(base_a, t);
    base_a.p0 = sub_vec3f32(base_a.p0, scale_vec3f32(direction, arrow_height));
    base_a.p1 = sub_vec3f32(base_a.p1, scale_vec3f32(direction, arrow_height));
    base_a.p2 = sub_vec3f32(base_a.p2, scale_vec3f32(direction, arrow_height));
    base_a.p3 = sub_vec3f32(base_a.p3, scale_vec3f32(direction, arrow_height));
    
    arrow_top = mul_vec3f32_mat4f32(arrow_top, r);
    arrow_top = mul_vec3f32_mat4f32(arrow_top, t);
    f32 top_extra = distance_vec3f32(arrow_top, b);
    arrow_top = sub_vec3f32(arrow_top, scale_vec3f32(direction, top_extra));
    
    renderer_push_quad(renderer, base_a, color);
    renderer_push_triangle(renderer, base_a.p0, color, arrow_top, color, base_a.p1, color);
    renderer_push_triangle(renderer, base_a.p1, color, arrow_top, color, base_a.p2, color);
    renderer_push_triangle(renderer, base_a.p2, color, arrow_top, color, base_a.p3, color);
    renderer_push_triangle(renderer, base_a.p3, color, arrow_top, color, base_a.p0, color);
  }
  
  {
    scale = scale*0.5;
    Quad base_a = {
      vec3f32(-scale, -scale,  scale),
      vec3f32( scale, -scale,  scale),
      vec3f32( scale, -scale, -scale),
      vec3f32(-scale, -scale, -scale),
    };
    base_a = transform_quad(base_a, r);
    base_a = transform_quad(base_a, t);
    
    Quad base_b = {
      sub_vec3f32(base_a.p0, scale_vec3f32(direction, distance_vec3f32(a, b))),
      sub_vec3f32(base_a.p1, scale_vec3f32(direction, distance_vec3f32(a, b))),
      sub_vec3f32(base_a.p2, scale_vec3f32(direction, distance_vec3f32(a, b))),
      sub_vec3f32(base_a.p3, scale_vec3f32(direction, distance_vec3f32(a, b))),
    };
    
    base_a.p0 = sub_vec3f32(base_a.p0, scale_vec3f32(direction, arrow_height));
    base_a.p1 = sub_vec3f32(base_a.p1, scale_vec3f32(direction, arrow_height));
    base_a.p2 = sub_vec3f32(base_a.p2, scale_vec3f32(direction, arrow_height));
    base_a.p3 = sub_vec3f32(base_a.p3, scale_vec3f32(direction, arrow_height));
    
    color = scale_vec4f32(color, 0.8);
    renderer_push_quad(renderer, base_b, color);
    renderer_push_quad(renderer, (Quad){ base_a.p1, base_b.p1, base_b.p0, base_a.p0 }, color);
    renderer_push_quad(renderer, (Quad){ base_a.p2, base_b.p2, base_b.p1, base_a.p1 }, color);
    renderer_push_quad(renderer, (Quad){ base_a.p3, base_b.p3, base_b.p2, base_a.p2 }, color);
    renderer_push_quad(renderer, (Quad){ base_b.p3, base_a.p3, base_a.p0, base_b.p0 }, color);
  }
}

internal void renderer_push_quad(Renderer* renderer, Quad quad, Vec4f32 color) {
  if ((renderer->triangles_count + 2) >= renderer->triangles_max) {
    printf("Error :: Renderer :: Too many triangles!");
    Assert(0);
  }
  
  renderer_push_triangle(renderer, quad.p0, color, quad.p1, color, quad.p2, color);
  renderer_push_triangle(renderer, quad.p0, color, quad.p2, color, quad.p3, color);
}

internal void renderer_push_quad_texture(Renderer* renderer, Quad quad, u32 texture) {
  renderer_push_triangle_texture(renderer,
                                 quad.p0, vec2f32(0.0f, 0.0f),
                                 quad.p1, vec2f32(1.0f, 0.0f),
                                 quad.p2, vec2f32(1.0f, 1.0f),
                                 texture);
  renderer_push_triangle_texture(renderer,
                                 quad.p2, vec2f32(1.0f, 1.0f),
                                 quad.p3, vec2f32(0.0f, 1.0f),
                                 quad.p0, vec2f32(0.0f, 0.0f),
                                 texture);
}

internal void renderer_push_cube(Renderer* renderer, Cube cube) {
  renderer_push_cube_highlight_face(renderer, cube, -1, Color_Black);
}

internal void renderer_push_cube_highlight_face(Renderer* renderer, Cube cube, Cube_Face highlight, Vec4f32 highlight_color) {
  f32 thickness = Clamp(0.0f, cube.border_thickness, 1.0f);
  
  if (thickness == 0.0f || thickness == 1.0f) {
    f32 scale = 1.0f;
    Vec4f32 color = (thickness == 0.0) ? cube.color : cube.border_color;
    
    Quad back = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale, scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale, scale, 1.0f), cube.transform)};
    renderer_push_quad(renderer, back, (highlight == CubeFace_Back) ? highlight_color : cube.color);
    
    Quad front = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale, scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale, scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, scale, 1.0f), cube.transform)};
    renderer_push_quad(renderer, front, (highlight == CubeFace_Front) ? highlight_color : cube.color);
    
    Quad left = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f, scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f, scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f, scale, scale), cube.transform)};
    renderer_push_quad(renderer, left, (highlight == CubeFace_Left) ? highlight_color : cube.color);
    
    Quad right = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f, scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f, scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f, scale, scale), cube.transform)};
    renderer_push_quad(renderer, right, (highlight == CubeFace_Right) ? highlight_color : cube.color);
    
    Quad bottom = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, 1.0f, scale), cube.transform)};
    renderer_push_quad(renderer, bottom, (highlight == CubeFace_Right) ? highlight_color : cube.color);
    
    Quad top = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale, 1.0f, scale), cube.transform)};
    renderer_push_quad(renderer, top, (highlight == CubeFace_Top) ? highlight_color : cube.color);
    
  } else {
    f32 scale = 1 - (thickness);
    
    //~ On XY Plane
    {
      Quad back = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale, scale, 1.0f), cube.transform)};
      renderer_push_quad(renderer, back, (highlight == CubeFace_Back) ? highlight_color : cube.color);
      
      Quad back_left_border_quad = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0,   1.0f, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, -scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, -scale , 1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P3,                                         cube.transform)};
      renderer_push_quad(renderer, back_left_border_quad, cube.border_color);
      
      Quad back_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale,   1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1,  1.0f, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P2,                                         cube.transform)};
      renderer_push_quad(renderer, back_top_border, cube.border_color);
      
      Quad back_right_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, -scale,  1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P1,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2,   1.0f, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, -scale, scale, 1.0f), cube.transform)};
      renderer_push_quad(renderer, back_right_border, cube.border_color);
      
      Quad back_bottom_border = {
        mul_vec3f32_mat4f32(P0,                                          cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale,   1.0f, 1.0f),  cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, -scale, 1.0f),  cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,   scale, 1.0f),  cube.transform)};
      renderer_push_quad(renderer, back_bottom_border, cube.border_color);
      
      Quad front = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, scale, 1.0f), cube.transform)};
      renderer_push_quad(renderer, front, (highlight == CubeFace_Front) ? highlight_color : cube.color);
      
      Quad front_left_border = {
        mul_vec3f32_mat4f32(P7,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, -scale,  1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, -scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4,   1.0f, scale, 1.0f), cube.transform)};
      renderer_push_quad(renderer, front_left_border, cube.border_color);
      
      Quad front_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale  , 1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P6,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5,  1.0f, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, -scale, 1.0f), cube.transform)};
      renderer_push_quad(renderer, front_top_border, cube.border_color);
      
      Quad front_right_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, -scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6,   1.0f, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P5,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, -scale,  1.0f, 1.0f), cube.transform)};
      renderer_push_quad(renderer, front_right_border, cube.border_color);
      
      Quad front_bottom_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f,  -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale,   1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P4, cube.transform)};
      renderer_push_quad(renderer, front_bottom_border, cube.border_color);
    }
    
    //~ On YZ plane
    {
      Quad left = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f, scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f, scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f, scale, scale), cube.transform)};
      renderer_push_quad(renderer, left, (highlight == CubeFace_Left) ? highlight_color : cube.color);
      
      Quad left_left_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f, scale,   1.0f), cube.transform),
        mul_vec3f32_mat4f32(P3,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, 1.0f,  -scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f, scale, -scale), cube.transform)};
      renderer_push_quad(renderer, left_left_border, cube.border_color);
      
      Quad left_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f, -scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f,   1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(P7,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f,  -scale, 1.0f), cube.transform)};
      renderer_push_quad(renderer, left_top_border, cube.border_color);
      
      Quad left_right_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,  1.0f,-scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f, scale,-scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, scale,  1.0f), cube.transform),
        mul_vec3f32_mat4f32(P4, cube.transform)};
      renderer_push_quad(renderer, left_right_border, cube.border_color);
      
      Quad left_bottom_border = {
        mul_vec3f32_mat4f32(P0, cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,  scale,  1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, -scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f,   1.0f, scale), cube.transform)};
      renderer_push_quad(renderer, left_bottom_border, cube.border_color);
      
      Quad right = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f, scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f, scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f, scale, scale), cube.transform)};
      renderer_push_quad(renderer, right, (highlight == CubeFace_Right) ? highlight_color : cube.color);
      
      Quad right_left_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f, scale, -scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, 1.0f,  -scale), cube.transform),
        mul_vec3f32_mat4f32(P2,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f, scale,   1.0f), cube.transform)};
      renderer_push_quad(renderer, right_left_border, cube.border_color);
      
      Quad right_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f,  -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P6,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f,   1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f, -scale, scale), cube.transform)};
      renderer_push_quad(renderer, right_top_border, cube.border_color);
      
      Quad right_right_border = {
        mul_vec3f32_mat4f32(P5, cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, scale,  1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f, scale,-scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f,  1.0f,-scale), cube.transform)};
      renderer_push_quad(renderer, right_right_border, cube.border_color);
      
      Quad right_bottom_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f,   1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, -scale, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f,  scale,  1.0f), cube.transform),
        mul_vec3f32_mat4f32(P1, cube.transform)};
      renderer_push_quad(renderer, right_bottom_border, cube.border_color);
    }
    
    //~ On XZ plane
    {
      Quad bottom = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, 1.0f, scale), cube.transform)};
      renderer_push_quad(renderer, bottom, (highlight == CubeFace_Bottom) ? highlight_color : cube.color);
      
      Quad bottom_left_border = {
        mul_vec3f32_mat4f32(P4, cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, -scale, 1.0f,  1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, -scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0,   1.0f, 1.0f, scale), cube.transform)};
      renderer_push_quad(renderer, bottom_left_border, cube.border_color);
      
      Quad bottom_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, 1.0f,   1.0f), cube.transform),
        mul_vec3f32_mat4f32(P5,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1,  1.0f, 1.0f, -scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, 1.0f, -scale), cube.transform)};
      renderer_push_quad(renderer, bottom_top_border, cube.border_color);
      
      Quad bottom_right_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, -scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5,   1.0f, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(P1,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, -scale, 1.0f,  1.0f), cube.transform)};
      renderer_push_quad(renderer, bottom_right_border, cube.border_color);
      
      Quad bottom_bottom_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0,  1.0f, 1.0f,  scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale, 1.0f, -scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale, 1.0f,   1.0f), cube.transform),
        mul_vec3f32_mat4f32(P0,                                         cube.transform)};
      renderer_push_quad(renderer, bottom_bottom_border, cube.border_color);
      
      Quad top = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale, 1.0f, scale), cube.transform)};
      renderer_push_quad(renderer, top, (highlight == CubeFace_Top) ? highlight_color : cube.color);
      
      Quad top_left_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3,   1.0f, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, -scale, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, -scale, 1.0f,  1.0f), cube.transform),
        mul_vec3f32_mat4f32(P7, cube.transform)};
      renderer_push_quad(renderer, top_left_border, cube.border_color);
      
      Quad top_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale, 1.0f, -scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2,  1.0f, 1.0f, -scale), cube.transform),
        mul_vec3f32_mat4f32(P6,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale, 1.0f,   1.0f), cube.transform)};
      renderer_push_quad(renderer, top_top_border, cube.border_color);
      
      Quad top_right_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, -scale, 1.0f,  1.0f), cube.transform),
        mul_vec3f32_mat4f32(P2,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6,   1.0f, 1.0f, scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, -scale, 1.0f, scale), cube.transform)};
      renderer_push_quad(renderer, top_right_border, cube.border_color);
      
      Quad top_bottom_border = {
        mul_vec3f32_mat4f32(P3,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, 1.0f,   1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, 1.0f, -scale), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3,  1.0f, 1.0f,  scale), cube.transform)};
      renderer_push_quad(renderer, top_bottom_border, cube.border_color);
    }
  }
}

// NOTE(fz): Position should be in NDC
internal void renderer_push_string(Renderer* renderer, String text, Vec2f32 position, Vec4f32 color) {
  // NDC to screen
  position.x = ((position.x + 1.0f) / 2.0f) * renderer->program_state->window_width;
  position.y = ((position.y + 1.0f) / 2.0f) * renderer->program_state->window_height;
  
  Renderer_Font_Info font_info = renderer->font_info;
  
  for (u32 i = 0; i < text.size; i++) {
    if (text.str[i] >= 32 && text.str[i] < 128) {
      stbtt_packedchar* info = &font_info.cdata[text.str[i] - 32];
      f32 size = 512.0f;
      
      Quad2D atlas_location = {
        info->x0 / size, info->y0 / size,
        (info->x1 - info->x0) / size, (info->y1 - info->y0) / size
      };
      
      // Calculate screen space location
      f32 x0 = position.x + info->xoff;
      f32 x1 = x0 + (info->x1 - info->x0);
      f32 y0 = position.y - info->yoff;
      f32 y1 = y0 - (info->y1 - info->y0);
      
      // Convert screen space location to NDC
      f32 ndc_x0 = (x0 / renderer->program_state->window_width)  * 2.0f - 1.0f;
      f32 ndc_y0 = (y0 / renderer->program_state->window_height) * 2.0f - 1.0f;
      f32 ndc_x1 = (x1 / renderer->program_state->window_width)  * 2.0f - 1.0f;
      f32 ndc_y1 = (y1 / renderer->program_state->window_height) * 2.0f - 1.0f;
      
      Vec3f32 top_left_pos     = { ndc_x0, ndc_y0, 0.0f };
      Vec3f32 top_right_pos    = { ndc_x1, ndc_y0, 0.0f };
      Vec3f32 bottom_left_pos  = { ndc_x0, ndc_y1, 0.0f };
      Vec3f32 bottom_right_pos = { ndc_x1, ndc_y1, 0.0f };
      
      // UV coords of this character in the texture atlas
      Vec2f32 top_left_uv     = { atlas_location.x, atlas_location.y };
      Vec2f32 top_right_uv    = { atlas_location.x + atlas_location.width, atlas_location.y };
      Vec2f32 bottom_left_uv  = { atlas_location.x, atlas_location.y + atlas_location.height };
      Vec2f32 bottom_right_uv = { atlas_location.x + atlas_location.width, atlas_location.y + atlas_location.height };
      
      renderer_push_triangle_texture_color(renderer, 
                                           top_left_pos, top_left_uv, 
                                           top_right_pos, top_right_uv, 
                                           bottom_left_pos, bottom_left_uv, 
                                           color, font_info.font_texture);
      renderer_push_triangle_texture_color(renderer, 
                                           top_right_pos, top_right_uv, 
                                           bottom_right_pos, bottom_right_uv, 
                                           bottom_left_pos, bottom_left_uv, 
                                           color, font_info.font_texture);
      
      position.x += info->xadvance;
    }
  }
}


internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("Mat4f32 :: Uniform %s not found\n", uniform);
  }
  glUniformMatrix4fv(uniform_location, 1, 1, &mat.data[0][0]);
}

internal void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("s32 :: Uniform %s not found\n", uniform);
  }
  glUniform1i(uniform_location, s);
}