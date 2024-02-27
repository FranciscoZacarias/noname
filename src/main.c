/* date = January 28th 2024 11:04 am */

/*
TODO:
- Implement sin, cos, tan, sqrt
*/

//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//////////////////////////////////////////////
// My includes. NOTE: Order matters

// Only thing that I want to allow that is not written by me. For obv reasons.
#include <windows.h>

// *.h
#include <stdlib.h> // TODO: REMOVE THIS
#include <stdio.h>  // TODO: REMOVE THIS
#include <math.h>   // TODO: REMOVE THIS. THIS IS BORDERLINE HEARSAY

#include "defines.h"
#include "main.h"
#include "fmath.h"
#include "shader.h"

// *.c
#include "fmath.c"
#include "shader.c"

global_variable s32 window_width  = 800;
global_variable s32 window_height = 600;
#define aspect_ratio() (window_width/window_height)

Vec3f32 cameraPos   = {0.0f, 0.0f,  3.0f};
Vec3f32 cameraFront = {0.0f, 0.0f, -1.0f};
Vec3f32 cameraUp    = {0.0f, 1.0f,  0.0f};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void process_input(GLFWwindow *window);

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
  
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD");
		return -1;
	}

  glEnable(GL_DEPTH_TEST);  

  Shader shader = shader_create(GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());	

float vertices[] = {
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_False, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	shader_use(shader);
	
	static b32 toggle = 1;
	while(!glfwWindowShouldClose(window)) {
    process_input(window);

		glClearColor(0.3f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4f32 view = mat4f32(1.0f);
		view = mat4f32_make_translate(0.0f, 0.0f, -3.0f);
		shader_set_uniform_mat4fv(shader, "view", view);
		Mat4f32 projection = mat4f32(1.0f);
		projection = mat4f32_perspective(45.0f, aspect_ratio(), 0.1f, 100.0f);
		shader_set_uniform_mat4fv(shader, "projection", projection);

		Mat4f32 model = mat4f32(1.0f);

		glBindVertexArray(VAO);
		for(unsigned int i = 0; i < 10; i++)
		{
			Mat4f32 model = mat4f32(1.0f);
			model = mat4f32_translate(model, cubePositions[i].x, cubePositions[i].y, cubePositions[i].z);
			float angle = 20.0f * i; 
			model = mat4f32_rotate(model, 1.0f, 0.3f, 0.5f, angle);
	
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

  local_persist GLenum polygon_mode = GL_FILL;
  if (glfwGetKey(window, GLFW_KEY_F1)) {
    polygon_mode = (polygon_mode == GL_FILL) ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
  }
}