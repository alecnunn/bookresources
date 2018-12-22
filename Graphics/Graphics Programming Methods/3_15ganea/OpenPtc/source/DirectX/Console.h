//
// DirectX Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DIRECTX_CONSOLE_H
#define __PTC_DIRECTX_CONSOLE_H

// include files
#include "config.h"
#include "Core/Base.h"
#include "Core/Copy.h"
#include "Core/Mode.h"
#include "Core/Types.h"
#include "Core/Clear.h"
#include "Core/Format.h"
#include "Core/Palette.h"
#include "Win32/Window.h"
#include "Win32/Keyboard.h"
#include "DirectX/Hook.h"
#include "DirectX/Library.h"
#include "DirectX/Display.h"
#include "DirectX/Primary.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DirectXConsole : public BaseConsole
{
    // friend classes
    friend class DirectXHook;

    public:

        // setup
        DirectXConsole();
        ~DirectXConsole();

        // console configuration
        virtual void PTCAPI configure(const char file[]);

        // console option string
        virtual bool PTCAPI option(const char option[]);

        // console modes
        virtual const Mode* PTCAPI modes();

        // console management
        virtual void PTCAPI open(const char title[],int pages = 0);
        virtual void PTCAPI open(const char title[],const Format &format,int pages = 0);
        virtual void PTCAPI open(const char title[],int width,int height,const Format &format,int pages = 0);
        virtual void PTCAPI open(const char title[],const Mode &mode,int pages = 0);
        virtual void PTCAPI close();
               
        // synchronization
        virtual void PTCAPI flush();
        virtual void PTCAPI finish();
        virtual void PTCAPI update();
        virtual void PTCAPI update(const Area &area);

        // keyboard input
        virtual bool PTCAPI key();
        virtual Key PTCAPI read();

        // copy to surface
        virtual void PTCAPI copy(BaseSurface &surface);
        virtual void PTCAPI copy(BaseSurface &surface,const Area &source,const Area &destination);

        // memory access
        virtual void* PTCAPI lock();
        virtual void PTCAPI unlock();

        // load pixels to console
        virtual void PTCAPI load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette);
        virtual void PTCAPI load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination);

        // save console pixels
        virtual void PTCAPI save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette);
        virtual void PTCAPI save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination);

        // clear console
        virtual void PTCAPI clear();
        virtual void PTCAPI clear(const Color &color);
        virtual void PTCAPI clear(const Color &color,const Area &area);

        // console palette
        virtual void PTCAPI palette(const Palette &palette);
        virtual const Palette& PTCAPI palette() const;

        // console clip area
        virtual void PTCAPI clip(const Area &area);

        // data access
        virtual int PTCAPI width() const;
        virtual int PTCAPI height() const;
        virtual int PTCAPI pages() const;
        virtual int PTCAPI pitch() const;
        virtual const Area& PTCAPI area() const;
        virtual const Area& PTCAPI clip() const;
        virtual const Format& PTCAPI format() const;
        virtual const char* PTCAPI name() const;
        virtual const char* PTCAPI title() const;
        virtual const char* PTCAPI information() const;

#ifdef __PTC_WIN32_EXTENSIONS__

        // extension functions
        virtual void PTCAPI open(HWND window,int pages = 0);
        virtual void PTCAPI open(HWND window,const Format &format,int pages = 0);
        virtual void PTCAPI open(HWND window,int width,int height,const Format &format,int pages = 0);
        virtual void PTCAPI open(HWND window,const Mode &mode,int pages = 0);
        virtual HWND PTCAPI window() const;
        virtual LPDIRECTDRAW PTCAPI lpDD() const;
        virtual LPDIRECTDRAW2 PTCAPI lpDD2() const;
        virtual LPDIRECTDRAWSURFACE PTCAPI lpDDS() const;
        virtual LPDIRECTDRAWSURFACE PTCAPI lpDDS_primary() const;
        virtual LPDIRECTDRAWSURFACE PTCAPI lpDDS_secondary() const;

#endif

    private:

        // internal console management routines
        void internal_open(const char title[],HWND window,const Mode &mode,int pages,bool exact);
        void internal_recycle(const char title[],HWND window,const Mode &mode,int pages,bool exact);
        void internal_close();
        void internal_shutdown();

        // internal console open management routines
        void internal_open_start(const char title[],HWND window);
        void internal_open_finish();
        void internal_open_reset();

        // internal fullscreen open routines
        void internal_open_fullscreen_start(HWND window,const Mode &mode,bool exact);
        void internal_open_fullscreen_change(const Mode &mode,bool exact);
        void internal_open_fullscreen_surface(const Mode &mode,int pages);
        void internal_open_fullscreen_finish();

        // internal windowed open routines
        void internal_open_windowed_start(HWND window,const Mode &mode,bool exact);
        void internal_open_windowed_change(const Mode &mode,bool exact);
        void internal_open_windowed_surface(const Mode &mode,int pages);
        void internal_open_windowed_finish();

        // internal console open recycling routines
        void internal_recycle_fullscreen(const char title[],HWND window,const Mode &mode,int pages,bool exact);
        void internal_recycle_windowed(const char title[],HWND window,const Mode &mode,int pages,bool exact);

        // painting
        void paint();

        // cursor state 
        void update_cursor();

        // disallow copy construction
        DirectXConsole(const DirectXConsole &console);

        // disallow console assignment
        DirectXConsole& operator =(const DirectXConsole &console);

        // title data
        char m_title[1024];

        // flags
        bool m_open;
        bool m_locked;
        bool m_cursor;

        // output type
        enum Output
        {
            DEFAULT,
            WINDOWED,
            FULLSCREEN
        };

        // window type
        enum Window
        {
            RESIZABLE,
            FIXED
        };

        // primary type
        enum Primary
        {
            DIRECT,
            SECONDARY
        };

        // nearest type
        enum Nearest
        {
            NEAREST_DEFAULT,
            NEAREST_CENTERING,
            NEAREST_STRETCHING
        };

        // cursor type
        enum Cursor
        {
            CURSOR_DEFAULT,
            CURSOR_SHOW,
            CURSOR_HIDE
        };

        // option data
        int m_frequency;
        int m_default_width;
        int m_default_height;
        int m_default_pages;
        bool m_center_window;
        bool m_synchronized_update;
        Format m_default_format;
        Output m_output_mode;
        Window m_window_mode;
        Primary m_primary_mode_windowed;
        Primary m_primary_mode_fullscreen;
        Nearest m_nearest_mode;
        Cursor m_cursor_mode;

        // objects
        Copy m_copy;

        // Win32 objects
        Win32Window *m_window;
        Win32Keyboard *m_keyboard;

        // DirectX objects
        DirectXHook *m_hook;
        DirectXLibrary m_library;
        DirectXDisplay m_display;
        DirectXPrimary m_primary;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
