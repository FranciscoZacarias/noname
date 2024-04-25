
#ifndef FCORE_H
#define FCORE_H

////////////////////////////////
// Core

#if COMPILER_MSVC
# define Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
# define Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

#define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))

#define IntFromPtr(p) (U64)((U8*)p - (U8*)0)
#define PtrFromInt(i) (void*)((U8*)0 + (i))
#define Member(T,m) (((T*)0)->m)
#define OffsetOfMember(T,m) IntFromPtr(&Member(T,m))

#define Kilobytes(n) (((u64)(n)) << 10)
#define Megabytes(n) (((u64)(n)) << 20)
#define Gigabytes(n) (((u64)(n)) << 30)
#define Terabytes(n) (((u64)(n)) << 40)

#define Thousand(n) ((n)*1000)
#define Million(n)  ((n)*1000000llu)
#define Billion(n)  ((n)*1000000000llu)
#define Trillion(n) ((n)*1000000000000llu)

#define DEFAULT_ALIGNMENT sizeof(void*)
#define AlignUpPow2(x,p)  (((x) + (p) - 1)&~((p) - 1))
#define AlignDownPow2(x,p) ((x)&~((p) - 1))

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define StaticAssert(c,l) typedef U8 Glue(l,__LINE__) [(c)?1:-1]

#include <string.h>
#define MemoryCopy(d,s,z)     memmove((d), (s), (z))
#define MemoryCopyStruct(d,s) MemoryCopy((d),(s), Min(sizeof(*(d)) , sizeof(*(s))))
#define MemoryZero(d,z)       memset((d), 0, (z))
#define MemoryZeroStruct(d,T) MemoryZero((d),sizeof(T))
#define MemoryMatch(a,b,z)   (memcmp((a),(b),(z)) == 0)

#define local_persist static
#define global        static
#define function      static

////////////////////////////////
// Types 

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

#endif // FCORE_H