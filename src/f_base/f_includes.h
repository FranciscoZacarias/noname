#ifndef F_INCLUDE_H
#define F_INCLUDE_H

//////////////////////////////////////////////
// CLib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

//////////////////////////////////
// Header to include in projects

#include "f_core.h"

#include "f_math.h"
#include "f_os/f_os.h"
#include "f_memory.h"
#include "f_string.h"

#include "f_math.c"
#include "f_os/f_os.c"
#include "f_memory.c"
#include "f_string.c"

#endif // F_INCLUDE_H