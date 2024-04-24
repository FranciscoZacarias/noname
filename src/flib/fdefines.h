
#ifndef FDEFINES_H
#define FDEFINES_H

// Start: Defines -------------------

#define local_persist static
#define global        static
#define function      static

#define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))
#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))

// End: Defines ---------------------

// Start: Types ---------------------

#define null ((void*)0)

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
#define U8_MAX  0xFF
#define U8_MIN  0x00
#define U16_MAX 0xFFFF
#define U16_MIN 0x0000
#define U32_MAX 0xFFFFFFFFu
#define U32_MIN 0x00000000u
#define U64_MAX 0xFFFFFFFFFFFFFFFFull
#define U64_MIN 0x0000000000000000ull

typedef signed char      s8;
typedef signed short     s16;
typedef signed int       s32;
typedef signed long long s64;
#define S8_MAX  0x7F
#define S8_MIN  (-S8_MAX - 1)
#define S16_MAX 0x7FFF
#define S16_MIN (-S16_MAX - 1)
#define S32_MAX 0x7FFFFFFF
#define S32_MIN (-S32_MAX - 1)
#define S64_MAX 0x7FFFFFFFFFFFFFFFll
#define S64_MIN (-S64_MAX - 1ll)

typedef float  f32;
typedef double f64;
#define F32_MAX 0x1.fffffep+127f
#define F32_MIN (-F32_MAX)
#define F64_MAX 0x1.fffffffffffffp+1023
#define F64_MIN (-F64_MAX)

typedef s32 b32;
#define true  ((b32)1)
#define false ((b32)0)

// End: Types -----------------------

#endif // FDEFINES_H