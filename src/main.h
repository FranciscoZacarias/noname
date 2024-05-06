#ifndef MAIN_H
#define MAIN_H

//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//////////////////////////////////////////////
// FLayer
#include "f_includes.h"

// *.h
#include "cube.h"
#include "utils.h"
#include "renderer.h"
#include "camera.h"

// *.c
#include "cube.c"
#include "utils.c"
#include "renderer.c"
#include "camera.c"

#define ENABLE_ASSERT 1

#define APP_NAME "noname"

#endif // MAIN_H
