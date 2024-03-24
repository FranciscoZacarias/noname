#ifndef MAIN_H
#define MAIN_H

//////////////////////////////////////////////
// Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//////////////////////////////////////////////
// Windows
#ifndef _WINDOWS_H
#define _WINDOWS_H
# include <windows.h>
#endif

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

// *.c
#include "shader.c"
#include "camera.c"

#define APP_NAME "noname"

#endif // MAIN_H
