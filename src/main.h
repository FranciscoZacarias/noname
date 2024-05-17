#ifndef MAIN_H
#define MAIN_H

#define ENABLE_HOTLOAD_VARIABLES 1
#define ENABLE_ASSERT 1

//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//////////////////////////////////////////////
// FLayer
#include "f_includes.h"

global s32 WindowWidth  = 1280;
global s32 WindowHeight = 720;

//////////////////////////////////////////////
// Third-party headers
#include "external/stb_truetype.h"
#include "external/stb_image.h"

// *.h
#include "cube.h"
#include "utils.h"
#include "renderer.h"
#include "camera.h"
#include "hotloadable.h"

//////////////////////////////////////////////
// Third-party source
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"


// *.c
#include "cube.c"
#include "utils.c"
#include "renderer.c"
#include "camera.c"
#include "hotloadable.c"

#define APP_NAME "noname"

#endif // MAIN_H
