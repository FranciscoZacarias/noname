#ifndef MAIN_H
#define MAIN_H

//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//////////////////////////////////////////////
// Windows
#include <windows.h>

//////////////////////////////////////////////
// CLib
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//////////////////////////////////////////////
// FLayer
#include "fdefines.h"

// *.h
#include "fmath.h"
#include "shader.h"
#include "camera.h"

// *.c
#include "fmath.c"
#include "shader.c"
#include "camera.c"

#define APP_NAME "noname"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos);

#endif // MAIN_H
