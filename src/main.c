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

global_variable Vec3f32 CameraPos   = {0.0f, 0.0f,  3.0f};
global_variable Vec3f32 CameraFront = {0.0f, 0.0f, -1.0f};
global_variable Vec3f32 CameraUp    = {0.0f, 1.0f,  0.0f};

global_variable f32 DeltaTime = 0.0f;
global_variable f32 LastFrame = 0.0f;

b32 FirstMouse = 1;
f32 yaw   = -90.0f;
f32 pitch =  0.0f;
f32 lastX =  800.0f / 2.0;
f32 lastY =  6000 / 2.0;
f32 fov   =  45.0f;

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
    
    Mat4f32 view = mat4f32(1.0f); // make sure to initialize matrix to identity matrix first
    f32 radius = 10.0f;
    f32 camX = (f32)(sin(glfwGetTime()) * radius);
    f32 camZ = (f32)(cos(glfwGetTime()) * radius);
    view = mat4f32_look_at(vec3f32(camX, 0.0f, camZ), vec3f32(0.0f, 0.0f, 0.0f), vec3f32(0.0f, 1.0f, 0.0f));
    
    Vec3f32 target = add_vec3f32_vec3f32(CameraPos, CameraFront);
    view = mat4f32_look_at(CameraPos, target, CameraUp);

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

  f32 cameraSpeed = (f32)(10 * DeltaTime);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    Vec3f32 delta = scale_vec3f32(CameraFront, cameraSpeed);
    CameraPos = add_vec3f32_vec3f32(CameraPos, delta);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    Vec3f32 delta = scale_vec3f32(CameraFront, cameraSpeed);
    CameraPos = sub_vec3f32_vec3f32(CameraPos, delta);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    Vec3f32 cross = cross_vec3f32(CameraFront, CameraUp);
    Vec3f32 delta = scale_vec3f32(cross, cameraSpeed);
    CameraPos = sub_vec3f32_vec3f32(CameraPos, delta);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    Vec3f32 cross = cross_vec3f32(CameraFront, CameraUp);
    Vec3f32 delta = scale_vec3f32(cross, cameraSpeed);
    CameraPos = add_vec3f32_vec3f32(CameraPos, delta);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    CameraPos.y -= cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    CameraPos.y += cameraSpeed;
  }

  local_persist GLenum polygon_mode = GL_FILL;
  if (glfwGetKey(window, GLFW_KEY_F1)) {
    polygon_mode = (polygon_mode == GL_FILL) ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
  }
}

void mouse_callback(GLFWwindow* window, f64 xposIn, f64 yposIn) {
    f32 xpos = (f32)(xposIn);
    f32 ypos = (f32)(yposIn);

    if (FirstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        FirstMouse = 0;
    }

    f32 xoffset = xpos - lastX;
    f32 yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    f32 sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    Vec3f32 front;
    front.x = cos(radians_from_degrees(yaw)) * cos(radians_from_degrees(pitch));
    front.y = sin(radians_from_degrees(pitch));
    front.z = sin(radians_from_degrees(yaw)) * cos(radians_from_degrees(pitch));

    CameraFront = normalize_vec3f32(front);
}