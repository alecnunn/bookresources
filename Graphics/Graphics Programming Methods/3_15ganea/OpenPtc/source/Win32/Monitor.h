//
// Win32 Monitor class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_WIN32_MONITOR_H
#define __PTC_WIN32_MONITOR_H

// include files
#include <windows.h>
#include "config.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Win32Monitor
{
    public:

        // setup
        Win32Monitor();
        ~Win32Monitor();

        // control
        void enter();
        void leave();

    private:

        // critical section
        CRITICAL_SECTION m_handle;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
