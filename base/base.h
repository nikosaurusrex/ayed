#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "base_platform.h"

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;

typedef int8_t   B8;
typedef int32_t  B32;

#define global static
#define intern static

// TODO fix
#define read_only const
#if 0
#if COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)
#pragma section(".rdata$", read)
#define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
#define read_only __attribute__((section(".rodata")))
#else
#define read_only const
#endif
#endif

#if COMPILER_MSVC
#define thread_local __declspec(thread)
#elif COMPILER_CLANG
#define thread_local __thread
#elif COMPILER_GCC
#define thread_local __thread
#endif

#if COMPILER_MSVC
#define align_by(n) __declspec(align(n))
#elif COMPILER_CLANG
#define align_by(n) __attribute((aligned(n)))
#elif COMPILER_GCC
#define align_by(n) __attribute((aligned(n)))
#endif

#define KILO_BYTES(n) (((U64)(n)) << 10)
#define MEGA_BYTES(n) (((U64)(n)) << 20)
#define GIGA_BYTES(n) (((U64)(n)) << 30)
#define TERA_BYTES(n) (((U64)(n)) << 40)

#define MIN(A, B)      (((A) < (B)) ? (A) : (B))
#define MAX(A, B)      (((A) > (B)) ? (A) : (B))
#define CLAMP_TOP(A, X) MIN(A, X)
#define CLAMP_BOT(X, B) MAX(X, B)
#define CLAMP(A, X, B)  (((X) < (A)) ? (A) : ((X) > (B)) ? (B) : (X))

#define MEM_COPY(dst, src, size) memcpy((dst), (src), (size))
#define MEM_MOVE(dst, src, size) memmove((dst), (src), (size))
#define MEM_SET(dst, byte, size) memset((dst), (byte), (size))
#define MEM_CMP(a, b, size)      memcmp((a), (b), (size))
#define MEM_ZERO(s, z)           memset((s), 0, (z))

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#if COMPILER_CLANG || COMPILER_GCC
#define EXPECT(expr, val) __builtin_expect((expr), (val))
#else
#define EXPECT(expr, val) (expr)
#endif

#define NK_LIKELY(expr)   EXPECT(expr, 1)
#define NK_UNLIKELY(expr) EXPECT(expr, 0)

#if COMPILER_MSVC
#define NK_TRAP() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
#define NK_TRAP() __builtin_trap()
#else
#error Unsupported Compiler.
#endif

#define NK_STRING_JOIN(arg1, arg2)             NK_STRING_JOIN_DELAY(arg1, arg2)
#define NK_STRING_JOIN_DELAY(arg1, arg2)       NK_STRING_JOIN_IMMEDIATE(arg1, arg2)
#define NK_STRING_JOIN_IMMEDIATE(arg1, arg2)   arg1 ## arg2

#ifdef COMPILER_MSVC
#define NK_LINENUMBER(name) NK_STRING_JOIN(name,__COUNTER__)
#else
#define NK_LINENUMBER(name) NK_STRING_JOIN(name,__LINE__)
#endif

#define STATIC_ASSERT(cond) typedef char NK_LINENUMBER(_static_assert_var) [ (cond) ? 1 : -1 ]

#define if_likely(cond) if (NK_LIKELY(cond))
#define if_unlikely(cond) if (NK_UNLIKELY(cond))

#if BUILD_DEBUG
#define ASSERT(x)                                                                                        \
   do {                                                                                                  \
      if (!(x)) {                                                                                        \
         NK_TRAP();                                                                                      \
      }                                                                                                  \
   } while (0)
#else
#define ASSERT(x) ((void)0)
#endif

#if COMPILER_MSVC
#define NKINLINE __forceinline
#elif COMPILER_CLANG || COMPILER_GCC
#define NKINLINE __attribute__((always_inline))
#else
#define NKINLINE
#endif

#define NOT_IMPLEMENTED ASSERT("Not Implemented!")

#define ALIGN_POW2(x, b) (((x) + (b)-1) & (~((b)-1)))
#define IS_POW2(x)      ((x) != 0 && ((x) & ((x)-1)) == 0)

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#define ASAN_POISON_MEMORY_REGION(addr, size) \
  __asan_poison_memory_region((addr), (size))
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) \
  __asan_unpoison_memory_region((addr), (size))
#else
#define ASAN_POISON_MEMORY_REGION(addr, size) \
  ((void)(addr), (void)(size))
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) \
  ((void)(addr), (void)(size))
#endif 

read_only global float pi32 = 3.1415926535897f;

read_only global U64 max_U64 = 0xffffffffffffffffull;
read_only global U32 max_U32 = 0xffffffff;
read_only global U16 max_U16 = 0xffff;
read_only global U8  max_U8  = 0xff;

read_only global S64 max_S64 = (S64)0x7fffffffffffffffull;
read_only global S32 max_S32 = (S32)0x7fffffff;
read_only global S16 max_S16 = (S16)0x7fff;
read_only global S8  max_S8  = (S8)0x7f;

read_only global S64 min_S64 = (S64)0xffffffffffffffffull;
read_only global S32 min_S32 = (S32)0xffffffff;
read_only global S16 min_S16 = (S16)0xffff;
read_only global S8  min_S8  = (S8)0xff;

static void log_fatal(const char *fmt, ...);
static void log_error(const char *fmt, ...);
static void log_dev(const char *fmt, ...);
static void log_info(const char *fmt, ...);
