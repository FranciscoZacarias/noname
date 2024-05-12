#ifndef MAIN_H
#define MAIN_H

#define ENABLE_CULL 1
#define ENABLE_ASSERT 1

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

#include "hotloadable.h"

// *.c
#include "cube.c"
#include "utils.c"
#include "renderer.c"
#include "camera.c"

#include "hotloadable.c"

#define APP_NAME "noname"

#endif // MAIN_H
