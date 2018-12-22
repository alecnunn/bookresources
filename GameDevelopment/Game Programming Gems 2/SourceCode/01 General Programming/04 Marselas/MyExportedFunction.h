/* Copyright (C) Herb Marselas, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Herb Marselas, 2001"
 */
#ifdef _BUILDING_MY_DLL
#define DLLFUNCTION __declspec(dllexport) // defined if building the DLL
#else
#define DLLFUNCTION __declspec(dllimport) // defined if building the application
#endif

DLLFUNCTION long myExportedFunction(void);
