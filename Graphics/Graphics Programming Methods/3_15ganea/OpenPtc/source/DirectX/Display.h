//
// DirectX Display class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DIRECTX_DISPLAY_H
#define __PTC_DIRECTX_DISPLAY_H

// include files
#include <ddraw.h>
#include <windows.h>
#include "config.h"
#include "Core/Mode.h"
#include "Core/Format.h"
#include "Win32/Event.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DirectXDisplay
{
    public:

        // setup
        DirectXDisplay();
        ~DirectXDisplay();

        // display setup
        void setup(LPDIRECTDRAW2 ddraw);

        // display modes
        const Mode* modes();

        // display management
        bool test(const Mode &mode,bool exact);
        void cooperative(HWND window,bool fullscreen);
        void open();
        void open(const Mode &mode,bool exact,int frequency);
        void close();

        // desktop management
        void save();
        void restore();

        // data access
        Mode mode() const;
        bool fullscreen() const;
        const char* information() const;

    public:

        // display data
        struct Data
        {
            DirectXDisplay *display;
            const char *title;
            int width;
            int height;
            const Format *format;
            Win32Event event;
        };

    private:

        // internal display open
        void internal_open(const Mode &mode,bool exact,int frequency);
        void internal_open_nearest(const Mode &mode,bool exact,int frequency);

        // display mode enumeration callback
        static HRESULT WINAPI callback(LPDDSURFACEDESC descriptor,LPVOID context);

        // data
        bool m_open;
        bool m_fullscreen;
        Mode m_mode;
        HWND m_window;
        LPDIRECTDRAW2 m_ddraw;

        // array data
        int m_modes_count;
        int m_resolutions_count;
        Mode m_modes[256];
        Mode m_resolutions[256];
        char m_information[1024];

        // foreground data
        HWND m_foreground;
        RECT m_foreground_rect;
        WINDOWPLACEMENT m_foreground_placement;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
