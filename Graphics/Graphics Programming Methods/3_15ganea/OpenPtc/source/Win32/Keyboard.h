//
// Keyboard class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_WIN32_KEYBOARD_H
#define __PTC_WIN32_KEYBOARD_H

// include files
#include "config.h"
#include "Core/Key.h"
#include "Win32/Hook.h"
#include "Win32/Event.h"
#include "Win32/Window.h"
#include "Win32/Monitor.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Win32Keyboard : public Win32Hook
{
    public:

        // setup
        Win32Keyboard(HWND window,DWORD thread,bool multithreaded);
        ~Win32Keyboard();

        // input
        bool key(Win32Window &window);
        Key read(Win32Window &window);

        // control
        void enable();
        void disable();

    private:

        // window procedure
        virtual LRESULT WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

        // internal key functions
        void insert(const Key &key);
        Key remove();
        bool ready();

        // data
        bool m_key;
        bool m_multithreaded;
        Win32Event m_event;
        Win32Monitor m_monitor;

        // flag data
        bool m_enabled;

        // modifiers
        bool m_alt;
        bool m_shift;
        bool m_control;

        // key buffer
        int m_head;
        int m_tail;
        Key m_buffer[1024];
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
