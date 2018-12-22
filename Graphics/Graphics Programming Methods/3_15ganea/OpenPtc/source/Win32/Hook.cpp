//
// Win32 Hook class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Error.h"
#include "Win32/Hook.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif

// static member data
int Win32Hook::m_count = 0;
Win32Monitor Win32Hook::m_monitor;
Win32Hook::Lookup* Win32Hook::m_cached = 0;
Win32Hook::Lookup Win32Hook::m_registry[16];




Win32Hook::Win32Hook(HWND window,DWORD thread)
{
    // setup data
    m_window = window;
    m_thread = thread;

    // add to registry
    add(m_window,m_thread);
}


Win32Hook::~Win32Hook()
{
    // remove from registry
    remove(m_window,m_thread);
}




void Win32Hook::add(HWND window,DWORD thread)
{
    // enter monitor
    m_monitor.enter();

    // invalidate cache
    m_cached = 0;

    // registry index
    int index = 0;

    // iterate registry
    while (index<m_count)
    {
        // search for existing window hook
        if (m_registry[index].window == window)
        {
            // match
            break;
        }

        // next
        index++;
    }

    // check results
    if (index!=m_count)
    {
        // get insertion point for hook
        const int insert = m_registry[index].count;
       
        // increase hook count
        m_registry[index].count++;

#ifdef __DEBUG__

        // check for maximum hook count
        if (m_registry[index].count>sizeof m_registry[index].hook) throw Error("Win32Hook too many hooks created!");

#endif

        // insert hook in registry
        m_registry[index].hook[insert] = this;
    }
    else
    {
        // setup new lookup
        m_registry[index].wndproc = GetWindowLong(window,GWL_WNDPROC);
        m_registry[index].window = window;
        m_registry[index].hook[0] = this;
        m_registry[index].count = 1;

        // increase lookup count
        m_count ++;

#ifdef __DEBUG__

        // check for maximum count
        if (m_count>sizeof m_registry) throw Error("Win32Hook too many lookups created!");

#endif

        // set window procedure to hook procedure
        SetWindowLong(window,GWL_WNDPROC,(LONG)hook);
    }

    // leave monitor
    m_monitor.leave();
}


void Win32Hook::remove(HWND window,DWORD thread)
{
    // enter monitor
    m_monitor.enter();

    // invalidate cache
    m_cached = 0;

    // registry index
    int index = 0;

    // iterate registry
    while (index<m_count)
    {
        // check for window match
        if (m_registry[index].window==window)
        {
            // search for this
            for (int i=0; i<m_registry[index].count; i++)
            {
                // check hook
                if (m_registry[index].hook[i]==this)
                {
                    // remove this hook (quite inefficient for high count...)
                    for (int j=i; j<m_registry[index].count-1; j++) m_registry[index].hook[j] = m_registry[index].hook[j+1];

                    // decrease hook count
                    m_registry[index].count--;

                    // break
                    break;
                }
            }

            // check remaining hook count
            if (m_registry[index].count==0)
            {
                // restore original window procedure
                SetWindowLong(window,GWL_WNDPROC,m_registry[i].wndproc);
                
                // remove this lookup (quite inefficient for high count...)
                for (int i=index; i<m_count-1; i++) m_registry[i] = m_registry[i+1];

                // decrease count
                m_count--;
            }

            // break
            break;
        }

        // next 
        index++; 
    }

    // leave monitor
    m_monitor.leave();
}




LRESULT CALLBACK Win32Hook::hook(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    // enter monitor
    m_monitor.enter();

    // lookup pointer
    Lookup *lookup = 0;

    // check cached lookup if valid
    if (m_cached && m_cached->window==hwnd)
    {
        // cached lookup match
        lookup = m_cached;
    }
    else
    {
        // search for matching window
        for (int i=0; i<m_count; i++)
        {
            // check for lookup window match
            if (m_registry[i].window==hwnd)
            {
                // setup cached lookup
                m_cached = &m_registry[i];

                // setup lookup
                lookup = m_cached;

                // break
                break;
            }
        }

#ifdef __DEBUG__

        // check for search failure
        if (i==m_count || lookup==0) throw Error("Win32Hook window lookup search failure!");

#endif
    }

    // result value
    LRESULT result = 0;

    // iterate all hooks for this window
    for (int i=lookup->count-1; i>=0; i--)
    {
        // call hook window procedure
        result = lookup->hook[i]->WndProc(hwnd,msg,wParam,lParam);

        // check result value ?
        //if (result==TRUE) break;
    }
    
    // check result ?
    //if (result!=TRUE)
    {
        /*
        // setup window procedure type definition
        typedef LRESULT (CALLBACK *PROCEDURE)(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

        // cast the wndproc to a window procedure
        const PROCEDURE procedure = (PROCEDURE)lookup->wndproc;

        // call original window procudure
        return procedure(hwnd,msg,wParam,lParam);
        */

        // call original window procudure
        result = CallWindowProc((WNDPROC)lookup->wndproc,hwnd,msg,wParam,lParam);
    }

    // leave monitor
    m_monitor.leave();

    // return result
    return result;
}   
