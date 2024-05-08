#ifndef F_INCLUDES_H
#define F_INCLUDES_H

//////////////////////////////////////////////
// CLib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

//////////////////////////////////
// Header to include in projects

#define F_CORE_HEADER
# include "f_core.h"

#define F_MATH_HEADER
# include "f_math.h"
#define F_OS_HEADER
# include "f_os/f_os.h"
#define F_MEMORY_HEADER
# include "f_memory.h"
#define F_STRING_HEADER
# include "f_string.h"

#define F_MATH_IMPLEMENTATION
# include "f_math.h"
#define F_OS_IMPLEMENTATION
# include "f_os/f_os.h"
#define F_MEMORY_IMPLEMENTATION
# include "f_memory.h"
#define F_STRING_IMPLEMENTATION
# include "f_string.h"

#endif // F_INCLUDES_H