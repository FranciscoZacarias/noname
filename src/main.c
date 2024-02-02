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

#include "main.h"   // Keep on top
#include "fmath.h"
#include "shader.h"

// *.c
#include "fmath.c"
#include "shader.c"

//////////////////////////////////////////////
// Vertex Shader
const char* GET_VERTEX_SHADER() {
  
  return SHADER_SOURCE(//////////////////////////////////////////////
                       // Vertex Shader start
                       
                       layout (location = 0) in vec3 aPos;
                       uniform mat4 transform;
                       
                       void main() {
                         gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
                       }
                       
                       // Vertex Shader end
                       //////////////////////////////////////////////
                       );
}

//////////////////////////////////////////////
// Fragment Shader
const char* GET_FRAGMENT_SHADER() {
  
  return SHADER_SOURCE(//////////////////////////////////////////////
                       // Fragment  Shader start
                       
                       out vec4 FragColor;
                       
                       void main() {
                         FragColor = vec4(0.8f, 1.0f, 0.8f, 1.0);
                       }
                       
                       // Fragment Shader End
                       //////////////////////////////////////////////
                       );
}

//////////////////////////////////////////////
// Forward declares
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void process_input(GLFWwindow *window);

int main() {
  
  glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(800, 600, APP_NAME, NULL, NULL);
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
  
  Shader shader;
  shader_create(&shader, GET_VERTEX_SHADER(), GET_FRAGMENT_SHADER());
  
  f32 vertices[] = {
     0.25f,  0.25f, 0.0f,
     0.25f, -0.25f, 0.0f,
    -0.25f, -0.25f, 0.0f,
    -0.25f,  0.25f, 0.0f
  };
  
  u32 indices[] = {
    0, 1, 3,
    1, 2, 3
  };
  
  u32 VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  
  u32 VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  u32 EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3*sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  
	while(!glfwWindowShouldClose(window)) {
    process_input(window);
    
		glClearColor(0.3f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
    
    Mat4 transform = mat4_make_identity();
    transform = mat4_translate(transform, vec4_new(0.5f, -0.5f, 0.0f));
    transform = mat4_rotate(transform, vec4_new(0.0f, 0.0f, 1.0f), (f32)glfwGetTime());
    transform = mat4_scale(transform, vec4_new(1.2f, 1.2f, 1.0f));
    
    shader_use(shader);
    shader_set_uniform_mat4fv(shader, "transform", transform);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
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
  local_persist GLenum polygon_mode = GL_FILL;
  
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	} else if (glfwGetKey(window, GLFW_KEY_F1)) {
    polygon_mode = (polygon_mode == GL_FILL) ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
  }
}
