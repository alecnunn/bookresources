//
// Base classes for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_BASE_H
#define __PTC_BASE_H

// include files
#include "config.h"
#include "Core/Index.h"
#include "Core/Types.h"
#ifdef __PTC_WIN32_EXTENSIONS__
#include <ddraw.h>
#endif

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class BaseSurface
{
    public:

        // virtual destructor
        virtual PTCAPI ~BaseSurface() {};

        // copy to surface
        virtual void PTCAPI copy(BaseSurface &surface) = 0;
        virtual void PTCAPI copy(BaseSurface &surface,const Area &source,const Area &destination) = 0;

        // surface memory access
        virtual void* PTCAPI lock() = 0;
        virtual void PTCAPI unlock() = 0;

        // load pixels to surface
        virtual void PTCAPI load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette) = 0;
        virtual void PTCAPI load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination) = 0;

        // save surface pixels
        virtual void PTCAPI save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette) = 0;
        virtual void PTCAPI save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination) = 0;

        // clear surface
        virtual void PTCAPI clear() = 0;
        virtual void PTCAPI clear(const Color &color) = 0;
        virtual void PTCAPI clear(const Color &color,const Area &area) = 0;

        // surface palette
        virtual void PTCAPI palette(const Palette &palette) = 0;
        virtual const Palette& PTCAPI palette() const = 0;

        // surface clip area
        virtual void PTCAPI clip(const Area &area) = 0;

        // data access
        virtual int PTCAPI width() const = 0;
        virtual int PTCAPI height() const = 0;
        virtual int PTCAPI pitch() const = 0;
        virtual const Area& PTCAPI area() const = 0;
        virtual const Area& PTCAPI clip() const = 0;
        virtual const Format& PTCAPI format() const = 0;

        // surface option string
        virtual bool PTCAPI option(const char option[]) = 0;
};




class BaseConsole : public BaseSurface
{
    public:

        // virtual destructor
        virtual PTCAPI ~BaseConsole() {};

        // console configuration
        virtual void PTCAPI configure(const char file[]) = 0;

        // console modes
        virtual const Mode* PTCAPI modes() = 0;

        // console management
        virtual void PTCAPI open(const char title[],int pages = 0) = 0;
        virtual void PTCAPI open(const char title[],const Format &format,int pages = 0) = 0;
        virtual void PTCAPI open(const char title[],int width,int height,const Format &format,int pages = 0) = 0;
        virtual void PTCAPI open(const char title[],const Mode &mode,int pages = 0) = 0;
        virtual void PTCAPI close() = 0;

        // synchronization
        virtual void PTCAPI flush() = 0;
        virtual void PTCAPI finish() = 0;
        virtual void PTCAPI update() = 0;
        virtual void PTCAPI update(const Area &area) = 0;

        // keyboard input
        virtual bool PTCAPI key() = 0;
        virtual Key PTCAPI read() = 0;

        // data access
        virtual int PTCAPI pages() const = 0;
        virtual const char* PTCAPI name() const = 0;
        virtual const char* PTCAPI title() const = 0;
        virtual const char* PTCAPI information() const = 0;

#ifdef __PTC_WIN32_EXTENSIONS__

        // extension functions
        virtual void PTCAPI open(HWND window,int pages = 0) = 0;
        virtual void PTCAPI open(HWND window,const Format &format,int pages = 0) = 0;
        virtual void PTCAPI open(HWND window,int width,int height,const Format &format,int pages = 0) = 0;
        virtual void PTCAPI open(HWND window,const Mode &mode,int pages = 0) = 0;
        virtual HWND PTCAPI window() const = 0;
        virtual LPDIRECTDRAW PTCAPI lpDD() const = 0;
        virtual LPDIRECTDRAW2 PTCAPI lpDD2() const = 0;
        virtual LPDIRECTDRAWSURFACE PTCAPI lpDDS() const = 0;
        virtual LPDIRECTDRAWSURFACE PTCAPI lpDDS_primary() const = 0;
        virtual LPDIRECTDRAWSURFACE PTCAPI lpDDS_secondary() const = 0;

#endif
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
