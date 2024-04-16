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

global Cube Cubes[64];
global u32 TotalCubes = 0;

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

	// Axis -------------
	Shader lines_program = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER_LINE_COLOR_FROM_VERTEX());
	f32 lenxyz = 32.0f;
	f32 xyz[] = {
		lenxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, //  X
	 -lenxyz,    0.0f,    0.0f, 1.0f, 0.0f, 0.0f, // -X
			0.0f,  lenxyz,    0.0f, 0.0f, 1.0f, 0.0f, //  Y
			0.0f, -lenxyz,    0.0f, 0.0f, 1.0f, 0.0f, // -Y
			0.0f,    0.0f,  lenxyz, 0.0f, 0.0f, 1.0f, //  Z
			0.0f,    0.0f, -lenxyz, 0.0f, 0.0f, 1.0f  // -Z
	};

	u32 VBO_axis, VAO_axis;
	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);

	glBindVertexArray(VAO_axis);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xyz), xyz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_False, 6 * sizeof(f32), (void*)(3* sizeof(f32)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
		shader_use(lines_program);
		{
			glLineWidth(2.0f);

			glBindVertexArray(VAO_axis);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);

			Mat4f32 model = mat4f32(1.0f);
			shader_set_uniform_mat4fv(lines_program, "model", model);
			shader_set_uniform_mat4fv(lines_program, "view", view);
			shader_set_uniform_mat4fv(lines_program, "projection", projection);

			glDrawArrays(GL_LINES, 0, 6);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		// Picking Phase
		{
			// picking phase
			HoveredCubeIndex = U32_MAX;
			u32 hovered_cube_distance_to_camera  = U32_MAX;
			u32 selected_cube_distance_to_camera = U32_MAX;
			
			for(u32 i = 0; i < TotalCubes; i++) {
				Cube copy = Cubes[i];
				CubeVertices transformed_vertices = cube_get_transformed_vertices(copy);

				for (u32 j = 0; j < ArrayCount(CubeObjectIndices); j += 6) {
					Vec3f32 intersection = intersect_line_with_plane(
						linef32(vec3f32(camera.position.x, camera.position.y, camera.position.z), Raycast),
						transformed_vertices.v[CubeObjectIndices[j+0]],
						transformed_vertices.v[CubeObjectIndices[j+1]],
						transformed_vertices.v[CubeObjectIndices[j+2]]);
					
					if (is_vector_inside_rectangle(intersection,  transformed_vertices.v[CubeObjectIndices[j+0]], transformed_vertices.v[CubeObjectIndices[j+1]], transformed_vertices.v[CubeObjectIndices[j+2]])) {

						// Pick hovered cube to highlight
						if (HoveredCubeIndex == U32_MAX) {
							HoveredCubeIndex = i;
							hovered_cube_distance_to_camera = distance_vec3f32(camera.position, cube_get_center(copy));
						} else {
							f32 current_cube_distance = distance_vec3f32(camera.position, cube_get_center(copy));
							if (current_cube_distance < hovered_cube_distance_to_camera) {
								HoveredCubeIndex = i;
								hovered_cube_distance_to_camera = current_cube_distance;
							}
						}

						// Add cube to the face being hovered:
						

						// Pick selected cube
						if (LeftMouseButton) {
							if (SelectedCubeIndex == U32_MAX) {
								SelectedCubeIndex = i;
								selected_cube_distance_to_camera = distance_vec3f32(camera.position, cube_get_center(copy));
							} else {
								f32 current_cube_distance = distance_vec3f32(camera.position, cube_get_center(copy));
								if (current_cube_distance < selected_cube_distance_to_camera) {
									SelectedCubeIndex = i;
									selected_cube_distance_to_camera = current_cube_distance;
								}
							}
						}
					}
				}
			}
		}

		// Draw cubes
		{
			for(u32 i = 0; i < TotalCubes; i++) {

				// Draw cubes normally
				cube_program_draw(Cubes[i], view, projection);

				// Draw hovered cube highlight
				if (HoveredCubeIndex == i) {
					// Hover cube
					Cube hover_cube = Cubes[i];
					glLineWidth(3.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					{
						hover_cube.color = vec3f32(sin(8.0f*glfwGetTime() + (2*PI/3)) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (4*PI/3)) * 0.5f + 0.5f, sin(8.0f*glfwGetTime()) * 0.5f + 0.5f);
						cube_scale(&hover_cube, vec3f32(1.1f, 1.1f, 1.1f));
					}
					cube_program_draw(hover_cube, view, projection);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glLineWidth(1.0f);
				}

				// Draw selected cube highlight
				if (SelectedCubeIndex == i) {
					Cube cube = Cubes[i];
					glLineWidth(3.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					{
						cube.color = vec3f32(sin(8.0f*glfwGetTime()) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (2*PI/3)) * 0.5f + 0.5f, sin(8.0f*glfwGetTime() + (4*PI/3)) * 0.5f + 0.5f);
						cube_scale(&cube, vec3f32(1.01f, 1.01f, 1.01f));
					}
					cube_program_draw(cube, view, projection);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glLineWidth(1.0f);
				}
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