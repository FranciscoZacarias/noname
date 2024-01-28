/* date = January 28th 2024 11:04 am

Code standard:
 global variables: CamelCase without separations
 local  variables: lower case separated by underscores
 functions: lower case separated by underscores - my_function_example
 struct: CamelCase

Francisco Zacarias
*/

//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glad.c"

//////////////////////////////////////////////
// My includes. NOTE: Order matters

// *.h
#include <stdio.h>
#include "core.h"

// *.c


//////////////////////////////////////////////
// Actual Program... Finally.

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void process_input(GLFWwindow *window);

const char* VertexShaderSource = \
VERTEX_SHADER(layout (location = 0) in vec3 Pos;
              void main() {
                gl_Position = vec4(Pos.x, Pos.y, Pos.z, 1.0);
              });

const char* FragmentShaderSource = \
FRAGMENT_SHADER(out vec4 FragColor;
                void main() {
                  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0);
                });

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
  
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD");
		return -1;
	}
  
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &VertexShaderSource, NULL);
  glCompileShader(vertex_shader);
  
  // TODO: Error checking, extract.
  int  success;
  char infoLog[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
    printf("Error %d compiling vertex shader. Log: %s", success, infoLog);
    return 1;
  }
  // ...
  
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &FragmentShaderSource, NULL);
  glCompileShader(fragment_shader);
  
  // TODO: Error checking, extract.
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
    printf("Error %d compiling fragment shader. Log: %s", success, infoLog);
    return 1;
  }
  // ...
  
  u32 shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  
  // TODO: Error checking, extract
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("Error %d linking shader program. Log: %s", success, infoLog);
    return 0;
  }
  // ...
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  f32 vertices[] = {
     0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
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
  // Because we only have one, this doesn't make sense. But so I dont forget
  // That we need to bind and unbind the array.
  // Also, when a VAO is bound and we bind a EBO, it get's stored in the VAO object,
  // so, next time we bind this VAO, we don't need to bind the EBO, because the VAO knows which one it is
  glBindVertexArray(0);
  
	while(!glfwWindowShouldClose(window)) {
    process_input(window);
    
		glClearColor(0.3f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shader_program);
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