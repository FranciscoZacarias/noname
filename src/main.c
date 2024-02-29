//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

// Only thing that I want to allow that is not written by me. For obv reasons.
#include <windows.h>

#include <stdlib.h> // TODO: REMOVE THIS
#include <stdio.h>  // TODO: REMOVE THIS
#include <math.h>   // TODO: REMOVE THIS. THIS IS BORDERLINE HEARSAY

// Headers only
#include "fdefines.h"
#include "main.h"

// *.h
#include "fmath.h"
#include "shader.h"
#include "camera.h"

// *.c
#include "fmath.c"
#include "shader.c"
#include "camera.c"

global_variable s32 window_width  = 800;
global_variable s32 window_height = 600;
#define aspect_ratio() (window_width/window_height)

global_variable Camera camera;
global_variable f32 lastX;
global_variable f32 lastY;
global_variable b32 FirstMouse = 1;

global_variable f32 DeltaTime = 0.0f;
global_variable f32 LastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

int main() {
  
  glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, APP_NAME, NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD");
		return -1;
	}

  glEnable(GL_DEPTH_TEST);  

  camera = camera_create();
  lastX  = window_width / 2.0f;
  lastY  = window_height / 2.0f;

  Shader shader = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());	

  f32 vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
  };

	Vec3f32 cubePositions[] = {
    vec3f32( 0.0f,  0.0f,  0.0f), 
    vec3f32( 2.0f,  5.0f, -15.0f), 
    vec3f32(-1.5f, -2.2f, -2.5f),  
    vec3f32(-3.8f, -2.0f, -12.3f),  
    vec3f32( 2.4f, -0.4f, -3.5f),  
    vec3f32(-1.7f,  3.0f, -7.5f),  
    vec3f32( 1.3f, -2.0f, -2.5f),  
    vec3f32( 1.5f,  2.0f, -2.5f), 
    vec3f32( 1.5f,  0.2f, -1.5f), 
    vec3f32(-1.3f,  1.0f, -1.5f)  
	};

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(f32), (void*)0);
  glEnableVertexAttribArray(0);

	shader_use(shader);
	
	static b32 toggle = 1;
	while(!glfwWindowShouldClose(window)) {
    f32 currentFrame = (f32)(glfwGetTime());
    DeltaTime = currentFrame - LastFrame;
    LastFrame = currentFrame;

    process_input(window);

		glClearColor(0.3f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Mat4f32 view = mat4f32(1.0f);
    
    Vec3f32 target = add_vec3f32_vec3f32(camera.position, camera.front);
    view = mat4f32_look_at(camera.position, target, camera.up);

    shader_set_uniform_mat4fv(shader, "view", view);

		Mat4f32 projection = mat4f32(1.0f);
		projection = mat4f32_perspective(45.0f, aspect_ratio(), 0.1f, 100.0f);
		shader_set_uniform_mat4fv(shader, "projection", projection);

		Mat4f32 model = mat4f32(1.0f);

		glBindVertexArray(VAO);
		for(u32 i = 0; i < 10; i++)
		{
			Mat4f32 model = mat4f32(1.0f);
			model = mat4f32_translate(model, cubePositions[i].x, cubePositions[i].y, cubePositions[i].z);
			f32 angle = 20.0f * i; 
			model = mat4f32_rotate(model, 1.0f, 0.3f, 0.5f, (f32)glfwGetTime() * angle);
	
			shader_set_uniform_mat4fv(shader, "model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
  
	glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	window_width  = width;
	window_height = height;
}

void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
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
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    camera_keyboard_callback(&camera, CameraMovement_Down, DeltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera_keyboard_callback(&camera, CameraMovement_Up, DeltaTime);
  }

  local_persist GLenum polygon_mode = GL_FILL;
  if (glfwGetKey(window, GLFW_KEY_F1)) {
    polygon_mode = (polygon_mode == GL_FILL) ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
  }
}

void mouse_callback(GLFWwindow* window, f64 xposIn, f64 yposIn) {
  f32 xpos = (f32)xposIn;
  f32 ypos = (f32)yposIn;

  if (FirstMouse) {
    lastX = xpos;
    lastY = ypos;
    FirstMouse = 0;
  }

  f32 xoffset = xpos - lastX;
  f32 yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  camera_mouse_callback(&camera, xoffset, yoffset);
}