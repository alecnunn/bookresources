//
// Win32 Keyboard class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Error.h"
#include "Win32/Keyboard.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




Win32Keyboard::Win32Keyboard(HWND window,DWORD thread,bool multithreaded) : Win32Hook(window,thread)
{
    // defaults
    m_key = false;
    m_head = 0;
    m_tail = 0;

    // setup defaults
    m_alt = false;
    m_shift = false;
    m_control = false;
    
    // enable buffering
    m_enabled = true;

    // setup data
    m_multithreaded = multithreaded;
}


Win32Keyboard::~Win32Keyboard()
{
    // destructor
}




bool Win32Keyboard::key(Win32Window &window)
{
    // check enabled flag
    if (!m_enabled) return false;

    // enter monitor if multithreaded
    if (m_multithreaded) m_monitor.enter();

    // update window
    window.update();

    // is a key ready?
    bool key = ready();

    // leave monitor if multithreaded
    if (m_multithreaded) m_monitor.leave();

    // get key
    return key;
}


Key Win32Keyboard::read(Win32Window &window)
{
    // check enabled flag
    if (!m_enabled) return Key();

    // key data
    Key key;

    // check if multithreaded
    if (m_multithreaded)
    {
        // check if ready
        if (!ready())
        {
            // wait for key event
            m_event.wait();

            // reset event
            m_event.reset();
        }

        // enter monitor
        m_monitor.enter();

        // remove key
        key = remove();

        // leave monitor
        m_monitor.leave();
    }
    else
    {
        // update until ready
        while (!ready())
        {
            // update window
            window.update();
        }

        // remove key
        key = remove();
    }

    // return key data
    return key;
}




void Win32Keyboard::enable()
{
    // enable buffering
    m_enabled = true;
}


void Win32Keyboard::disable()
{
    // disable buffering
    m_enabled = false;
}




LRESULT Win32Keyboard::WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    // check enabled flag
    if (!m_enabled) return 0;

    // process key messages
    if (message==WM_KEYDOWN)// || (message==WM_SYSKEYDOWN && lParam&(1<<29)))
    {
        // update modifiers
        if (wParam==VK_MENU)
        {
            // alt down
            m_alt = true;
        }
        else if (wParam==VK_SHIFT)
        {
            // shift down
            m_shift = true;
        }
        else if (wParam==VK_CONTROL)
        {
            // control down
            m_control = true;
        }

        // enter monitor if multithreaded
        if (m_multithreaded) m_monitor.enter();

        // handle key repeat count
        for (int i=0; i<(lParam&0xFF); i++)
        {
            // create key object
            Key key(wParam,m_alt,m_shift,m_control);

            // insert key
            insert(key);
        }

        // check multithreaded flag
        if (m_multithreaded)
        {
            // set event
            m_event.set();

            // leave monitor
            m_monitor.leave();
        }
    }
    else if (message==WM_KEYUP)
    {
        // update modifiers
        if (wParam==VK_MENU)
        {
            // alt up
            m_alt = false;
        }
        else if (wParam==VK_SHIFT)
        {
            // shift up
            m_shift = false;
        }
        else if (wParam==VK_CONTROL)
        {
            // control up
            m_control = false;
        }
    }

    // done
    return 0;
}




void Win32Keyboard::insert(const Key &key)
{
    // check for overflow
    if (m_head!=m_tail-1)
    {
        // insert key at head
        m_buffer[m_head] = key; 
    
        // increase head
        m_head ++;

        // wrap head from end to start
        if (m_head==sizeof(m_buffer)) m_head = 0;
    }
}


Key Win32Keyboard::remove()
{
    // get key data from tail
    Key key = m_buffer[m_tail];

    // increase tail
    m_tail ++;

    // wrap tail from end to start
    if (m_tail==sizeof(m_buffer)) m_tail = 0;

    // return key
    return key;
}


bool Win32Keyboard::ready()
{
    // check if key is ready
    return (m_head!=m_tail);
}
