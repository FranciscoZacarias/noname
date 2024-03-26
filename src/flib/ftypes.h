
#ifndef FTYPES_H
#define FTYPES_H

#define local_persist   static
#define global static
#define function        static

#define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))

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

typedef s8  b8;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

#endif // FTYPES_H