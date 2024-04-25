
#ifndef FCORE_H
#define FCORE_H

// Start: System ----------------

#if ARCH_64BIT
# define IntFromPtr(ptr) ((U64)(ptr))
#elif ARCH_32BIT
# define IntFromPtr(ptr) ((U32)(ptr))
#else
# error Missing pointer-to-integer cast for this architecture.
#endif
#define PointerFromInt(i) (void*)((U8*)0 + (i))
#define Member(T,m) (((T*)0)->m)

#if COMPILER_MSVC
# define Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
# define Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

// End: System ------------------

// Start: Core -------------------

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

// End: Core ---------------------

#endif // FCORE_H