// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// platform.h

// Detect compiler, operating system, architecture.

#ifndef INCLUDED_PLATFORM_H
#define INCLUDED_PLATFORM_H

// Determine compiler: after this, one of the following symbols will
// be defined: COMPILER_GCC, COMPILER_MSVC.

#if defined(__GNUG__)
# define COMPILER_GCC
#elif defined(_MSC_VER)
# define COMPILER_MSVC
// i don't care about your debug symbol issues...
# pragma warning(disable:4786)
#else
# error "Could not determine compiler"
#endif

// Determine OS: after this, one of the following symbols will be
// defined: OS_LINUX, OS_WIN32, OS_NETBSD.

#if defined(COMPILER_GCC) && defined(__linux__)
# define OS_LINUX
#elif defined(COMPILER_GCC) && defined(__NetBSD__)
# define OS_NETBSD
#elif defined(COMPILER_GCC) && defined(__CYGWIN32__)
# define OS_WIN32
#elif defined(COMPILER_MSVC) && defined(_WIN32)
# define OS_WIN32
#else
# error "Could not determine OS"
#endif

// Determine CPU: after this, one of the following symbols will be
// defined: CPU_X86, CPU_SPARC.

#if defined(COMPILER_GCC) && defined(__i386__)
# define CPU_X86
#elif defined(COMPILER_GCC) && defined(__sparc__)
# define CPU_SPARC
#elif defined(COMPILER_MSVC) && defined(_M_IX86)
# define CPU_X86
#else
# error "Could not determine CPU"
#endif

#endif // !INCLUDED_PLATFORM_H
