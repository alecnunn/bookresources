//
// Win32 Window class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <string.h>
#include <process.h>
#include "Core/Log.h"
#include "Core/Types.h"
#include "Core/Error.h"
#include "Win32/Window.h"
#include "Win32/Cursor.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




Win32Window::Win32Window(HWND window)
{
    // log user managed window attach
    LOG("attaching to user managed window");

    // defaults
    defaults();

    // setup window
    m_window = window;

    // clear managed flag
    m_managed = false;
}


Win32Window::Win32Window(const char wndclass[],const char title[],unsigned extra,unsigned style,int show,int x,int y,int width,int height,bool center,bool multithreaded,void *data)
{
    // log managed window create
    LOG("creating managed window");

    // defaults
    defaults();

    // setup multithreaded flag
    m_multithreaded = multithreaded;

    try
    {
        // get module instances
        HINSTANCE program_instance = GetModuleHandle(0);
        HINSTANCE library_instance = GetModuleHandle("ptc.dll");
        if (!library_instance) library_instance = GetModuleHandle("ptcdebug.dll");
        if (!library_instance) library_instance = program_instance;

        // register window class
        WNDCLASSEX wc;
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.hInstance     = program_instance;
        wc.lpszClassName = wndclass;
        wc.style         = CS_VREDRAW|CS_HREDRAW;
        wc.hIcon         = LoadIcon(library_instance,"IDI_PTC_ICON");
        wc.hIconSm       = 0;
        wc.lpszMenuName  = 0;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hbrBackground = 0;//(HBRUSH) GetStockObject(BLACK_BRUSH);
        if (multithreaded) wc.lpfnWndProc = (WNDPROC)WndProcMultiThreaded;
        else wc.lpfnWndProc = (WNDPROC)WndProcSingleThreaded;
        wc.hCursor = LoadCursor(0,IDC_ARROW);
        RegisterClassEx(&wc);

        // adjust window rectangle for desired client area
        RECT rectangle = { 0,0,width,height };
        AdjustWindowRectEx(&rectangle,style,FALSE,extra);

        // check centering
        if (center)
        {
            // log window centering
            LOG("centering window");

            // get width and height of display
            const int display_width  = GetSystemMetrics(SM_CXSCREEN);
            const int display_height = GetSystemMetrics(SM_CYSCREEN);

            // setup centered window rectangle
            x = (display_width-(rectangle.right-rectangle.left)) / 2;
            y = (display_height-(rectangle.bottom-rectangle.top)) / 2;
        }

        // setup window data
        strcpy(m_name,wndclass);
        strcpy(m_title,title);
        m_extra  = extra;
        m_style  = style;
        m_show   = show;
        m_x      = x;
        m_y      = y;
        m_width  = rectangle.right-rectangle.left;
        m_height = rectangle.bottom-rectangle.top;
        m_data   = data;

        // check multithread flag
        if (multithreaded)
        {
            // error message
            throw Error("multithread window support is disabled");

            /*
            // create window event
            m_event = CreateEvent(0,0,0,0);

            // create window thread
            m_thread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)ThreadFunction,this,0,&m_id);

            // wait for window signal
            WaitForSingleObject(m_event,INFINITE);

            // check window handle
            if (!IsWindow(m_window)) throw Error("could not create window");
            */
        }
        else
        {
            // create window
            m_window = CreateWindowEx(m_extra,m_name,m_title,m_style,m_x,m_y,m_width,m_height,0,0,0,m_data);
    
            // check window handle
            if (!IsWindow(m_window)) throw Error("could not create window");

            // show window
            ShowWindow(m_window,m_show);

            // set window focus
            SetFocus(m_window);

            // set active window
            SetActiveWindow(m_window);

            // set foreground window
            SetForegroundWindow(m_window);
        }
    }
    catch (Error &error)
    {
        // close
        close();

        // error message
        throw Error("could not create window",error);
    }
}


Win32Window::~Win32Window()
{
    // close
    close();
}




void Win32Window::cursor(bool flag)
{   
    // check
    if (flag)
    {
        // set window cursor to arrow
        SetClassLong(m_window,GCL_HCURSOR,(LONG)LoadCursor(0,IDC_ARROW));

        // show cursor
        Win32Cursor::show();
    }
    else
    {
        // set window cursor to null
        SetClassLong(m_window,GCL_HCURSOR,0);

        // hide cursor
        Win32Cursor::hide();
    }

    // send set cursor message
    SendMessage(m_window,WM_SETCURSOR,0,0);
}




void Win32Window::resize(int width,int height)
{
    // get current window rectangle
    RECT window_rectangle;
    GetWindowRect(m_window,&window_rectangle);

    // adjust window rectangle for desired client area
    RECT rectangle = { 0,0,width,height };
    AdjustWindowRectEx(&rectangle,m_style,FALSE,m_extra);

    // resize window
    SetWindowPos(m_window,HWND_TOP,window_rectangle.left,window_rectangle.top,rectangle.right-rectangle.left,rectangle.bottom-rectangle.top,0);
    
    // todo: detect if the window is resized off the screen, and let windows reposition it correctly... ?
}




void Win32Window::update(bool force)
{
    // check managed window
    if (!m_managed && !force) return;

    // check multithreaded flag
    if (!m_multithreaded)
    {   
        // message
        MSG message;
        
        // process all messages
        while (PeekMessage(&message,m_window,0,0,PM_REMOVE))
        {
            // translate and dispatch
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    else
    {
        // sleep
        Sleep(0);
    }
}




HWND Win32Window::handle() const
{
    // get handle
    return m_window;
}


DWORD Win32Window::thread() const
{
    // get thread id
    if (m_multithreaded) return m_id;
    else return GetCurrentThreadId();
}


bool Win32Window::managed() const
{
    // get managed flag
    return m_managed;
}


bool Win32Window::multithreaded() const
{
    // get multithreaded flag
    return m_multithreaded;
}




void Win32Window::defaults()
{
    // defaults
    m_window = 0;
    m_event = 0;
    m_thread = 0;
    m_id = 0;
    m_name[0] = 0;
    m_title[0] = 0;
    m_extra = 0;
    m_style = 0;
    m_show = 0;
    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;
    m_data = 0;
    m_managed = true;
    m_multithreaded = false;
}


void Win32Window::close()
{
    // check user managed
    if (!m_managed)
    {
        // log user managed window detach
        LOG("detaching from user managed window");

        // clear data
        m_window = 0;
    }
    else
    {
        // log managed window close
        LOG("closing managed window");

        // check multithread flag
        if (m_multithreaded)
        {
            // check window and thread
            if (m_thread && IsWindow(m_window))
            {
                // close window
                PostMessage(m_window,WM_DESTROY,0,0);
            
                // wait for window thread to exit
                WaitForSingleObject(m_thread,INFINITE);
            }

            // release objects
            if (m_event)  CloseHandle(m_event);
            if (m_thread) CloseHandle(m_thread);

            // set normal priority class
            SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
        }
        else
        {
            // check window handle is still valid
            if (m_window && IsWindow(m_window))
            {
                // destroy window
                DestroyWindow(m_window);
            }
        }

        // reset data
        m_window = 0;
        m_event = 0;
        m_thread = 0;
        m_id = 0;

        // unregister window class
        UnregisterClass(m_name,GetModuleHandle(0));
    }
}




void WINAPI Win32Window::ThreadFunction(Win32Window *owner)
{
    // create window
    owner->m_window = CreateWindowEx(owner->m_extra,owner->m_name,owner->m_title,owner->m_style,owner->m_x,owner->m_y,owner->m_width,owner->m_height,0,0,0,owner->m_data);
    
    // check window handle
    if (IsWindow(owner->m_window))
    {
        // show window
        ShowWindow(owner->m_window,owner->m_show);

        // set window focus
        SetFocus(owner->m_window);

        // set foreground window
        SetForegroundWindow(owner->m_window);

        // signal window event
        SetEvent(owner->m_event);

        // message loop
        MSG message;
        while (GetMessage(&message,0,0,0)==TRUE)
        {
            // process window message
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    else
    {
        // window failure
        SetEvent(owner->m_event);
    }
}


LRESULT CALLBACK Win32Window::WndProcSingleThreaded(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    // result data
    int result = 0;

    // handle message
    switch (message)
    {
        case WM_CLOSE:
        {
            // log window message
            LOG("Win32Window WM_CLOSE");

            // exit
            exit(0);
        }

        default:
        {
            // unhandled messages
            result = DefWindowProc(hWnd,message,wParam,lParam);
        }
    }

    // return value
    return result;
}


LRESULT CALLBACK Win32Window::WndProcMultiThreaded(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    // result data
    int result = 0;

    // handle message
    switch (message)
    {
        case WM_DESTROY:
        {
            // log window message
            LOG("Win32Window WM_DESTROY");

            // shutdown window
            PostQuitMessage(0);
        }
        break;

        case WM_CLOSE:
        {
            // log window message
            LOG("Win32Window WM_CLOSE");

            // exit
            exit(0);
        }

        default:
        {
            // unhandled messages
            result = DefWindowProc(hWnd,message,wParam,lParam);
        }
    }

    // return value
    return result;
}
