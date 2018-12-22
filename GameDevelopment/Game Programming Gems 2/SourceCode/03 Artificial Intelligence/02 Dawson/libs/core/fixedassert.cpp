#include "priv_coreprecompiled.h"

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <signal.h>	// For raise()

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

/*
These functions are supposed to enhance the assert
functionality in conjunction with an updated
assert define to avoid recursive asserts and to allow you to break
directly into your assert, instead of into assert.c.

If you need to work with these files, start from the last function in
this file.
*/

/*
 * assertion string components for message box
 */
#define BOXINTRO	"Assertion failed!"
#define PROGINTRO   "Program: "
#define FILEINTRO   "File: "
#define LINEINTRO   "Line: "
#define EXPRINTRO   "Expression: "
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
			sizeof(HELPINTRO) > ASSERTBUFSZ )
	{
		strncat( assertbuf, expr,
			ASSERTBUFSZ -
			(strlen(assertbuf) +
			DOTDOTDOTSZ +
			2*DBLNEWLINESZ +
			sizeof(HELPINTRO)) );
		strcat( assertbuf, dotdotdot );
	}
	else
		strcat( assertbuf, expr );

	strcat( assertbuf, dblnewline );

	strcat(assertbuf, HELPINTRO);

	HWND hWndParent = 0;
	hWndParent = GetActiveWindow();

	if (hWndParent)
		hWndParent = GetLastActivePopup(hWndParent);

	nCode = MessageBoxA(hWndParent, assertbuf,
		"Microsoft Visual C++ Runtime Library",
		MB_ABORTRETRYIGNORE|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);

	if (nCode == IDABORT)
		ExitProcess(10);	// Exit with extreme prejudice - I don't want no leak reports!

	if (nCode == IDRETRY)
		return true;	// Specify that a break is desired.

	// Otherwise it must be the ignore case
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
