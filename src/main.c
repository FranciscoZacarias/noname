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
// Forward declares
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void process_input(GLFWwindow *window);

Vec4 cameraPos   = {0.0f, 0.0f,  3.0f, 1.0f};
Vec4 cameraFront = {0.0f, 0.0f,  -3.0f, 1.0f};
Vec4 cameraUp    = {0.0f, 1.0f,  0.0f, 1.0f};

float delta_time = 0.0f;// Time between current frame and last frame
float last_frame = 0.0f; // Time of last frame

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
  
  f32 vertices[] = {
    // Front
     0.5f,  0.5f, 0.5f,
     0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f,  0.5f, 0.5f,
    // Back
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
  };
  
  u32 indices[] = {
    // Front
    0, 1, 3,
    1, 2, 3,
    // Back
    4, 5, 7, 
    5, 6, 7,
    // Right
    0, 4, 5,
    0, 1, 5,
    // Left
    2, 6, 7,
    2, 3, 7,
    // Top
    0, 3, 4,
    3, 4, 7,
    // Bot
    1, 2, 5,
    2, 5, 6
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
  
  Vec4 cubes[10] = {
    vec4_make( 0.0f,  0.0f,  0.0f),
    vec4_make( 2.0f,  5.0f, -15.0f),
    vec4_make(-1.5f, -2.2f, -2.5f),
    vec4_make(-3.8f, -2.0f, -12.3f),
    vec4_make( 2.4f, -0.4f, -3.5f),
    vec4_make(-1.7f,  3.0f, -7.5f),
    vec4_make( 1.3f, -2.0f, -2.5f),
    vec4_make( 1.5f,  2.0f, -2.5f),
    vec4_make( 1.5f,  0.2f, -1.5f),
    vec4_make(-1.3f,  1.0f, -1.5f)
  };

  Vec4 colors[10] = {
    vec4_make(0.0f, 0.0f, 0.0f),
    vec4_make(1.0f, 0.5f, 0.0f),
    vec4_make(0.0f, 0.5f, 1.0f),
    vec4_make(0.5f, 0.0f, 1.0f),
    vec4_make(1.0f, 0.0f, 0.5f),
    vec4_make(0.5f, 1.0f, 0.0f),
    vec4_make(0.0f, 1.0f, 0.5f),
    vec4_make(0.5f, 0.5f, 0.5f),
    vec4_make(1.0f, 1.0f, 1.0f),
    vec4_make(0.0f, 0.0f, 1.0f)
  };

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_use(shader);

	while(!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;  

    process_input(window);

		glClearColor(0.3f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Vec4 target = add_vec4_vec4(cameraPos, cameraFront);
    Mat4 view   = mat4_look_at(cameraPos, target, cameraUp);
    
    shader_set_uniform_mat4fv(shader, "view", view);

    Mat4 projection = mat4_make_perspective(radians(45), (f32)WINDOW_WIDTH/(f32)WINDOW_HEIGHT, 0.1f, 100.0f);
    shader_set_uniform_mat4fv(shader, "projection", projection);

    glBindVertexArray(VAO);
    for(u32 i = 0; i < 10; i++) {
      Mat4 model    = mat4_make_translate(cubes[i]);
      shader_set_uniform_mat4fv(shader, "model", model);
      shader_set_uniform_vec4fv(shader, "in_color", colors[i]);
      glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
    }

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

  const float cameraSpeed = 2.5f * delta_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    Vec4 delta = vec4_scale(cameraFront, cameraSpeed);
    cameraPos  = add_vec4_vec4(cameraPos, delta);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    Vec4 delta = vec4_scale(cameraFront, cameraSpeed);
    cameraPos  = sub_vec4_vec4(cameraPos, delta);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    Vec4 x = cross(cameraFront, cameraUp);
    Vec4 delta = vec4_scale(x, cameraSpeed);
    cameraPos = add_vec4_vec4(cameraPos, delta);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    Vec4 x = cross(cameraFront, cameraUp);
    Vec4 delta = vec4_scale(x, cameraSpeed);
    cameraPos = sub_vec4_vec4(cameraPos, delta);
  }
}
