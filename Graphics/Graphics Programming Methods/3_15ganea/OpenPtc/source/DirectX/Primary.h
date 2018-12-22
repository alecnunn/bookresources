//
// DirectX Primary class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DIRECTX_PRIMARY_H
#define __PTC_DIRECTX_PRIMARY_H

// include files
#include <ddraw.h>
#include "config.h"
#include "Core/Area.h"
#include "Core/Copy.h"
#include "Core/Clear.h"
#include "Core/Types.h"
#include "Core/Format.h"
#include "Core/Palette.h"
#include "Win32/Window.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DirectXPrimary
{
    public:

        // setup
        DirectXPrimary();
        ~DirectXPrimary();

        // management
        void initialize(Win32Window &window,LPDIRECTDRAW2 lpDD2);
        void primary(int pages,bool video,bool fullscreen,bool palette,bool complex);
        void secondary(int width,int height);
        void synchronize(bool update);
        void centering(bool center);
        void close();

        // sychronization
        void update();

        // surface memory access
        void* lock();
        void unlock();

        // clear surface
        void clear();
        void clear(const Color &color,const Area &area);

        // surface palette
        void palette(const Palette &palette);
        const Palette& palette() const;

        // surface clip area
        void clip(const Area &area);

        // data access
        int width() const;
        int height() const;
        int pages() const;
        int pitch() const;
        const Area& area() const;
        const Area& clip() const;
        const Format& format() const;

        // directdraw data access
        LPDIRECTDRAWSURFACE lpDDS() const;
        LPDIRECTDRAWSURFACE lpDDS_primary() const;
        LPDIRECTDRAWSURFACE lpDDS_secondary() const;

        // primary management
        void activate();
        void deactivate();
        bool active() const;
        void block() const;
        void save() const;
        void restore() const;

        // painting
        void paint();

        // primary blocking toggle
        void blocking(bool blocking);

    private:

        // internal color packing functions
        int32 pack(const Color &color,const Format &format);
        void analyse(int32 mask,int &base,int &size);

        // data
        int m_width;
        int m_height;
        int m_pages;
        Area m_area;
        Area m_clip;
        Format m_format;

        // flag data
        bool m_active;
        bool m_blocking;
        bool m_centering;
        bool m_fullscreen;
        bool m_synchronize;

        // clear object
        Clear m_clear;

        // window object
        Win32Window *m_window;

        // locked buffer
        void *m_locked;

        // palette object
        Palette m_palette;

        // primary data
        int m_primary_width;
        int m_primary_height;

        // secondary data
        int m_secondary_width;
        int m_secondary_height;

        // ddraw interface
        LPDIRECTDRAW2 m_lpDD2;

        // primary surface interfaces
        LPDIRECTDRAWSURFACE m_lpDDS;
        LPDIRECTDRAWSURFACE m_lpDDS_primary;
        LPDIRECTDRAWSURFACE m_lpDDS_primary_back;
        LPDIRECTDRAWSURFACE m_lpDDS_primary_page[32];

        // secondary surface interface
        LPDIRECTDRAWSURFACE m_lpDDS_secondary;

        // primary clipper interface
        LPDIRECTDRAWCLIPPER m_lpDDC;
        
        // working surface interfaces
        LPDIRECTDRAWSURFACE m_back;
        LPDIRECTDRAWSURFACE m_front;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
