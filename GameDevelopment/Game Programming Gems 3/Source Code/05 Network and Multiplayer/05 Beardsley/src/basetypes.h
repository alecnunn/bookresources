// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// basetypes.h

// Define base integral types: [u]int{8,16,32,64}
// Define base floating types: float32, float64
// Define MAKE_INT64 and MAKE_UINT64 macros for constants
// Define PRINTF_INT64 and PRINTF_UINT64 macros for printf format

#ifndef INCLUDED_BASETYPES_H
#define INCLUDED_BASETYPES_H

#include "platform.h"

#if defined(OS_LINUX)
# include <stdint.h>
#endif

// gcc linux x86

#if defined(COMPILER_GCC) && defined(OS_LINUX) && defined(CPU_X86)

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## LL)
#define MAKE_UINT64(x) (x ## ULL)

#define PRINTF_INT64 "%lld"
#define PRINTF_UINT64 "%llu"

// gcc NetBSD sparc

#elif defined(COMPILER_GCC) && defined(OS_NETBSD) && defined(CPU_SPARC)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## LL)
#define MAKE_UINT64(x) (x ## ULL)

#define PRINTF_INT64 "%lld"
#define PRINTF_UINT64 "%llu"

// gcc win32 x86

#elif defined(COMPILER_GCC) && defined(OS_WIN32) && defined(CPU_X86)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## LL)
#define MAKE_UINT64(x) (x ## ULL)

#define PRINTF_INT64 "%lld"
#define PRINTF_UINT64 "%llu"

// msvc win32 x86

#elif defined(COMPILER_MSVC) && defined(OS_WIN32) && defined(CPU_X86)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed __int64 int64;
typedef unsigned __int64 uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## I64)
#define MAKE_UINT64(x) (x ## UI64)

#define PRINTF_INT64 "%I64d"
#define PRINTF_UINT64 "%I64u"

#else
# error "Platform not recognized"
#endif

#endif // !INCLUDED_BASETYPES_H
