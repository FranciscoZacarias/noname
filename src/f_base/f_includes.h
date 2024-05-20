#ifndef F_INCLUDES_H
#define F_INCLUDES_H

//~ CLib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

//~ Extern
#include "external/stb_sprintf.h"

//~ Headers
#include "f_core.h"
#include "f_math.h"
#include "f_memory.h"
#include "f_string.h"
#include "f_thread_context.h"
#include "f_os/f_os.h"

//~ Extern
#define STB_SPRINTF_IMPLEMENTATION
#include "external/stb_sprintf.h"

//~ Source
#include "f_math.c"
#include "f_memory.c"
#include "f_string.c"
#include "f_thread_context.c"
#include "f_os/f_os.c"

#endif // F_INCLUDES_H