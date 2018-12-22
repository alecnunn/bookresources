/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */


// VisualSysD3D8Dll.h

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VISUALSYSD3D8DLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VISUALSYSD3D8DLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef VISUALSYSD3D8DLL_EXPORTS
#define VISUALSYSD3D8DLL_API __declspec(dllexport)
#else
#define VISUALSYSD3D8DLL_API __declspec(dllimport)
#endif

class VisualSys_t;

extern "C"
{
	VISUALSYSD3D8DLL_API VisualSys_t *CreateDllVisualSys();
	VISUALSYSD3D8DLL_API void DeleteDllVisualSys( VisualSys_t *pVisualSys );
}
