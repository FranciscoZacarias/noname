#include "main.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

global s32 WindowWidth  = 1280;
global s32 WindowHeight = 720;
#define AspectRatio ((f32)WindowWidth/(f32)WindowHeight)

global Camera camera;
global f32 LastX;
global f32 LastY;

typedef struct MouseState {
	f32 screen_space_x;
	f32 screen_space_y;

	f32 ndc_x;
	f32 ndc_y;
} MouseState;

global MouseState Mouse = { 0 };

// Keyboard state
global b32 F_KeyPreviousState = 1;
global b32 F_KeyState = 0;

global b32 G_KeyPreviousState = 1;
global b32 G_KeyState = 0;
/////////////////

typedef enum CameraMode {
	CameraMode_Select,
	CameraMode_Fly
} CameraMode;

global CameraMode ActiveCameraMode = CameraMode_Select;
global b32 LeftMouseButton = 0;

global f32 DeltaTime = 0.0f;
global f32 LastFrame = 0.0f;

global Vec3f32 Raycast = {F32_MAX, F32_MAX, F32_MAX};

global u32  SelectedCubeIndex = U32_MAX; // Explicitly selected cube

global u32 HoveredCubeIndex = U32_MAX; // Cube being hovered by the mouse cursor, right now
global Vec3f32 HoveredP1 = { 0 };
global Vec3f32 HoveredP2 = { 0 };
global Vec3f32 Hoveredp3 = { 0 };
global f32 HoveredFaceDistanceToCamera = F32_MAX;

global Cube Cubes[2048];
global u32 TotalCubes = 0;

global Vec3f32 CubeToAddPosiiton = { 0 };
global b32 AddCube = 0;

CubeProgram screen_shader = { 0 };

int main(void) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, APP_NAME, NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// Camera and Mouse -----------
	camera = camera_create();
	LastX  = WindowWidth / 2.0f;
	LastY  = WindowHeight / 2.0f;

	Mouse.screen_space_x = LastX;
	Mouse.screen_space_y = LastY;
	Mouse.ndc_x = LastX;
	Mouse.ndc_y = LastY;

	// Cubes -------------
	Cubes[TotalCubes++] = cube_create(vec3f32( 0.0f,  0.0f,  0.0f), vec3f32(1.0f, 0.0f, 0.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32( 0.0f,  0.0f, -8.0f), vec3f32(0.0f, 1.0f, 0.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32( 0.0f, -0.0f,  8.0f), vec3f32(0.0f, 0.0f, 1.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32( 0.0f,  8.0f,  0.0f), vec3f32(1.0f, 0.5f, 0.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32( 0.0f, -8.0f,  0.0f), vec3f32(1.0f, 1.0f, 0.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32( 8.0f,  0.0f,  0.0f), vec3f32(0.5f, 0.5f, 0.5f));
	Cubes[TotalCubes++] = cube_create(vec3f32(-8.0f,  0.0f,  0.0f), vec3f32(1.0f, 0.0f, 1.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32( 8.0f,  8.0f,  8.0f), vec3f32(0.5f, 0.0f, 1.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32(-8.0f, -8.0f, -8.0f), vec3f32(0.0f, 0.0f, 0.0f));
	Cubes[TotalCubes++] = cube_create(vec3f32( 8.0f, -8.0f, -8.0f), vec3f32(0.0f, 1.0f, 1.0f));
	cube_program_init();

	//////////////////////////////////
	// World Axis -------------
	Shader axis_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX());
	f32 axis_xyz[] = {
	   32.0f,   0.0f,  0.0f,  1.0f, 0.0f, 0.0f, //  X
	  -32.0f,   0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // -X
			0.0f,  32.0f,  0.0f,  0.0f, 1.0f, 0.0f, //  Y
			0.0f, -32.0f,  0.0f,  0.0f, 1.0f, 0.0f, // -Y
			0.0f,   0.0f,  32.0f, 0.0f, 0.0f, 1.0f, //  Z
			0.0f,   0.0f, -32.0f, 0.0f, 0.0f, 1.0f  // -Z
	};

	u32 VBO_axis, VAO_axis;
	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);

	glBindVertexArray(VAO_axis);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_xyz), axis_xyz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)(3* sizeof(f32)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	///////////////////////////////////////
	// Offscreen frame buffer for MSAA
	u32 frame_buffer;
	glGenFramebuffers(1, &frame_buffer);

	// multi sampled color attachment texture
	u32 texture_color_buffer_multi_sampled;
	glGenTextures(1, &texture_color_buffer_multi_sampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer_multi_sampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, WindowWidth, WindowHeight, GL_True);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// multi sampled render buffer object for depth and stencil attachments
	u32 rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Attach to framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer_multi_sampled, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete");
	}

	///////////////////////////////////////
	// Intermidiate buffer for post-processing effects. Does not do any 3D rendering
	u32 intermidiate_fbo;
	glGenFramebuffers(1, &intermidiate_fbo);

	// create color attachment texture
	u32 screen_texture;
	glGenTextures(1, &screen_texture);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowWidth, WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach to intermidiate_fbo
	glBindFramebuffer(GL_FRAMEBUFFER, intermidiate_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	///////////////////////////////////////
	// Screen shader
	screen_shader.shader_program = shader_create(GET_SCREEN_VERTEX_SHADER(), GET_SCREEN_FRAGMENT_SHADER());
	f32 quad_vertices[] = {
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	glGenVertexArrays(1, &screen_shader.VAO);
	glGenBuffers(1, &screen_shader.VBO);

	glBindVertexArray(screen_shader.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, screen_shader.VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_False, 2 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	while(!glfwWindowShouldClose(window)) {
		f32 currentFrame = (f32)(glfwGetTime());
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		process_input(window);

		// View
		Mat4f32 view = mat4f32(1.0f);
		Mat4f32 look_at = look_at_mat4f32(camera.position, add_vec3f32(camera.position, camera.front), camera.up);
		view = mul_mat4f32(look_at, view);

		// Projection 
		Mat4f32 projection = mat4f32(1.0f);
		Mat4f32 perspective = perspective_mat4f32(Radians(45), AspectRatio, 0.1f, 100.0f);
		projection = mul_mat4f32(perspective, projection);

		// Raycast
		Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(Mouse.ndc_x, Mouse.ndc_y, 1.0f), projection, view);
		Raycast = sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(camera.position.x, camera.position.y, camera.position.z));

		// Picking Phase
		{
			HoveredCubeIndex = U32_MAX;
			HoveredP1 = vec3f32(0.f, 0.f, 0.f);
			HoveredP2 = vec3f32(0.f, 0.f, 0.f);
			Hoveredp3 = vec3f32(0.f, 0.f, 0.f);
			HoveredFaceDistanceToCamera = F32_MAX;

			u32 hovered_cube_distance_to_camera  = U32_MAX;
			u32 selected_cube_distance_to_camera = U32_MAX;
			
			for(u32 i = 0; i < TotalCubes; i++) {
				Cube copy = Cubes[i];
				if (copy.dead) continue;
				CubeVertices transformed_vertices = cube_get_transformed_vertices(copy);

				for (u32 j = 0; j < ArrayCount(CubeVerticesLocalSpace.vertices); j += 6) {
					Vec3f32 p1 = transformed_vertices.vertices[j+0];
					Vec3f32 p2 = transformed_vertices.vertices[j+1];
					Vec3f32 p3 = transformed_vertices.vertices[j+2];

					Vec3f32 intersection = intersect_line_with_plane(linef32(vec3f32(camera.position.x, camera.position.y, camera.position.z), Raycast), p1, p2, p3);

					if (is_vector_inside_rectangle(intersection, p1, p2, p3)) {
						Vec3f32 cube_center = cube_get_center(copy);

						// Pick hovered cube to highlight
						if (HoveredCubeIndex == U32_MAX) {
							HoveredCubeIndex = i;
							hovered_cube_distance_to_camera = distance_vec3f32(camera.position, cube_center);
						} else {
							f32 current_cube_distance = distance_vec3f32(camera.position, cube_center);
							if (current_cube_distance < hovered_cube_distance_to_camera) {
								HoveredCubeIndex = i;
								hovered_cube_distance_to_camera = current_cube_distance;
							}
						}

						// Pick selected cube
						if (LeftMouseButton) {
							if (SelectedCubeIndex == U32_MAX) {
								SelectedCubeIndex = i;
								selected_cube_distance_to_camera = distance_vec3f32(camera.position, cube_center);
							} else {
								f32 current_cube_distance = distance_vec3f32(camera.position, cube_center);
								if (current_cube_distance < selected_cube_distance_to_camera) {
									SelectedCubeIndex = i;
									selected_cube_distance_to_camera = current_cube_distance;
								}
							}
						}
					}
				}
			}

			// Find the face that is closest to the camera from the hovered cube.
			if (HoveredCubeIndex != U32_MAX) {
				CubeVertices transformed_vertices = cube_get_transformed_vertices(Cubes[HoveredCubeIndex]);
				for (u32 j = 0; j < ArrayCount(CubeVerticesLocalSpace.vertices); j += 6) {
					Vec3f32 p1 = transformed_vertices.vertices[j+0];
					Vec3f32 p2 = transformed_vertices.vertices[j+1];
					Vec3f32 p3 = transformed_vertices.vertices[j+2];
					Vec3f32 intersection = intersect_line_with_plane(linef32(vec3f32(camera.position.x, camera.position.y, camera.position.z), Raycast), p1, p2, p3);

					if (is_vector_inside_rectangle(intersection, p1, p2, p3)) {
						if (HoveredP1.x == 0.f && HoveredP1.y == 0.f && HoveredP1.z == 0.f,
								HoveredP2.x == 0.f && HoveredP2.y == 0.f && HoveredP2.z == 0.f,
								Hoveredp3.x == 0.f && Hoveredp3.y == 0.f && Hoveredp3.z == 0.f) {
							HoveredP1 = p1;
							HoveredP2 = p2;
							Hoveredp3 = p3;
							HoveredFaceDistanceToCamera = distance_vec3f32(intersection, camera.position);
						} else {
							if (distance_vec3f32(intersection, camera.position) < HoveredFaceDistanceToCamera) {
								HoveredP1 = p1;
								HoveredP2 = p2;
								Hoveredp3 = p3;
								HoveredFaceDistanceToCamera = distance_vec3f32(intersection, camera.position);
							}
						}
					}
				}
			}

			if (HoveredCubeIndex != U32_MAX) {
				if (G_KeyState) {
					Cubes[HoveredCubeIndex].dead = 1;
				} else if (F_KeyState) {
					Vec3f32 cube_center = cube_get_center(Cubes[HoveredCubeIndex]);
					Vec3f32 v1 = sub_vec3f32(HoveredP2, HoveredP1);
					Vec3f32 v2 = sub_vec3f32(Hoveredp3, HoveredP1);
					Vec3f32 face_normal = normalize_vec3f32(cross_vec3f32(v1, v2));
					// Dot product between point and plane (normal and a point)
					f32 dot = (cube_center.x - HoveredP1.x) * face_normal.x +
										(cube_center.y - HoveredP1.y) * face_normal.y +
										(cube_center.z - HoveredP1.z) * face_normal.z;
					Vec3f32 new_cube_position;
					if (dot > 0) { // Point is on the side the normal is pointing twoards
						new_cube_position = add_vec3f32(cube_center, scale_vec3f32(face_normal, -2));
					} else { // Point is on the opposite side of the normal
						new_cube_position = add_vec3f32(cube_center, scale_vec3f32(face_normal, 2));
					}
					CubeToAddPosiiton = new_cube_position;
					AddCube = 1;
				}
			}

			if (AddCube) {
				Cubes[TotalCubes++] = cube_create(CubeToAddPosiiton,  vec3f32(0.8118f, 0.6627, 0.5451f));
				AddCube = 0;
			}
		}

		// Draw phase
		{
			// bind multi sampled frame_buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
			glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			// Draw Axis
			shader_use(axis_program);
			{
				glLineWidth(2.0f);

				glBindVertexArray(VAO_axis);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);

				Mat4f32 model = mat4f32(1.0f);
				shader_set_uniform_mat4fv(axis_program, "model", model);
				shader_set_uniform_mat4fv(axis_program, "view", view);
				shader_set_uniform_mat4fv(axis_program, "projection", projection);

				glDrawArrays(GL_LINES, 0, 6);

				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			// Draw cubes
			for(u32 i = 0; i < TotalCubes; i++) {
				Cube cube = Cubes[i];
				if (cube.dead) continue;
				cube_draw(cube, view, projection);

				// Draw outline of the cube
				{
					Cube temp_cube_just_edges = cube;
					temp_cube_just_edges.color = vec3f32(0.0f, 0.0f, 0.0f);
					// cube_lines_draw(temp_cube_just_edges, view, projection);
				}

				if (i == SelectedCubeIndex)  {
					
					// Draw Gizmos for the selected cube
					{
						Vec3f32 cube_position = vec3f32(cube.transform.m12, cube.transform.m13, cube.transform.m14);
						f32 gizmos_length = 1.5f;
						f32 gizmos_scale  = 0.06f;

						// X
						Cube gizmos_x       = cube_create(cube_position, vec3f32(1.0f, 0.0f, 0.0f));
						Mat4f32 translate_x = translate_mat4f32(gizmos_length, 0.0f, 0.0f);
						gizmos_x.transform  = mul_mat4f32(translate_x, gizmos_x.transform);
						Mat4f32 scale_x     = scale_mat4f32(gizmos_length, gizmos_scale, gizmos_scale);
						gizmos_x.transform  = mul_mat4f32(scale_x, gizmos_x.transform);
						cube_draw(gizmos_x, view, projection);
						// Y
						Cube gizmos_y       = cube_create(cube_position, vec3f32(0.0f, 1.0f, 0.0f));
						Mat4f32 translate_y = translate_mat4f32(0.0f, gizmos_length, 0.0f);
						gizmos_y.transform  = mul_mat4f32(translate_y, gizmos_y.transform);
						Mat4f32 scale_y     = scale_mat4f32(gizmos_scale, gizmos_length, gizmos_scale);
						gizmos_y.transform  = mul_mat4f32(scale_y, gizmos_y.transform);
						cube_draw(gizmos_y, view, projection);
						// Z
						Cube gizmos_z       = cube_create(cube_position, vec3f32(0.0f, 0.0f, 1.0f));
						Mat4f32 translate_z = translate_mat4f32(0.0f, 0.0f, gizmos_length);
						gizmos_z.transform  = mul_mat4f32(translate_z, gizmos_z.transform);
						Mat4f32 scale_z     = scale_mat4f32(gizmos_scale, gizmos_scale, gizmos_length);
						gizmos_z.transform  = mul_mat4f32(scale_z, gizmos_z.transform);
						cube_draw(gizmos_z, view, projection);
					}

					// Draw highlight of selected cube
					{
						glLineWidth(2.0f);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						cube.color = vec3f32(sin(8.0f*glfwGetTime()) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (2*PI/3)) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (4*PI/3)) * 0.5f + 0.5f);
						cube_scale(&cube, vec3f32(1.01f, 1.01f, 1.01f));
						cube_draw(cube, view, projection);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glLineWidth(1.0f);
					}
				}

				if (HoveredCubeIndex == i) {
					// Draw hovered highlights
					{
						Cube hover_cube = cube;
						glLineWidth(2.0f);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						hover_cube.color = vec3f32(sin(8.0f*glfwGetTime() + (2*PI/3)) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (4*PI/3)) * 0.5f + 0.5f, sin(8.0f*glfwGetTime()) * 0.5f + 0.5f);
						cube_scale(&hover_cube, vec3f32(1.1f, 1.1f, 1.1f));
						cube_draw(hover_cube, view, projection);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glLineWidth(1.0f);
					}
				}
			}

			// Copy from multisampled buffer to intermidiate frame buffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermidiate_fbo);

			// Draw into the intermidiate fbo
			glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			// glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			glUseProgram(screen_shader.shader_program);
			glBindVertexArray(screen_shader.VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, screen_texture);
			shader_set_uniform_s32(screen_shader.shader_program, "window_width", WindowWidth);
			shader_set_uniform_s32(screen_shader.shader_program, "window_height", WindowHeight);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glUseProgram(0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cube_program_clean();

	glDeleteVertexArrays(1, &VAO_axis);
	glDeleteBuffers(1, &VBO_axis);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	WindowWidth  = width;
	WindowHeight = height;
}

void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}

	local_persist b32 is_tab_down = 0;
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (!is_tab_down) {
			is_tab_down = 1;
			if (SelectedCubeIndex + 1 > 9) {
				SelectedCubeIndex = 0;
			} else {
				SelectedCubeIndex++;
			}
		}
	} else {
		if (is_tab_down) {
			is_tab_down = 0;
		}
	}

	if (SelectedCubeIndex != U32_MAX) {
		f32 step = 5.0f * DeltaTime;
		if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
			cube_translate(&Cubes[SelectedCubeIndex], vec3f32(0.0f, step, 0.0f));
		}
		if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) {
			cube_translate(&Cubes[SelectedCubeIndex], vec3f32(0.0f, -step, 0.0f));
		}
		if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
			cube_translate(&Cubes[SelectedCubeIndex], vec3f32(-step, 0.0f, 0.0f));
		}
		if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
			cube_translate(&Cubes[SelectedCubeIndex], vec3f32(step, 0.0f, 0.0f));
		}
		if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) {
			cube_translate(&Cubes[SelectedCubeIndex], vec3f32(0.0f, 0.0f, step));
		}
		if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) {
			cube_translate(&Cubes[SelectedCubeIndex], vec3f32(0.0f, 0.0f, -step));
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		LeftMouseButton = 1;
	} else {
		LeftMouseButton = 0;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (ActiveCameraMode == CameraMode_Select) {
			ActiveCameraMode = CameraMode_Fly;
			LastX = WindowWidth/2;
			LastY = WindowHeight/2;
			glfwSetCursorPos(window, WindowWidth/2, WindowHeight/2);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Front, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Back, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Left, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Right, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Down, DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera_keyboard_callback(&camera, CameraMovement_Up, DeltaTime);
		}
	} else {
		if (ActiveCameraMode == CameraMode_Fly) {
			ActiveCameraMode = CameraMode_Select;
			LastX = WindowWidth/2;
			LastY = WindowHeight/2;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, Mouse.screen_space_x, Mouse.screen_space_y);
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			if (G_KeyState == 0 && G_KeyPreviousState == 1) {
				G_KeyPreviousState = 0;
				G_KeyState = 1;
			} else {
				G_KeyState = 0;
			}
		} else {
			G_KeyPreviousState = 1;
			G_KeyState = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			if (F_KeyState == 0 && F_KeyPreviousState == 1) {
				F_KeyPreviousState = 0;
				F_KeyState = 1;
			} else {
				F_KeyState = 0;
			}
		} else {
			F_KeyPreviousState = 1;
			F_KeyState = 0;
		}
	}
}

void mouse_callback(GLFWwindow* window, f64 xposIn, f64 yposIn) {
	local_persist b32 FirstMouse = 1;

	if (ActiveCameraMode == CameraMode_Fly) {
		f32 xpos = (f32)xposIn;
		f32 ypos = (f32)yposIn;

		if (FirstMouse == 1) {
			LastX = xpos;
			LastY = ypos;
			FirstMouse = 0;
		}

		f32 xoffset = xpos - LastX;
		f32 yoffset = LastY - ypos;
		LastX = xpos;
		LastY = ypos;

		camera_mouse_callback(&camera, xoffset, yoffset);
	} else {
			Mouse.screen_space_x = xposIn;
			Mouse.screen_space_y = yposIn;

			Mouse.ndc_x = (2.0f * xposIn) / WindowWidth - 1.0f;
			Mouse.ndc_y = 1.0f - (2.0f * yposIn) / WindowHeight;
	}
}