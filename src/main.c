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

//////////////////////////////////////////////
// Forward declares
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void process_input(GLFWwindow *window);

int main() {
  
  glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME, NULL, NULL);
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

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(shader);

	while(!glfwWindowShouldClose(window)) {
    process_input(window);

		glClearColor(0.3f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
  
	glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
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
