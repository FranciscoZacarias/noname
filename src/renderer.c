
function Renderer renderer_init(Arena* arena, s32 window_width, s32 window_height) {
	Arena_Temp arena_temp = arena_temp_begin(arena);
  Renderer result = { 0 };

  // --- Compile shader
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	{
		OS_File vertex_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(DEFAULT_VERTEX_SHADER));
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
		OS_File fragment_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(DEFAULT_FRAGMENT_SHADER));
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
		glBufferData(GL_ARRAY_BUFFER, MAX_TRIANGLES_VERTICES * sizeof(Renderer_Vertex), NULL, GL_DYNAMIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, sizeof(Renderer_Vertex), (void*) OffsetOfMember(Renderer_Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_False, sizeof(Renderer_Vertex), (void*) OffsetOfMember(Renderer_Vertex, color));
		glEnableVertexAttribArray(1);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// --- MSAA: FBO and RBO setup
	glGenFramebuffers(1, &result.msaa_frame_buffer_object);

	glGenTextures(1, &result.msaa_texture_color_buffer_multisampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, result.msaa_texture_color_buffer_multisampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, window_width, window_height, GL_True);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glGenRenderbuffers(1, &result.msaa_render_buffer_object);
	glBindRenderbuffer(GL_RENDERBUFFER, result.msaa_render_buffer_object);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, window_width, window_height);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
		OS_File vertex_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(SCREEN_VERTEX_SHADER));
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
		OS_File vertex_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(SCREEN_FRAGMENT_SHADER));
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

	arena_temp_end(&arena_temp);

	return result;
}

function void renderer_generate_msaa_and_intermidiate_buffers(Renderer* renderer, s32 window_width, s32 window_height) {
	// Delete and recreate MSAA framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, renderer->msaa_frame_buffer_object);
	glDeleteTextures(1, &renderer->msaa_texture_color_buffer_multisampled);
	glDeleteRenderbuffers(1, &renderer->msaa_render_buffer_object);

	// Regen MSAA buffer
	glGenFramebuffers(1, &renderer->msaa_frame_buffer_object);

	glGenTextures(1, &renderer->msaa_texture_color_buffer_multisampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderer->msaa_texture_color_buffer_multisampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, window_width, window_height, GL_True);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glGenRenderbuffers(1, &renderer->msaa_render_buffer_object);
	glBindRenderbuffer(GL_RENDERBUFFER, renderer->msaa_render_buffer_object);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, window_width, window_height);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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

function void renderer_recompile_default_shader(Arena* arena, Renderer* renderer) {
	Arena_Temp arena_temp = arena_temp_begin(arena);

  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	{
		OS_File vertex_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(DEFAULT_VERTEX_SHADER));
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
		OS_File fragment_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(DEFAULT_FRAGMENT_SHADER));
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

	arena_temp_end(&arena_temp);
}

function void renderer_recompile_screen_shader(Arena* arena, Renderer* renderer) {
	Arena_Temp arena_temp = arena_temp_begin(arena);

  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	{
		OS_File vertex_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(SCREEN_VERTEX_SHADER));
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
		OS_File vertex_shader_source = os_file_load_entire_file(arena_temp.arena, StringLiteral(SCREEN_FRAGMENT_SHADER));
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

	arena_temp_end(&arena_temp);
}

function void renderer_free(Renderer* renderer) {
	glDeleteVertexArrays(1, &renderer->vao);
	glDeleteBuffers(1, &renderer->triangle_vbo);
	glDeleteProgram(renderer->shader_program);

	glDeleteVertexArrays(1, &renderer->screen_vao);
	glDeleteBuffers(1, &renderer->screen_vbo);
	glDeleteProgram(renderer->screen_program);
}

function void renderer_begin_frame(Renderer* renderer, Vec4f32 background_color) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->msaa_frame_buffer_object);
	glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	renderer->triangle_count = 0;

	glUseProgram(renderer->shader_program);
}

void renderer_end_frame(Renderer* renderer, s32 window_width, s32 window_height) {
	glBindVertexArray(renderer->vao);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->triangle_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->triangle_count * 3 * sizeof(Renderer_Vertex), renderer->triangle_data);

#if ENABLE_CULL
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CCW);
#endif
	glDrawArrays(GL_TRIANGLES, 0, renderer->triangle_count * 3);
  glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->msaa_frame_buffer_object);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->postprocessing_fbo);	
	glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(renderer->screen_program);
	glBindVertexArray(renderer->screen_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderer->screen_texture);
	renderer_set_uniform_s32(renderer->screen_program, "window_width", window_width);
	renderer_set_uniform_s32(renderer->screen_program, "window_height", window_height);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

function void renderer_push_triangle(Renderer* renderer, Vec3f32 a_position, Vec4f32 a_color, Vec3f32 b_position, Vec4f32 b_color, Vec3f32 c_position, Vec4f32 c_color) {
	if ((renderer->triangle_count + 1) >= MAX_TRIANGLES) {
		printf("Error :: Renderer :: Too many triangles!");
		Assert(0);
	}

	s64 index = renderer->triangle_count * 3;
	renderer->triangle_data[index+0].position = a_position;
	renderer->triangle_data[index+0].color = a_color;
	renderer->triangle_data[index+1].position = b_position;
	renderer->triangle_data[index+1].color = b_color;
	renderer->triangle_data[index+2].position = c_position;
	renderer->triangle_data[index+2].color = c_color;

	renderer->triangle_count += 1;
}

function void renderer_push_arrow(Renderer* renderer, Vec3f32 a, Vec3f32 b, Vec4f32 color, f32 scale) {
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
		renderer_push_triangle(renderer, base_a.p0, color, arrow_top, COLOR_BLACK, base_a.p1, color);
		renderer_push_triangle(renderer, base_a.p1, color, arrow_top, COLOR_BLACK, base_a.p2, color);
		renderer_push_triangle(renderer, base_a.p2, color, arrow_top, COLOR_BLACK, base_a.p3, color);
		renderer_push_triangle(renderer, base_a.p3, color, arrow_top, COLOR_BLACK, base_a.p0, color);
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

		color = scale_vec4f32(color, 0.7);
		renderer_push_quad(renderer, base_b, color);
		renderer_push_quad(renderer, (Quad){ base_a.p1, base_b.p1, base_b.p0, base_a.p0 }, color);
		renderer_push_quad(renderer, (Quad){ base_a.p2, base_b.p2, base_b.p1, base_a.p1 }, color);
		renderer_push_quad(renderer, (Quad){ base_a.p3, base_b.p3, base_b.p2, base_a.p2 }, color);
		renderer_push_quad(renderer, (Quad){ base_b.p3, base_a.p3, base_a.p0, base_b.p0 }, color);
	}
}

function void renderer_push_quad(Renderer* renderer, Quad quad, Vec4f32 color) {
	if ((renderer->triangle_count + 2) >= MAX_TRIANGLES) {
		printf("Error :: Renderer :: Too many triangles!");
		Assert(0);
	}

  renderer_push_triangle(renderer, quad.p0, color, quad.p1, color, quad.p2, color);
  renderer_push_triangle(renderer, quad.p0, color, quad.p2, color, quad.p3, color);
}

function void renderer_push_cube(Renderer* renderer, Cube cube, Vec4f32 border_color) {
	renderer_push_cube_highlight_face(renderer, cube, border_color, -1, COLOR_BLACK);
}

function void renderer_push_cube_highlight_face(Renderer* renderer, Cube cube, Vec4f32 border_color, Cube_Face highlight, Vec4f32 highlight_color) {
  f32 scale = 1-CubeBorderThickness;

	// On XY Plane
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
		renderer_push_quad(renderer, back_left_border_quad, border_color);

		Quad back_top_border = {
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale,   1.0f, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, -scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1,  1.0f, -scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(P2,                                         cube.transform)};
		renderer_push_quad(renderer, back_top_border, border_color);

		Quad back_right_border = {
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, -scale,  1.0f, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(P1,                                         cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2,   1.0f, scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, -scale, scale, 1.0f), cube.transform)};
		renderer_push_quad(renderer, back_right_border, border_color);

		Quad back_bottom_border = {
			mul_vec3f32_mat4f32(P0,                                          cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale,   1.0f, 1.0f),  cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, -scale, 1.0f),  cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,   scale, 1.0f),  cube.transform)};
		renderer_push_quad(renderer, back_bottom_border, border_color);

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
		renderer_push_quad(renderer, front_left_border, border_color);

		Quad front_top_border = {
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale  , 1.0f, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(P6,                                         cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5,  1.0f, -scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, -scale, 1.0f), cube.transform)};
		renderer_push_quad(renderer, front_top_border, border_color);

		Quad front_right_border = {
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, -scale, scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6,   1.0f, scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(P5,                                         cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, -scale,  1.0f, 1.0f), cube.transform)};
		renderer_push_quad(renderer, front_right_border, border_color);

		Quad front_bottom_border = {
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f,  -scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, -scale, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale,   1.0f, 1.0f), cube.transform),
			mul_vec3f32_mat4f32(P4, cube.transform)};
		renderer_push_quad(renderer, front_bottom_border, border_color);
  }

	// On YZ plane
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
    renderer_push_quad(renderer, left_left_border, border_color);

    Quad left_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f, -scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f,   1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(P7,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f,  -scale, 1.0f), cube.transform)};
    renderer_push_quad(renderer, left_top_border, border_color);

    Quad left_right_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,  1.0f,-scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f, scale,-scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(P4, cube.transform)};
    renderer_push_quad(renderer, left_right_border, border_color);

    Quad left_bottom_border = {
      mul_vec3f32_mat4f32(P0, cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,  scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, -scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f,   1.0f, scale), cube.transform)};
    renderer_push_quad(renderer, left_bottom_border, border_color);

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
    renderer_push_quad(renderer, right_left_border, border_color);

    Quad right_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f,  -scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(P6,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f,   1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f, -scale, scale), cube.transform)};
    renderer_push_quad(renderer, right_top_border, border_color);

    Quad right_right_border = {
      mul_vec3f32_mat4f32(P5, cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f, scale,-scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f,  1.0f,-scale), cube.transform)};
    renderer_push_quad(renderer, right_right_border, border_color);

    Quad right_bottom_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f,   1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, -scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f,  scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(P1, cube.transform)};
    renderer_push_quad(renderer, right_bottom_border, border_color);
	}

	// On XZ plane
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
    renderer_push_quad(renderer, bottom_left_border, border_color);

    Quad bottom_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, 1.0f,   1.0f), cube.transform),
      mul_vec3f32_mat4f32(P5,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1,  1.0f, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, 1.0f, -scale), cube.transform)};
    renderer_push_quad(renderer, bottom_top_border, border_color);

    Quad bottom_right_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, -scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5,   1.0f, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(P1,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, -scale, 1.0f,  1.0f), cube.transform)};
    renderer_push_quad(renderer, bottom_right_border, border_color);

    Quad bottom_bottom_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0,  1.0f, 1.0f,  scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale, 1.0f,   1.0f), cube.transform),
      mul_vec3f32_mat4f32(P0,                                         cube.transform)};
    renderer_push_quad(renderer, bottom_bottom_border, border_color);

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
    renderer_push_quad(renderer, top_left_border, border_color);

    Quad top_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2,  1.0f, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(P6,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale, 1.0f,   1.0f), cube.transform)};
    renderer_push_quad(renderer, top_top_border, border_color);

    Quad top_right_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, -scale, 1.0f,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(P2,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6,   1.0f, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, -scale, 1.0f, scale), cube.transform)};
    renderer_push_quad(renderer, top_right_border, border_color);

    Quad top_bottom_border = {
      mul_vec3f32_mat4f32(P3,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, 1.0f,   1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3,  1.0f, 1.0f,  scale), cube.transform)};
    renderer_push_quad(renderer, top_bottom_border, border_color);
	}
}

function void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat) {
	s32 uniform_location = glGetUniformLocation(program, uniform);
	if (uniform_location == -1) {
		printf("Mat4f32 :: Uniform %s not found\n", uniform);
	}
	glUniformMatrix4fv(uniform_location, 1, 1, &mat.data[0][0]);
}

function void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s) {
	s32 uniform_location = glGetUniformLocation(program, uniform);
	if (uniform_location == -1) {
		printf("s32 :: Uniform %s not found\n", uniform);
	}
	glUniform1i(uniform_location, s);
}