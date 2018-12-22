#include "priv_coreprecompiled.h"

#include <windows.h>
#include <signal.h>	// For raise()

/*
The assert support functions in this file were taken from assert.c in the
Microsoft C run time source code and may not be generally distributed.

They enhance the assert functionality in conjunction with an updated
assert define to avoid recursive asserts and to allow you to break
directly into your assert, instead of into assert.c.

If you need to work with these files, start from the last function in
this file.
*/

/***
*__crtMessageBox - call MessageBoxA dynamically.
*
*Purpose:
*	   Avoid static link with user32.dll. Only load it when actually needed.
*
*Entry:
*	   see MessageBoxA docs.
*
*Exit:
*	   see MessageBoxA docs.
*
*Exceptions:
*
*******************************************************************************/
static int __cdecl __crtMessageBoxA(
		LPCSTR lpText,
		LPCSTR lpCaption,
		UINT uType
		)
{
	static int (APIENTRY *pfnMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT) = NULL;
	static HWND (APIENTRY *pfnGetActiveWindow)(void) = NULL;
	static HWND (APIENTRY *pfnGetLastActivePopup)(HWND) = NULL;

	HWND hWndParent = NULL;

	if (NULL == pfnMessageBoxA)
	{
		HINSTANCE hlib = LoadLibrary("user32.dll");

		if (NULL == hlib || NULL == (pfnMessageBoxA =
					(int (APIENTRY *)(HWND, LPCSTR, LPCSTR, UINT))
					GetProcAddress(hlib, "MessageBoxA")))
			return 0;

		pfnGetActiveWindow = (HWND (APIENTRY *)(void))
					GetProcAddress(hlib, "GetActiveWindow");

		pfnGetLastActivePopup = (HWND (APIENTRY *)(HWND))
					GetProcAddress(hlib, "GetLastActivePopup");
	}

	if (pfnGetActiveWindow)
		hWndParent = (*pfnGetActiveWindow)();

	if (hWndParent != NULL && pfnGetLastActivePopup)
		hWndParent = (*pfnGetLastActivePopup)(hWndParent);

	return (*pfnMessageBoxA)(hWndParent, lpText, lpCaption, uType);
}

/*
 * assertion format string for use with output to stderr
 */
static char _assertstring[] = "Assertion failed: %s, file %s, line %d\n";

/*	  Format of MessageBox for assertions:
*
*	   ================= Microsft Visual C++ Debug Library ================
*
*	   Assertion Failed!
*
*	   Program: c:\test\mytest\foo.exe
*	   File: c:\test\mytest\bar.c
*	   Line: 69
*
*	   Expression: <expression>
*
*	   For information on how your program can cause an assertion
*	   failure, see the Visual C++ documentation on asserts
*
*	   (Press Retry to debug the application - JIT must be enabled)
*
*	   ===================================================================
*/

/*
 * assertion string components for message box
 */
#define BOXINTRO	"Assertion failed!"
#define PROGINTRO   "Program: "
#define FILEINTRO   "File: "
#define LINEINTRO   "Line: "
#define EXPRINTRO   "Expression: "
#define INFOINTRO   "For information on how your program can cause an assertion\n" \
					"failure, see the Visual C++ documentation on asserts"
#define HELPINTRO   "(Press Retry to debug the application - JIT must be enabled)"

static char * dotdotdot = "...";
static char * newline = "\n";
static char * dblnewline = "\n\n";

#define DOTDOTDOTSZ 3
#define NEWLINESZ   1
#define DBLNEWLINESZ   2

#define MAXLINELEN  60 /* max length for line in message box */
#define ASSERTBUFSZ (MAXLINELEN * 9) /* 9 lines in message box */

static bool _internalcsassert(const char* expr, const char* filename, unsigned int lineno)
{
	int nCode;
	char * pch;
	char assertbuf[ASSERTBUFSZ];
	char progname[MAX_PATH];

	/*
	 * Line 1: box intro line
	 */
	strcpy( assertbuf, BOXINTRO );
	strcat( assertbuf, dblnewline );

	/*
	 * Line 2: program line
	 */
	strcat( assertbuf, PROGINTRO );

	if ( !GetModuleFileName( NULL, progname, MAX_PATH ))
		strcpy( progname, "<program name unknown>");

	pch = (char *)progname;

	/* sizeof(PROGINTRO) includes the NULL terminator */
	if ( sizeof(PROGINTRO) + strlen(progname) + NEWLINESZ > MAXLINELEN )
	{
		pch += (sizeof(PROGINTRO) + strlen(progname) + NEWLINESZ) - MAXLINELEN;
		strncpy( pch, dotdotdot, DOTDOTDOTSZ );
	}

	strcat( assertbuf, pch );
	strcat( assertbuf, newline );

	/*
	 * Line 3: file line
	 */
	strcat( assertbuf, FILEINTRO );

	/* sizeof(FILEINTRO) includes the NULL terminator */
	if ( sizeof(FILEINTRO) + strlen(filename) + NEWLINESZ >
		 MAXLINELEN )
	{
		/* too long. use only the first part of the filename string */
		strncat( assertbuf, filename, MAXLINELEN - sizeof(FILEINTRO)
				 - DOTDOTDOTSZ - NEWLINESZ );
		/* append trailing "..." */
		strcat( assertbuf, dotdotdot );
	}
	else
		/* plenty of room on the line, just append the filename */
		strcat( assertbuf, filename );

	strcat( assertbuf, newline );

	/*
	 * Line 4: line line
	 */
	strcat( assertbuf, LINEINTRO );
	itoa( lineno, assertbuf + strlen(assertbuf), 10 );
	strcat( assertbuf, dblnewline );

	/*
	 * Line 5: message line
	 */
	strcat( assertbuf, EXPRINTRO );

	/* sizeof(HELPINTRO) includes the NULL terminator */

	if (	strlen(assertbuf) +
			strlen(expr) +
			2*DBLNEWLINESZ +
			sizeof(INFOINTRO)-1 +
			sizeof(HELPINTRO) > ASSERTBUFSZ )
	{
		strncat( assertbuf, expr,
			ASSERTBUFSZ -
			(strlen(assertbuf) +
			DOTDOTDOTSZ +
			2*DBLNEWLINESZ +
			sizeof(INFOINTRO)-1 +
			sizeof(HELPINTRO)) );
		strcat( assertbuf, dotdotdot );
	}
	else
		strcat( assertbuf, expr );

	strcat( assertbuf, dblnewline );

	/*
	 * Line 6, 7: info line
	 */

	strcat(assertbuf, INFOINTRO);
	strcat( assertbuf, dblnewline );

	/*
	 * Line 8: help line
	 */
	strcat(assertbuf, HELPINTRO);

	/*
	 * Write out via MessageBox
	 */

	nCode = __crtMessageBoxA(assertbuf,
		"Microsoft Visual C++ Runtime Library",
		MB_ABORTRETRYIGNORE|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);

	/* Abort: abort the program */
	if (nCode == IDABORT)
	{
		/* raise abort signal */
		raise(SIGABRT);

		/* We usually won't get here, but it's possible that
		   SIGABRT was ignored.  So exit the program anyway. */

		_exit(3);
	}

	/* Retry: call the debugger */
	if (nCode == IDRETRY)
	{
		//_DbgBreak();
		/* return to user code */
		return true;	// Specify that a break is desired.
	}

	/* Ignore: continue execution */
	if (nCode == IDIGNORE)
		return false;

	// Shouldn't ever get here?
	abort();
	return false;
}

bool __cdecl _csassert(const char* expr, const char* filename, unsigned int lineno)
{
	// Recursion detector.
	static bool AlreadyHere = false;
	bool Result = false;
	if (AlreadyHere)
	{
		// Nested asserts happen very easily when working on Windows apps. If you
		// trigger an assert inside of a timing message you will probably never
		// see it - especially if you are single stepping through the code,
		dprintf("Nested assert - this is very bad!!!\n");
		DEBUG_BREAK();
		return Result;
	}
	AlreadyHere = true;
	// Call the real assert function.
	Result = _internalcsassert(expr, filename, lineno);
	AlreadyHere = false;
	return Result;
}
