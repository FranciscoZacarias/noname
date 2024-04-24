
#ifndef FCORE_H
#define FCORE_H

// Start: Helper ----------------

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

#if OS_WINDOWS
# include <windows.h>
# include <tmmintrin.h>
# include <wmmintrin.h>
# include <intrin.h>
# if ARCH_X64
#  define ins_atomic_u64_eval(x) InterlockedAdd64((volatile __int64 *)(x), 0)
#  define ins_atomic_u64_inc_eval(x) InterlockedIncrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_dec_eval(x) InterlockedDecrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_eval_assign(x,c) InterlockedExchange64((volatile __int64 *)(x),(c))
#  define ins_atomic_u64_add_eval(x,c) InterlockedAdd64((volatile __int64 *)(x), c)
#  define ins_atomic_u64_eval_cond_assign(x,k,c) InterlockedCompareExchange64((volatile __int64 *)(x),(k),(c))
#  define ins_atomic_u32_eval(x,c) InterlockedAdd((volatile LONG *)(x), 0)
#  define ins_atomic_u32_eval_assign(x,c) InterlockedExchange((volatile LONG *)(x),(c))
#  define ins_atomic_u32_eval_cond_assign(x,k,c) InterlockedCompareExchange((volatile LONG *)(x),(k),(c))
#  define ins_atomic_ptr_eval_assign(x,c) (void*)ins_atomic_u64_eval_assign((volatile __int64 *)(x), (__int64)(c))
# else
#  error Atomic intrinsics not defined for this operating system / architecture combination.
# endif
#elif OS_LINUX
# if ARCH_X64
#  define ins_atomic_u64_inc_eval(x) __sync_fetch_and_add((volatile U64 *)(x), 1)
# else
#  error Atomic intrinsics not defined for this operating system / architecture combination.
# endif
#else
# error Atomic intrinsics not defined for this operating system.
#endif

// End: Helper ------------------

// Start: Core -------------------

#define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))

#define OffsetOf(T,m) IntFromPointer(&Member(T,m))

#define Kilobytes(n) (((u64)(n)) << 10)
#define Megabytes(n) (((u64)(n)) << 20)
#define Gigabytes(n) (((u64)(n)) << 30)
#define Terabytes(n) (((u64)(n)) << 40)

#define Thousand(n) ((n)*1000)
#define Million(n)  ((n)*1000000)
#define Billion(n)  ((n)*1000000000)

#include <string.h>
#define MemoryCopy(dst, src, size) memmove((dst), (src), (size))
#define MemorySet(dst, byte, size) memset((dst), (byte), (size))

// End: Core ---------------------

#endif // FCORE_H