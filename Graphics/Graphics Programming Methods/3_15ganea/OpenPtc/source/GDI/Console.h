//
// GDI Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_GDI_CONSOLE_H
#define __PTC_GDI_CONSOLE_H

// include files
#include "config.h"
#include "Core/Base.h"
#include "Core/Copy.h"
#include "Core/Mode.h"
#include "Core/Types.h"
#include "Core/Clear.h"
#include "Core/Format.h"
#include "Core/Palette.h"
#ifdef __PTC_WIN32_EXTENSIONS__
#include <ddraw.h>
#endif




class GDIConsole : public BaseConsole
{
    public:

        // setup
        GDIConsole();
        ~GDIConsole();

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

        // disallow copy construction
        GDIConsole(const GDIConsole &console);

        // disallow console assignment
        GDIConsole& operator =(const GDIConsole &console);

        // empty mode list
        Mode m_modes[32];
};




#endif
