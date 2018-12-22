//
// Win32 Window class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_WIN32_WINDOW_H
#define __PTC_WIN32_WINDOW_H

// include files
#include <windows.h>
#include "config.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Win32Window
{
    public:

        // setup
        Win32Window(HWND window);
        Win32Window(const char wndclass[],const char title[],unsigned extra,unsigned style,int show,int x,int y,int width,int height,bool center,bool multithreaded,void *data=0);
        ~Win32Window();

        // cursor  management
        void cursor(bool flag);

        // resize window
        void resize(int width,int height);

        // update window
        void update(bool force = false);

        // data access
        HWND handle() const;
        DWORD thread() const;
        bool managed() const;
        bool multithreaded() const;

    private:

        // internals
        void defaults();
        void close();

        // window internals
        static void WINAPI ThreadFunction(Win32Window *owner);
        static LRESULT CALLBACK WndProcSingleThreaded(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
        static LRESULT CALLBACK WndProcMultiThreaded(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

        // window data
        HWND m_window;
        HANDLE m_event;
        HANDLE m_thread;
        DWORD m_id;

        // window creation data
        char m_name[1024];    // window class name
        char m_title[1024];   // window title
        unsigned m_extra;     // window style extra
        unsigned m_style;     // window style
        int m_show;           // window show command
        int m_x;              // window x
        int m_y;              // window y
        int m_width;          // window width
        int m_height;         // window height
        void *m_data;         // creation data

        // managed flag
        bool m_managed;

        // multithread flag
        bool m_multithreaded;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
