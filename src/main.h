/* date = January 28th 2024 11:04 am */

#ifndef TYPES_H
#define TYPES_H

#define APP_NAME "noname"

#define VERTEX_SHADER(code)   "#version 330 core\n" #code
#define FRAGMENT_SHADER(code) "#version 330 core\n" #code

#define global_variable static
#define local_persist   static

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

typedef float  f32;
typedef double f64;

typedef s32 b32;
typedef s8  b8;

#endif //TYPES_H
