#include "shader_source.c"

function Renderer renderer_init(s32 window_width, s32 window_height) {
  Renderer result = { 0 };

  // --- Compile shader
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	{
		glShaderSource(vertex_shader, 1, &VertexShaderCode, NULL);
		glCompileShader(vertex_shader);
		{
			s32  success;
			glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
				printf("Error %d compiling vertex shader. Log: %s", success, infoLog);
				Assert(0);
			}
		}
	}

	u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	{
		glShaderSource(fragment_shader, 1, &FragmentShaderCode, NULL);
		glCompileShader(fragment_shader);
		{
			s32  success;
			glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
				printf("Error %d compiling fragment shader. Log: %s", success, infoLog);
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
	
	glGenBuffers(1, &result.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
	glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(RendererVertex), NULL, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, sizeof(RendererVertex), (void*) offsetof(RendererVertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_False, sizeof(RendererVertex), (void*) offsetof(RendererVertex, color));
	glEnableVertexAttribArray(1);
	
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
		glShaderSource(screen_vertex_shader, 1, &ScreenVertexShaderCode, NULL);
		glCompileShader(screen_vertex_shader);
		{
			s32  success;
			glGetShaderiv(screen_vertex_shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				glGetShaderInfoLog(screen_vertex_shader, 1024, NULL, infoLog);
				printf("Error %d compiling vertex shader. Log: %s", success, infoLog);
				Assert(0);
			}
		}
	}

	u32 screen_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	{
		glShaderSource(screen_fragment_shader, 1, &ScreenFragmentShaderCode, NULL);
		glCompileShader(screen_fragment_shader);
		{
			s32  success;
			glGetShaderiv(screen_fragment_shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				glGetShaderInfoLog(screen_fragment_shader, 1024, NULL, infoLog);
				printf("Error %d compiling fragment shader. Log: %s", success, infoLog);
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

	return result;
}

function void renderer_free(Renderer* renderer) {
	glDeleteVertexArrays(1, &renderer->vao);
	glDeleteBuffers(1, &renderer->vbo);
	glDeleteProgram(renderer->shader_program);
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
	glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->triangle_count * 3 * sizeof(RendererVertex), renderer->triangle_data);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CCW);
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

function void renderer_push_quad(Renderer* renderer, Quad quad, Vec4f32 color) {
	if ((renderer->triangle_count + 2) >= MAX_TRIANGLES) {
		printf("Error :: Renderer :: Too many triangles!");
		Assert(0);
	}

  // renderer_push_triangle(renderer, quad.p0, color, quad.p1, color, quad.p2, color);
  renderer_push_triangle(renderer, quad.p0, color, quad.p1, color, quad.p2, color);
  renderer_push_triangle(renderer, quad.p0, color, quad.p2, color, quad.p3, color);
}

function void renderer_push_cube(Renderer* renderer, Cube cube, Vec4f32 border_color) {
  f32 scale = 1-CubeBorderThickness;

	// On XY Plane
  {
		Mat4f32 final_transform = mul_mat4f32(scale_mat4f32(scale, scale, 1.0f), cube.transform);
		CubeVertices vertices_transformed = cube_vertices_apply_transform(CubeVerticesLocalSpace, final_transform);

	  // ---  Back Quad
    Quad back = cube_vertices_get_quad_back(vertices_transformed);
    renderer_push_quad(renderer, back, cube.color);
    
    // --- Borders
    {
      Quad back_left_border_quad = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0,   1.0f, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, -scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, -scale , 1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P3,                                         cube.transform)
      };
      renderer_push_quad(renderer, back_left_border_quad, border_color);

      Quad back_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale,   1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1,  1.0f, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P2,                                         cube.transform)
      };
      renderer_push_quad(renderer, back_top_border, border_color);

      Quad back_right_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, -scale,  1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P1,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2,   1.0f, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, -scale, scale, 1.0f), cube.transform)
      };
      renderer_push_quad(renderer, back_right_border, border_color);

      Quad back_bottom_border = {
        mul_vec3f32_mat4f32(P0,                                          cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale,   1.0f, 1.0f),  cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, -scale, 1.0f),  cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,   scale, 1.0f),  cube.transform)
      };
      renderer_push_quad(renderer, back_bottom_border, border_color);
    }

	  // ---  Front Quad
    Quad front = cube_vertices_get_quad_front(vertices_transformed);
    renderer_push_quad(renderer, front, cube.color);

    // --- Border
    {
      Quad front_left_border = {
        mul_vec3f32_mat4f32(P7,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, -scale,  1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, -scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4,   1.0f, scale, 1.0f), cube.transform),
      };
      renderer_push_quad(renderer, front_left_border, border_color);

      Quad front_top_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale  , 1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P6,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5,  1.0f, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, -scale, 1.0f), cube.transform),
      };
      renderer_push_quad(renderer, front_top_border, border_color);

      Quad front_right_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, -scale, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6,   1.0f, scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P5,                                         cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, -scale,  1.0f, 1.0f), cube.transform),
      };
      renderer_push_quad(renderer, front_right_border, border_color);

      Quad front_bottom_border = {
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f,  -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, -scale, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale,   1.0f, 1.0f), cube.transform),
        mul_vec3f32_mat4f32(P4, cube.transform),
      };
      renderer_push_quad(renderer, front_bottom_border, border_color);
    }
  }

	// On YZ plane
  {
		Mat4f32 final_transform = mul_mat4f32(scale_mat4f32(1.0f, scale, scale), cube.transform);
		CubeVertices vertices_transformed = cube_vertices_apply_transform(CubeVerticesLocalSpace, final_transform);

		// --- Left Quad
    Quad left = cube_vertices_get_quad_left(vertices_transformed);
    renderer_push_quad(renderer, left, cube.color);

    Quad left_left_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f, scale,   1.0f), cube.transform),
      mul_vec3f32_mat4f32(P3,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, 1.0f,  -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f, scale, -scale), cube.transform),
    };
    renderer_push_quad(renderer, left_left_border, border_color);

    Quad left_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f, -scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f,   1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(P7,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f,  -scale, 1.0f), cube.transform),
    };
    renderer_push_quad(renderer, left_top_border, border_color);

    Quad left_right_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,  1.0f,-scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, 1.0f, scale,-scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(P4, cube.transform),
    };
    renderer_push_quad(renderer, left_right_border, border_color);

    Quad left_bottom_border = {
      mul_vec3f32_mat4f32(P0, cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, 1.0f,  scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, 1.0f, -scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, 1.0f,   1.0f, scale), cube.transform),
    };
    renderer_push_quad(renderer, left_bottom_border, border_color);

		// --- Right Quad
    Quad right = cube_vertices_get_quad_right(vertices_transformed);
    renderer_push_quad(renderer, right, cube.color);

    Quad right_left_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f, scale, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, 1.0f,  -scale), cube.transform),
      mul_vec3f32_mat4f32(P2,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f, scale,   1.0f), cube.transform),
    };
    renderer_push_quad(renderer, right_left_border, border_color);

    Quad right_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f,  -scale, 1.0f), cube.transform),
      mul_vec3f32_mat4f32(P6,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f,   1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f, -scale, scale), cube.transform),
    };
    renderer_push_quad(renderer, right_top_border, border_color);

    Quad right_right_border = {
      mul_vec3f32_mat4f32(P5, cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, 1.0f, scale,-scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f,  1.0f,-scale), cube.transform),
    };
    renderer_push_quad(renderer, right_right_border, border_color);

    Quad right_bottom_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, 1.0f,   1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, 1.0f, -scale, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, 1.0f,  scale,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(P1, cube.transform),
    };
    renderer_push_quad(renderer, right_bottom_border, border_color);
	}

	// On XZ plane
  {
		Mat4f32 final_transform = mul_mat4f32(scale_mat4f32(scale, 1.0f, scale), cube.transform);
		CubeVertices vertices_transformed = cube_vertices_apply_transform(CubeVerticesLocalSpace, final_transform);

		// ---  Bottom Quad
    Quad bottom = cube_vertices_get_quad_bot(vertices_transformed);
    renderer_push_quad(renderer, bottom, cube.color);

    Quad bottom_left_border = {
      mul_vec3f32_mat4f32(P4, cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, -scale, 1.0f,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, -scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0,   1.0f, 1.0f, scale), cube.transform),
    };
    renderer_push_quad(renderer, bottom_left_border, border_color);

    Quad bottom_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, scale, 1.0f,   1.0f), cube.transform),
      mul_vec3f32_mat4f32(P5,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1,  1.0f, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, scale, 1.0f, -scale), cube.transform),
    };
    renderer_push_quad(renderer, bottom_top_border, border_color);

    Quad bottom_right_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P4, -scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5,   1.0f, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(P1,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0, -scale, 1.0f,  1.0f), cube.transform),
    };
    renderer_push_quad(renderer, bottom_right_border, border_color);

    Quad bottom_bottom_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P0,  1.0f, 1.0f,  scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P5, scale, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P1, scale, 1.0f,   1.0f), cube.transform),
      mul_vec3f32_mat4f32(P0,                                         cube.transform),
    };
    renderer_push_quad(renderer, bottom_bottom_border, border_color);

		// ---  Top Quad
    Quad top = cube_vertices_get_quad_top(vertices_transformed);
    renderer_push_quad(renderer, top, cube.color);

    Quad top_left_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3,   1.0f, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, -scale, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, -scale, 1.0f,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(P7, cube.transform),
    };
    renderer_push_quad(renderer, top_left_border, border_color);

    Quad top_top_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, scale, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2,  1.0f, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(P6,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, scale, 1.0f,   1.0f), cube.transform),
    };
    renderer_push_quad(renderer, top_top_border, border_color);

    Quad top_right_border = {
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3, -scale, 1.0f,  1.0f), cube.transform),
      mul_vec3f32_mat4f32(P2,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6,   1.0f, 1.0f, scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P7, -scale, 1.0f, scale), cube.transform),
    };
    renderer_push_quad(renderer, top_right_border, border_color);

    Quad top_bottom_border = {
      mul_vec3f32_mat4f32(P3,                                         cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P2, scale, 1.0f,   1.0f), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P6, scale, 1.0f, -scale), cube.transform),
      mul_vec3f32_mat4f32(scale_vec3f32_xyz(P3,  1.0f, 1.0f,  scale), cube.transform),
    };
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