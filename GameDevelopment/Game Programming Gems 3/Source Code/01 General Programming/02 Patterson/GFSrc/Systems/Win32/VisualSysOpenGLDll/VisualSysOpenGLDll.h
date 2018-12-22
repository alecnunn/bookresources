/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSysOpenGLDll.h

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VISUALSYSOPENGLDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VISUALSYSOPENGLDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef VISUALSYSOPENGLDLL_EXPORTS
#define VISUALSYSOPENGLDLL_API __declspec(dllexport)
#else
#define VISUALSYSOPENGLDLL_API __declspec(dllimport)
#endif

class VisualSys_t;

extern "C"
{
	VISUALSYSOPENGLDLL_API VisualSys_t *CreateDllVisualSys();
	VISUALSYSOPENGLDLL_API void DeleteDllVisualSys( VisualSys_t *pVisualSys );
}
