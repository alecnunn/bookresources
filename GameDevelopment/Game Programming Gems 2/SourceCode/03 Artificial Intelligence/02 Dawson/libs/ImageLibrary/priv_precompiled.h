/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	PRECOMPILED_H
#define	PRECOMPILED_H

// It is a design goal of this library that the library should
// compile even if all Windows specific header files are removed
// from the precompiled header file. In other words, the code
// should be as free from Windows dependencies as possible, and
// those files that genuinely need windows.h can #include it
// explicitly.
#define	WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <windowsx.h>

#include "core/core.h"

// Pull in some STL stuff, so that we can use it without huge
// compile costs.
#include <string>
#include <vector>

#endif
