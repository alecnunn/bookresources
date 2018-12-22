//
// Win32 Event class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_WIN32_EVENT_H
#define __PTC_WIN32_EVENT_H

// include files
#include <windows.h>
#include "config.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Win32Event
{
    public:

        // setup
        Win32Event();
        ~Win32Event();

        // control
        void set();
        void reset();
        void wait();

        // data access
        HANDLE handle();

    private:

        // event handle
        HANDLE m_event;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
