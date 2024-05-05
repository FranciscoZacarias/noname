
/*
  Project specific utils.
*/

#ifndef UTILS_H
#define UTILS_H

#define COLOR_WHITE vec4f32(1.0f, 1.0f, 1.0f)
#define COLOR_BLACK vec4f32(0.0f, 0.0f, 0.0f)

#define COLOR_RED    vec4f32(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN  vec4f32(0.0f, 1.0f, 0.0f)
#define COLOR_BLUE   vec4f32(0.0f, 0.0f, 1.0f)
#define COLOR_YELLOW vec4f32(1.0f, 1.0f, 0.0f)

// Palletes
#if 1
# define PALLETE_COLOR_A vec4f32(0.894f, 0.772f, 0.620f)
# define PALLETE_COLOR_B vec4f32(0.686f, 0.510f, 0.376f)
# define PALLETE_COLOR_C vec4f32(0.502f, 0.239f, 0.231f)
# define PALLETE_COLOR_D vec4f32(0.196f, 0.176f, 0.169f)
#else
# define PALLETE_COLOR_A vec4f32(0.278f, 0.576f, 0.686f)
# define PALLETE_COLOR_B vec4f32(1.0f, 0.769f, 0.439f)
# define PALLETE_COLOR_C vec4f32(0.867f, 0.341f, 0.275f)
# define PALLETE_COLOR_D vec4f32(0.545f, 0.196f, 0.173f)
#endif

#endif // UTILS_H