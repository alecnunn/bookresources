//
// DirectX Hook class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Log.h"
#include "Core/Error.h"
#include "Win32/Cursor.h"
#include "DirectX/Hook.h"
#include "DirectX/Display.h"
#include "DirectX/Console.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




DirectXHook::DirectXHook(DirectXConsole &console,HWND window,DWORD thread,bool cursor,bool managed,bool fullscreen) : Win32Hook(window,thread)
{
    // log window hook create
    LOG("creating window hook");

    // setup console
    m_console = &console;
    
    // setup data
    m_cursor = cursor;
    m_managed = managed;
    m_fullscreen = fullscreen;
}


DirectXHook::~DirectXHook()
{
    // log window hook destroy
    LOG("destroying window hook");
}




void DirectXHook::cursor(bool flag)
{
    // set cursor flag
    m_cursor = flag;
}

    


LRESULT DirectXHook::WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch (message)
    {
        case WM_PAINT:
        {
            // log window message
            LOG("DirectXHook WM_PAINT");

            // paint console
            m_console->paint();
        }
        break;

        case WM_ACTIVATEAPP:
        {
            // log window message
            LOG("DirectXHook WM_ACTIVATEAPP");

            // get window message data
            BOOL active = (BOOL) wParam;
            DWORD thread = (DWORD) lParam;
            
            // check active flag
            if (active==FALSE)
            {
                // deactivate
                deactivate();

                // check cursor and fullscreen
                if (!m_cursor && m_fullscreen)
                {
                    // show cursor
                    Win32Cursor::show();
                }
            }
            else
            {
                // check cursor and fullscreen
                if (!m_cursor && m_fullscreen)
                {
                    // window placement data
                    WINDOWPLACEMENT placement;
                    
                    // get window placement for activate app
                    if (GetWindowPlacement(hWnd,&placement)==0)
                    {
                        // on failure set to normal show
                        placement.showCmd = SW_SHOWNORMAL;
                    }

                    // check show command is not minimize
                    if (placement.showCmd!=SW_SHOWMINIMIZED)
                    {
                        // hide cursor
                        Win32Cursor::hide();
                    }
                }

                // activate
                activate();
            }

            // handled
            return TRUE;
        }
        break;

        case WM_SETCURSOR:
        {
            // check cursor
            if (!m_cursor)
            {
                // hide cursor
                SetCursor(0);
            }

            // handled
            return TRUE;
        }
        break;

        case WM_CLOSE:
        {
            // log window message
            LOG("DirectXHook WM_CLOSE");

            // check managed
            if (m_managed)
            {
                // get a reference to the console
                DirectXConsole &console = *m_console;

                // close console
                console.close();

                // note: at this point the hook object has been deleted by the console!

                // internal console shutdown
                console.internal_shutdown();

                // exit
                exit(0);
            }

            // handled
            return TRUE;
        }
        break;
    }

    // unhandled
    return FALSE;
}




void DirectXHook::activate()
{
    // check if open
    if (m_console->m_open)
    {
        // log activate
        LOG("activate");

        // get console object references
        DirectXDisplay &display = m_console->m_display;
        DirectXPrimary &primary = m_console->m_primary;

        // check if primary is not active
        if (!primary.active())
        {
            // save display
            display.save();

            // activate primary
            primary.activate();
        }
    }
}


void DirectXHook::deactivate()
{
    // check if open
    if (m_console->m_open)
    {
        // log deactivate
        LOG("deactivate");

        // get console object references
        DirectXDisplay &display = m_console->m_display;
        DirectXPrimary &primary = m_console->m_primary;

        // check if primary is active
        if (primary.active())
        {
            // save primary
            primary.save();

            // deactivate primary
            primary.deactivate();

            // restore display
            display.restore();
        }
    }
}
