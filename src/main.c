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
global Vec3f32 hovered_p1 = { 0 };
global Vec3f32 hovered_p2 = { 0 };
global Vec3f32 hovered_p3 = { 0 };

global Cube Cubes[1024];
global u32 TotalCubes = 0;

global Vec3f32 CubeToAddPosiiton = { 0 };
global b32 AddCube = 0;

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
	cube_lines_program_init();

	// World Axis -------------
	Shader axis_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX());
	f32 axsxyz = 32.0f;
	f32 axis_xyz[] = {
		axsxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, //  X
	 -axsxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, // -X
			0.0f,  axsxyz,    0.0f, 0.0f, 1.0f, 0.0f, //  Y
			0.0f, -axsxyz,    0.0f, 0.0f, 1.0f, 0.0f, // -Y
			0.0f,    0.0f,  axsxyz, 0.0f, 0.0f, 1.0f, //  Z
			0.0f,    0.0f, -axsxyz, 0.0f, 0.0f, 1.0f  // -Z
	// World Axis -------------
	Shader axis_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX());
	f32 axsxyz = 32.0f;
	f32 axis_xyz[] = {
		axsxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, //  X
	 -axsxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, // -X
			0.0f,  axsxyz,    0.0f, 0.0f, 1.0f, 0.0f, //  Y
			0.0f, -axsxyz,    0.0f, 0.0f, 1.0f, 0.0f, // -Y
			0.0f,    0.0f,  axsxyz, 0.0f, 0.0f, 1.0f, //  Z
			0.0f,    0.0f, -axsxyz, 0.0f, 0.0f, 1.0f  // -Z
	};

	u32 VBO_axis, VAO_axis;
	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);

	glBindVertexArray(VAO_axis);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_xyz), axis_xyz, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_xyz), axis_xyz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)(3* sizeof(f32)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Cybe gismos -------------
	/*
	Shader gismos_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX());
	f32 gizxyz = 2.0f;
	f32 gizmos_xyz[] = {
		gizxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, //  X
	    0.0f,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, // -X
			0.0f,  gizxyz,    0.0f, 0.0f, 1.0f, 0.0f, //  Y
			0.0f,    0.0f,    0.0f, 0.0f, 1.0f, 0.0f, // -Y
			0.0f,    0.0f,  gizxyz, 0.0f, 0.0f, 1.0f, //  Z
			0.0f,    0.0f,    0.0f, 0.0f, 0.0f, 1.0f  // -Z
	};

	u32 VBO_gismos, VAO_gismos;
	glGenVertexArrays(1, &VAO_gismos);
	glGenBuffers(1, &VBO_gismos);

	glBindVertexArray(VAO_gismos);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_gismos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gizmos_xyz), gizmos_xyz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)(3* sizeof(f32)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/

	while(!glfwWindowShouldClose(window)) {
		f32 currentFrame = (f32)(glfwGetTime());
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		process_input(window);

		glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		// Draw Axis
		shader_use(axis_program);
		shader_use(axis_program);
		{
			glLineWidth(2.0f);

			glBindVertexArray(VAO_axis);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);

			Mat4f32 model = mat4f32(1.0f);
			shader_set_uniform_mat4fv(axis_program, "model", model);
			shader_set_uniform_mat4fv(axis_program, "view", view);
			shader_set_uniform_mat4fv(axis_program, "projection", projection);
			shader_set_uniform_mat4fv(axis_program, "model", model);
			shader_set_uniform_mat4fv(axis_program, "view", view);
			shader_set_uniform_mat4fv(axis_program, "projection", projection);

			glDrawArrays(GL_LINES, 0, 6);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		// Picking Phase
		{
			// picking phase
			HoveredCubeIndex = U32_MAX;
			hovered_p1 = vec3f32(0.f, 0.f, 0.f);;
			hovered_p2 = vec3f32(0.f, 0.f, 0.f);;
			hovered_p3 = vec3f32(0.f, 0.f, 0.f);;

			u32 hovered_cube_distance_to_camera  = U32_MAX;
			u32 selected_cube_distance_to_camera = U32_MAX;
			
			for(u32 i = 0; i < TotalCubes; i++) {
				Cube copy = Cubes[i];
				CubeVertices transformed_vertices = cube_get_transformed_vertices(copy);

				for (u32 j = 0; j < ArrayCount(CubeObjectIndices); j += 6) {
					Vec3f32 p1 = transformed_vertices.v[CubeObjectIndices[j+0]];
					Vec3f32 p2 = transformed_vertices.v[CubeObjectIndices[j+1]];
					Vec3f32 p3 = transformed_vertices.v[CubeObjectIndices[j+2]];

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
				for (u32 j = 0; j < ArrayCount(CubeObjectIndices); j += 6) {
					Vec3f32 p1 = transformed_vertices.v[CubeObjectIndices[j+0]];
					Vec3f32 p2 = transformed_vertices.v[CubeObjectIndices[j+1]];
					Vec3f32 p3 = transformed_vertices.v[CubeObjectIndices[j+2]];
					Vec3f32 intersection = intersect_line_with_plane(linef32(vec3f32(camera.position.x, camera.position.y, camera.position.z), Raycast), p1, p2, p3);

					if (is_vector_inside_rectangle(intersection, p1, p2, p3)) {
						Vec3f32 cube_center = cube_get_center(Cubes[HoveredCubeIndex]);
						if (hovered_p1.x == 0 && hovered_p1.y == 0 && hovered_p1.z == 0 &&
								hovered_p2.x == 0 && hovered_p2.y == 0 && hovered_p2.z == 0 &&
								hovered_p3.x == 0 && hovered_p3.y == 0 && hovered_p3.z == 0) {
							hovered_p1 = p1;
							hovered_p2 = p2;
							hovered_p3 = p3;
						} else {
							Vec3f32 plane_hovered_v1 = sub_vec3f32(hovered_p2, hovered_p1);
							Vec3f32 plane_hovered_v2 = sub_vec3f32(hovered_p3, hovered_p1);
							Vec3f32 normal_plane_hovered = cross_vec3f32(plane_hovered_v1, plane_hovered_v2);
							f32 hovered_normal_angle_with_camera = angle_vec3f32(camera.front, normal_plane_hovered);

							Vec3f32 plane_iteration_v1 = sub_vec3f32(p2, p1);
							Vec3f32 plane_iteration_v2 = sub_vec3f32(p3, p1);
							Vec3f32 normal_plane_iteration = cross_vec3f32(plane_iteration_v1, plane_iteration_v2);
							f32 iteration_normal_angle_with_camera = angle_vec3f32(camera.front, normal_plane_iteration);
							
							if (iteration_normal_angle_with_camera > hovered_normal_angle_with_camera) {
								hovered_p1 = p1;
								hovered_p2 = p2;
								hovered_p3 = p3;
							}
						}
					}
				}
			}

			// Add cube to the face being hovered:
			if (F_KeyState && HoveredCubeIndex != U32_MAX) {
				Vec3f32 cube_center = cube_get_center(Cubes[HoveredCubeIndex]);
				Vec3f32 v1 = sub_vec3f32(hovered_p2, hovered_p1);
				Vec3f32 v2 = sub_vec3f32(hovered_p3, hovered_p1);
				Vec3f32 face_normal = normalize_vec3f32(cross_vec3f32(v1, v2));
				// Dot product between point and plane (normal and a point)
				f32 dot = (cube_center.x - hovered_p1.x) * face_normal.x +
									(cube_center.y - hovered_p1.y) * face_normal.y +
									(cube_center.z - hovered_p1.z) * face_normal.z;
				
				Vec3f32 new_cube_position;
				if (dot > 0) { // Point is on the side the normal is pointing twoards
				if (dot > 0) { // Point is on the side the normal is pointing twoards
					new_cube_position = add_vec3f32(cube_center, scale_vec3f32(face_normal, -2));
				} else { // Point is on the opposite side of the normal
				} else { // Point is on the opposite side of the normal
					new_cube_position = add_vec3f32(cube_center, scale_vec3f32(face_normal, 2));
				}

				CubeToAddPosiiton = new_cube_position;
				AddCube = 1;
			}
		}

		// Draw cubes
		{
			for(u32 i = 0; i < TotalCubes; i++) {
				Cube cube = Cubes[i];
				cube_program_draw(cube, view, projection);

				// Draw outline of the cube
				{
					Cube temp_cube_just_edges = cube;
					temp_cube_just_edges.color = vec3f32(0.0f, 0.0f, 0.0f);
					cube_lines_draw(temp_cube_just_edges, view, projection);
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
						cube_program_draw(gizmos_x, view, projection);
						// Y
						Cube gizmos_y       = cube_create(cube_position, vec3f32(0.0f, 1.0f, 0.0f));
						Mat4f32 translate_y = translate_mat4f32(0.0f, gizmos_length, 0.0f);
						gizmos_y.transform  = mul_mat4f32(translate_y, gizmos_y.transform);
						Mat4f32 scale_y     = scale_mat4f32(gizmos_scale, gizmos_length, gizmos_scale);
						gizmos_y.transform  = mul_mat4f32(scale_y, gizmos_y.transform);
						cube_program_draw(gizmos_y, view, projection);
						// Z
						Cube gizmos_z       = cube_create(cube_position, vec3f32(0.0f, 0.0f, 1.0f));
						Mat4f32 translate_z = translate_mat4f32(0.0f, 0.0f, gizmos_length);
						gizmos_z.transform  = mul_mat4f32(translate_z, gizmos_z.transform);
						Mat4f32 scale_z     = scale_mat4f32(gizmos_scale, gizmos_scale, gizmos_length);
						gizmos_z.transform  = mul_mat4f32(scale_z, gizmos_z.transform);
						cube_program_draw(gizmos_z, view, projection);
					}


					/*
					shader_use(gismos_program);
					{
						glBindVertexArray(VAO_gismos);
						glBindBuffer(GL_ARRAY_BUFFER, VBO_gismos);

						Mat4f32 model = mat4f32(1.0f);
						model = mul_mat4f32(cube.transform, model);
						shader_set_uniform_mat4fv(gismos_program, "model", model);
						shader_set_uniform_mat4fv(gismos_program, "view", view);
						shader_set_uniform_mat4fv(gismos_program, "projection", projection);

						glLineWidth(5.0f);
						glDrawArrays(GL_LINES, 0, 6);
						glLineWidth(1.0f);

						glBindVertexArray(0);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
					}
					*/

					// Draw highlight of selected cube
					{
						glLineWidth(2.0f);
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						cube.color = vec3f32(sin(8.0f*glfwGetTime()) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (2*PI/3)) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (4*PI/3)) * 0.5f + 0.5f);
						cube_scale(&cube, vec3f32(1.01f, 1.01f, 1.01f));
						cube_program_draw(cube, view, projection);
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
						cube_program_draw(hover_cube, view, projection);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glLineWidth(1.0f);
					}
				}
			}

			if (AddCube) {
				Cubes[TotalCubes++] = cube_create(CubeToAddPosiiton,  vec3f32(0.8118f, 0.6627, 0.5451f));
				AddCube = 0;
			}
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