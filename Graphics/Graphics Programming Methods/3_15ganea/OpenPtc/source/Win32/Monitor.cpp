//
// Win32 Monitor class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Win32/Monitor.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif

// disable multiple threading
#define __DISABLE_MULTITHREADING__




Win32Monitor::Win32Monitor()
{
#ifndef __DISABLE_MULTITHREADING__

    // initialize critical section
    InitializeCriticalSection(&m_handle);

#endif
}


Win32Monitor::~Win32Monitor()
{
#ifndef __DISABLE_MULTITHREADING__

    // delete critical section
    DeleteCriticalSection(&m_handle);

#endif
}




void Win32Monitor::enter()
{
#ifndef __DISABLE_MULTITHREADING__

    // enter critical section
    EnterCriticalSection(&m_handle);

#endif
}


void Win32Monitor::leave()
{
#ifndef __DISABLE_MULTITHREADING__

    // leave critical section
    LeaveCriticalSection(&m_handle);

#endif
}
