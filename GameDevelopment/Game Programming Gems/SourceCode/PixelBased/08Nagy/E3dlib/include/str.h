/* Copyright (C) Gabor Nagy, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gabor Nagy, 2000"
 */
/*==============================================================================*/
/* Real-time cast shadows							*/
/*										*/
/* - String library include file						*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-Feb-22 16:56:45						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#include <math.h>
#include <stdlib.h>

#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
extern "C" {
#endif

// Define boolean flags
//
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifdef WIN32
#define EStrNLCR		"\012\015"
#define EStrSLASH		'\\'
#define EStrSLASH_STRING	"\\"
#else
#define EStrNLCR		"\n"
#define EStrSLASH		'/'
#define EStrSLASH_STRING	"/"
#endif


extern void*	EMalloc(size_t LSize);
extern void*	ERealloc(void* LBlock,size_t LSize);
extern void	EFree(void* LBlock);
extern int	EStr_StringsEqual(register char* LString0, register char* LString1);
extern char*	EStr_GetFileName(char* LFullStr,char* LRet,int LMaxLen);
extern char*	EStr_GetPathName(char* LFullStr,char* LRet,int LMaxLen);

#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
}	// Close scope of 'extern "C"' declaration which encloses file.
#endif
