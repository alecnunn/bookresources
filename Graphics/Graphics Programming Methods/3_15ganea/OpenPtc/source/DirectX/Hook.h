//
// DirectX Hook class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DIRECTX_HOOK_H
#define __PTC_DIRECTX_HOOK_H

// include files
#include "config.h"
#include "Win32/Hook.h"
#include "DirectX/Index.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DirectXHook : public Win32Hook
{
    public:

        // setup
        DirectXHook(DirectXConsole &console,HWND window,DWORD thread,bool cursor,bool managed,bool fullscreen);
        ~DirectXHook();

        // cursor management
        void cursor(bool flag);

    protected:

        // window procedure
        virtual LRESULT WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

        // window management
        void activate();
        void deactivate();

    private:

        // console
        bool m_cursor;
        bool m_managed;
        bool m_fullscreen;
        DirectXConsole *m_console;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
