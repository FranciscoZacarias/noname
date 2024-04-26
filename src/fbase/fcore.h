
#ifndef FCORE_H
#define FCORE_H

////////////////////////////////
// Context Cracking

// Clang OS/Arch Cracking
#if defined(__clang__)

# define COMPILER_CLANG 1
# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
# else
#  error This compiler/OS combo is not supported.
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define ARCH_X86 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# elif defined(__arm__)
#  define ARCH_ARM32 1
# else
#  error Architecture not supported.
# endif

// MSVC OS/Arch Cracking
#elif defined(_MSC_VER)

# define COMPILER_MSVC 1
# if _MSC_VER >= 1930
#  define COMPILER_MSVC_YEAR 2022
# elif _MSC_VER >= 1920
#  define COMPILER_MSVC_YEAR 2019
# elif _MSC_VER >= 1910
#  define COMPILER_MSVC_YEAR 2017
# elif _MSC_VER >= 1900
#  define COMPILER_MSVC_YEAR 2015
# elif _MSC_VER >= 1800
#  define COMPILER_MSVC_YEAR 2013
# elif _MSC_VER >= 1700
#  define COMPILER_MSVC_YEAR 2012
# elif _MSC_VER >= 1600
#  define COMPILER_MSVC_YEAR 2010
# elif _MSC_VER >= 1500
#  define COMPILER_MSVC_YEAR 2008
# elif _MSC_VER >= 1400
#  define COMPILER_MSVC_YEAR 2005
# else
#  define COMPILER_MSVC_YEAR 0
# endif

# if defined(_WIN32)
#  define OS_WINDOWS 1
# else
#  error This compiler/OS combo is not supported.
# endif

# if defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_IX86)
#  define ARCH_X86 1
# elif defined(_M_ARM64)
#  define ARCH_ARM64 1
# elif defined(_M_ARM)
#  define ARCH_ARM32 1
# else
#  error Architecture not supported.
# endif

// GCC OS/Arch Cracking
#elif defined(__GNUC__) || defined(__GNUG__)

# define COMPILER_GCC 1
# if defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
# else
#  error This compiler/OS combo is not supported.
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define ARCH_X86 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# elif defined(__arm__)
#  define ARCH_ARM32 1
# else
#  error Architecture not supported.
# endif

#else
# error Compiler not supported.
#endif

// Arch Cracking
#if defined(ARCH_X64)
# define ARCH_64BIT 1
#elif defined(ARCH_X86)
# define ARCH_32BIT 1
#endif

#if ARCH_ARM32 || ARCH_ARM64 || ARCH_X64 || ARCH_X86
# define ARCH_LITTLE_ENDIAN 1
#else
# error Endianness of this architecture not understood by context cracker.
#endif

// Zero All Undefined Options
#if !defined(ARCH_32BIT)
# define ARCH_32BIT 0
#endif
#if !defined(ARCH_64BIT)
# define ARCH_64BIT 0
#endif
#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif
#if !defined(ARCH_ARM32)
# define ARCH_ARM32 0
#endif
#if !defined(COMPILER_MSVC)
# define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif
#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(OS_MAC)
# define OS_MAC 0
#endif

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

#endif // FCORE_H