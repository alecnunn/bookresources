/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef _WIN32_H_
#define _WIN32_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <strstrea.h>
#include <fstream.h>
#include <time.h>
#include <assert.h>
#include <gl/gl.h>
#include <gl/glu.h>

void WinInit(void);
void WinSwapBuffers(void);
void WinDestroy(void);

typedef struct WinKey_Str {
	unsigned int down[64];
	int downCnt;
	unsigned int up[64];
	int upCnt;
} WinKey;
extern WinKey winKey;

// Output Window Handle
extern HANDLE outputWinHandle;
extern HANDLE inputWinHandle;

//#ifdef __cplusplus
//}
//#endif

#endif
