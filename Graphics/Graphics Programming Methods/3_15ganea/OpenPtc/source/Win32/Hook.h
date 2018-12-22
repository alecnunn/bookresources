//
// Win32 Hook class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_WIN32_HOOK_H
#define __PTC_WIN32_HOOK_H

// include files
#include <windows.h>
#include "config.h"
#include "Win32/Monitor.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Win32Hook
{
    public:

        // setup
        Win32Hook(HWND window,DWORD thread);
        ~Win32Hook();

    protected:

        // window procedure
        virtual LRESULT WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) = 0;

    private:

        // hook management
        void add(HWND hWnd,DWORD thread);
        void remove(HWND hWnd,DWORD thread);
        
        // window message hook
        static LRESULT CALLBACK hook(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

        // lookup structure
        struct Lookup
        {
            HWND window;
            LONG wndproc;
            Win32Hook *hook[16];
            int count;
        };
        
        // hook data
        HWND m_window;
        DWORD m_thread;

        // static data
        static HHOOK m_hook;
        static int m_count;
        static Lookup* m_cached;
        static Lookup m_registry[16];
        static Win32Monitor m_monitor;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
