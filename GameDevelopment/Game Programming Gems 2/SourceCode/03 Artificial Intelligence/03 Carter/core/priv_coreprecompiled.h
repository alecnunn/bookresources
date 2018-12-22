#ifndef	PRECOMPILED_H
#define	PRECOMPILED_H

// It is a design goal of this library that the library should
// compile even if all Windows specific header files are removed
// from the precompiled header file. In other words, the code
// should be as free from Windows dependencies as possible, and
// those files that genuinely need windows.h can #include it
// explicitly.
//#include <windows.h>
//#include <windowsx.h>

#include "core.h"

#endif
