// assertx.h - simple assertion functionality.
//
// By Paul Glinker
//

#ifndef ASSERTX_H
#define ASSERTX_H


#ifdef NDEBUG


#define ASSERT( exp )


#else // !def NDEBUG


#ifdef _WINDOWS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

inline void assert_windows(const char *szExp, const char *szFilename, int iLineNum)
{
	char szMsg[128];
	
	sprintf(szMsg, "Expr: (%s)\nFile: %s\nLine: %d\n", szExp, szFilename, iLineNum);

	MessageBox(NULL,szMsg,"ASSERTION FAILED!", (MB_OK|MB_ICONERROR));

	exit(1);
}

#define ASSERT( exp )	if ( !(exp) ) assert_windows( #exp, __FILE__, __LINE__ )


#else // !def _WINDOWS


#include <stdio.h>
#include <stdlib.h>

inline void assert_generic(const char *szExp, const char *szFilename, int iLineNum)
{
	printf("================================================\n");
	printf("ASSERTION FAILED!\nExpr: (%s)\nFile: %s\nLine: %d\n", szExp, szFilename, iLineNum);
	printf("================================================\n");

	exit(1);
}

#define ASSERT( exp )	if ( !(exp) ) assert_generic( #exp, __FILE__, __LINE__ )


#endif //_WINDOWS

#endif // NDEBUG


#endif // ASSERTX_H

