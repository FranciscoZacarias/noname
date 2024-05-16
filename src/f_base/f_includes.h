#ifndef F_INCLUDES_H
#define F_INCLUDES_H

//////////////////////////////////////////////
// CLib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> // Required for: sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

//////////////////////////////////////////////
// Third-party headers
#include "external/stb_truetype.h"
#include "external/stb_image.h"

//////////////////////////////////
// Headers
#include "f_core.h"
#include "f_math.h"
#include "f_memory.h"
#include "f_string.h"
#include "f_thread_context.h"
#include "f_os/f_os.h"

//////////////////////////////////////////////
// Third-party source
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

//////////////////////////////////
// Headers
#include "f_math.c"
#include "f_memory.c"
#include "f_string.c"
#include "f_thread_context.c"
#include "f_os/f_os.c"

#endif // F_INCLUDES_H