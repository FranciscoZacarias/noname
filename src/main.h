#ifndef MAIN_H
#define MAIN_H

//~ noname
#define APP_NAME "noname"
#define ENABLE_HOTLOAD_VARIABLES 1
#define ENABLE_ASSERT 1

//~ Colors
#define Color_White vec4f32(1.0f, 1.0f, 1.0f, 1.0f)
#define Color_Black vec4f32(0.0f, 0.0f, 0.0f, 1.0f)

#define Color_Red    vec4f32(1.0f, 0.0f, 0.0f, 1.0f)
#define Color_Green  vec4f32(0.0f, 1.0f, 0.0f, 1.0f)
#define Color_Blue   vec4f32(0.0f, 0.0f, 1.0f, 1.0f)
#define Color_Yellow vec4f32(1.0f, 1.0f, 0.0f, 1.0f)
#define Color_Purple vec4f32(1.0f, 0.0f, 1.0f, 1.0f)

#if 1
# define PALLETE_COLOR_A vec4f32(0.894f, 0.772f, 0.620f, 1.0f)
# define PALLETE_COLOR_B vec4f32(0.686f, 0.510f, 0.376f, 1.0f)
# define PALLETE_COLOR_C vec4f32(0.502f, 0.239f, 0.231f, 1.0f)
# define PALLETE_COLOR_D vec4f32(0.196f, 0.176f, 0.169f, 1.0f)
#else
# define PALLETE_COLOR_A vec4f32(0.278f, 0.576f, 0.686f, 1.0f)
# define PALLETE_COLOR_B vec4f32(1.0f, 0.769f, 0.439f, 1.0f)
# define PALLETE_COLOR_C vec4f32(0.867f, 0.341f, 0.275f, 1.0f)
# define PALLETE_COLOR_D vec4f32(0.545f, 0.196f, 0.173f, 1.0f)
#endif

//~ Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

GLFWwindow* GlfwWindow;
GLFWcursor* ArrowCursor;
GLFWcursor* DragCursor;

//~ FLayer
#include "f_includes.h"

//~ Third-party headers
#include "external/stb_truetype.h"
#include "external/stb_image.h"

//~ *.h
#include "input.h"
#include "camera.h"
#include "core.h"
#include "game.h" // NOTE(fz): The renderer will make assumptions on types that exist in game.h. Keep this on top
#include "renderer.h"
#include "hotloadable.h"

//~ Third-party source
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

//~ *.c
#include "input.c"
#include "camera.c"
#include "core.c"
#include "game.c"
#include "renderer.c"
#include "hotloadable.c"

#endif // MAIN_H
