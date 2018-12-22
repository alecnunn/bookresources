//
// Win32 Event class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Error.h"
#include "Win32/Event.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




Win32Event::Win32Event()
{
    // create event handle
    m_event = CreateEvent(0,TRUE,FALSE,0);

    // check event handle
    if (!m_event) throw Error("could not create event");
}


Win32Event::~Win32Event()
{
    // close handle
    CloseHandle(m_event);
}


void Win32Event::set()
{
    // set event
    SetEvent(m_event);
}


void Win32Event::reset()
{
    // reset event
    ResetEvent(m_event);
}


void Win32Event::wait()
{
    // wait for event
    WaitForSingleObject(m_event,INFINITE);
}


HANDLE Win32Event::handle()
{
    // get handle
    return m_event;
}
