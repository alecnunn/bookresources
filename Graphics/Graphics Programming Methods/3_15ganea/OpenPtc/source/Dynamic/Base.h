//
// Dynamic Base classes for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_BASE_H
#define __PTC_DYNAMIC_BASE_H

// include files
#include "config.h"
#include "Core/Types.h"
#include "Dynamic/Index.h"
#ifdef __COMPILER_SUPPORTS_DDRAW__
#include <ddraw.h>
#else
#include <windows.h>
#endif

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicBaseSurface
{
    public:

        // virtual destructor
        virtual ~DynamicBaseSurface() {};

        // copy to surface
        virtual void copy(DynamicBaseSurface &surface) = 0;
        virtual void copy(DynamicBaseSurface &surface,const DynamicArea &source,const DynamicArea &destination) = 0;

        // surface memory access
        virtual void* lock() = 0;
        virtual void unlock() = 0;

        // load pixels to surface
        virtual void load(const void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette) = 0;
        virtual void load(const void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette,const DynamicArea &source,const DynamicArea &destination) = 0;

        // save surface pixels
        virtual void save(void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette) = 0;
        virtual void save(void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette,const DynamicArea &source,const DynamicArea &destination) = 0;

        // clear surface
        virtual void clear() = 0;
        virtual void clear(const DynamicColor &color) = 0;
        virtual void clear(const DynamicColor &color,const DynamicArea &area) = 0;

        // surface palette
        virtual void palette(const DynamicPalette &palette) = 0;
        virtual const DynamicPalette& palette() const = 0;

        // surface clip area
        virtual void clip(const DynamicArea &area) = 0;

        // data access
        virtual int width() const = 0;
        virtual int height() const = 0;
        virtual int pitch() const = 0;
        virtual const DynamicArea& area() const = 0;
        virtual const DynamicArea& clip() const = 0;
        virtual const DynamicFormat& format() const = 0;

        // surface option string
        virtual bool option(const char option[]) = 0;

    private:

        virtual PTC_SURFACE surface()
        {
            // get ptc surface object
            return (PTC_SURFACE) this;
        }

        // friend classes
        friend class DynamicSurface;
        friend class DynamicConsole;
};




class DynamicBaseConsole : public DynamicBaseSurface
{
    public:

        // virtual destructor
        virtual ~DynamicBaseConsole() {};

        // console configuration
        virtual void configure(const char file[]) = 0;

        // console modes
        virtual const DynamicMode* modes() = 0;

        // console management
        virtual void open(const char title[],int pages = 0) = 0;
        virtual void open(const char title[],const DynamicFormat &format,int pages = 0) = 0;
        virtual void open(const char title[],int width,int height,const DynamicFormat &format,int pages = 0) = 0;
        virtual void open(const char title[],const DynamicMode &mode,int pages = 0) = 0;
        virtual void close() = 0;

        // synchronization
        virtual void flush() = 0;
        virtual void finish() = 0;
        virtual void update() = 0;
        virtual void update(const DynamicArea &area) = 0;

        // keyboard input
        virtual bool key() = 0;
        virtual DynamicKey read() = 0;

        // data access
        virtual int pages() const = 0;
        virtual const char* name() const = 0;
        virtual const char* title() const = 0;
        virtual const char* information() const = 0;

#ifdef __PTC_WIN32_EXTENSIONS__

        // extension functions
        virtual void open(HWND window,int pages = 0) = 0;
        virtual void open(HWND window,const DynamicFormat &format,int pages = 0) = 0;
        virtual void open(HWND window,int width,int height,const DynamicFormat &format,int pages = 0) = 0;
        virtual void open(HWND window,const DynamicMode &mode,int pages = 0) = 0;
        virtual HWND window() const = 0;
        virtual LPDIRECTDRAW lpDD() const = 0;
        virtual LPDIRECTDRAW2 lpDD2() const = 0;
        virtual LPDIRECTDRAWSURFACE lpDDS() const = 0;
        virtual LPDIRECTDRAWSURFACE lpDDS_primary() const = 0;
        virtual LPDIRECTDRAWSURFACE lpDDS_secondary() const = 0;

#endif
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
