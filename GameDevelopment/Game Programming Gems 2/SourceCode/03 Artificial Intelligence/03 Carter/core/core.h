#ifndef	INC_CORE_H
#define	INC_CORE_H

/*
This header defines a useful set of defines, types, pragmas and
includes that I like to always have available. It should very rarely
be changed, and should be included from everywhere.
*/


// Disable the STUPID!!! STL in use warning.
#pragma warning(disable : 4786)

// Some generally useful header files that I want always available.
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifdef	_MSC_VER
// This disgusting macro forces VisualC++ to use the ANSI C++ rules
// for the scoping of variables declared inside for statements.
// Trust me - it works. Using this evil macro makes it much easier
// to write portable code.
	#define	for if (0) ; else for
	#define	DEBUG_BREAK()	__asm { int 3 }
#else
	#error Unknown compiler - I can't promise that this code will work.
	#define	DEBUG_BREAK() DebugBreak()
#endif

// Cool compile time assert. Use for things like:
// CompileTimeAssert(sizeof(BITMAPINFOHEADER) == sizeof(PortableBITMAPINFOHEADER));
// From Writing Bug-Free C Code for Windows, Jerry Jongerius, Prentice Hall, 1995
#define CompileTimeAssert(exp) extern char _CompileTimeAssert[(exp)?1:-1]

// Asserts are good. However Microsoft's implementation is not always good.
// It can very easily cause infinitely recursive asserts, because when the
// assert dialog appears it runs a message pump, which can cause messages
// to be delivered to your window - which can cause your assert to trigger
// again.
// Recursion: n. see recursion.
// Also, if you click 'Debug' on one of Microsoft's assert you get dropped
// into the assert.c source file, instead of into your own source code.
// This assert macro fixes these problems.
// Define this if you don't want the fixed assert dialogs.
//#define	USEMICROSOFTASSERTDIALOG
#ifndef	_MSC_VER
	// The Borland compiler gives lots of warnings if you redefine the assert
	// macro - I haven't explored how to avoid them so I'll avoid redefining it.
	#define	USEMICROSOFTASSERTDIALOG
#endif
#ifdef	USEMICROSOFTASSERTDIALOG
	#include <assert.h>
#else
	#undef	assert	// Make sure we've wiped out any previous definitions.
	#ifdef  NDEBUG
		// In release mode, asserts do nothing.
		#define assert(exp)     ((void)0)
	#else
		// In debug mode we need an assert macro and an assert function.
		// _csassert() returns true if the user requests debugging.
		bool _csassert(const char* expr, const char* filename, unsigned int lineno);
		#define assert(exp) \
				do { \
					if (!(exp)) { \
						dprintf("%s(%d): Assertion failed\n%s\n", __FILE__, __LINE__, #exp); \
						if (_csassert(#exp, __FILE__, __LINE__)) \
							DEBUG_BREAK(); \
						} \
				} while (0)
	#endif
#endif

// I love this macro for finding the the number of elements in an array.
// What can I say - I know the preprocessor is evil, but it works so
// damned well for a few key things!
#define	NUMELEMENTS(x)	(sizeof(x) / sizeof(x[0]))

// Some generally useful functions that I want to have always available.

// The 'r' versions of these functions continue working in
// release builds. The versions without the r disappear in
// release builds.
// Send formatted debugging information to the debugger output
// window and DBWin32.
void rdprintf(const char* format, ...);
void rdprintv(const char *format, va_list ap);
#if defined(_DEBUG) || defined(_PCLINT)
	#define	dprintf rdprintf
	#define	dprintv rdprintv
#else
	// Magic to make these disappear in release builds.
	#define dprintf 0 && 
	#define dprintv(x, y)
#endif
// Lower level function that just displays a string. Maximum
// size is about 500 characters.
void OutputDebugStringW95( const char* achBuffer);

// Are we running under a debugger? Works on Win95 also.
bool UniversalIsDebuggerPresent();

// Get the 'time' in seconds, with unspecified precision that should be at least
// one millisecond. The start time is unspecified.
double GetSeconds();

// Record detailed information about a crash, to give you a hope
// of being able to diagnose it.
struct _EXCEPTION_POINTERS;
typedef _EXCEPTION_POINTERS* PEXCEPTION_POINTERS;
int HandleException(PEXCEPTION_POINTERS data, const char *Message);


// Everybody has types used for specified sized ints (even though
// you should mostly just use int), however everybody spells them
// differently. int8, INT8, Int8, etc. The spelling below is the
// CORRECT spelling. Don't believe me? Check the spec for the new
// version of C.
typedef	signed char int8_t;
typedef	unsigned char uint8_t;
typedef	signed short int16_t;
typedef	unsigned short uint16_t;
typedef	signed int int32_t;
typedef	unsigned int uint32_t;

#endif	// INC_CORE_H
