//
// Win32 Cursor class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Log.h"
#include "Win32/Cursor.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




void Win32Cursor::show()
{
    // log showing cursor
    LOG("showing cursor");

    // force cursor show
    while (ShowCursor(1)<0);
}


void Win32Cursor::hide()
{
    // log hiding cursor
    LOG("hiding cursor");

    // force cursor hide
    while (ShowCursor(0)>=0);
}