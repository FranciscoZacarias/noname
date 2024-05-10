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

#define F_CORE_HEADER
# include "f_core.h"
#define F_MATH_HEADER
# include "f_math.h"
#define F_STRING_HEADER
# include "f_string.h"
#define F_OS_HEADER
# include "f_os/f_os.h"
#define F_MEMORY_HEADER
# include "f_memory.h"

//////////////////////////////////////////////
// Third-party source
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

//////////////////////////////////
// Headers

#define F_MATH_IMPLEMENTATION
# include "f_math.h"
#define F_STRING_IMPLEMENTATION
# include "f_string.h"
#define F_OS_IMPLEMENTATION
# include "f_os/f_os.h"
#define F_MEMORY_IMPLEMENTATION
# include "f_memory.h"

#endif // F_INCLUDES_H