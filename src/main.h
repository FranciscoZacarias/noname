#ifndef MAIN_H
#define MAIN_H

//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//////////////////////////////////////////////
// CLib
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////////////////////
// FLayer
#include "finclude.h"

// *.h
#include "shader.h"
#include "camera.h"
#include "cube.h"

// *.c
#include "shader_source.c"
#include "shader.c"
#include "camera.c"
#include "cube.c"

#define APP_NAME "noname"

#endif // MAIN_H
