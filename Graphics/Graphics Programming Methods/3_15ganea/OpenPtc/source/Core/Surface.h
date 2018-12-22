//
// Surface class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_SURFACE_H
#define __PTC_SURFACE_H

// include files
#include "config.h"
#include "Core/Base.h"
#include "Core/Copy.h"
#include "Core/Area.h"
#include "Core/Types.h"
#include "Core/Clear.h"
#include "Core/Format.h"
#include "Core/Palette.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Surface : public BaseSurface
{
    public:

        // setup
        PTCAPI Surface(int width,int height,const Format &format);
        PTCAPI ~Surface();

        // copy to surface
        virtual void PTCAPI copy(BaseSurface &surface);
        virtual void PTCAPI copy(BaseSurface &surface,const Area &source,const Area &destination);

        // memory access
        virtual void* PTCAPI lock();
        virtual void PTCAPI unlock();

        // load pixels to surface
        virtual void PTCAPI load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette);
        virtual void PTCAPI load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination);

        // save surface pixels
        virtual void PTCAPI save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette);
        virtual void PTCAPI save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination);

        // clear surface
        virtual void PTCAPI clear();
        virtual void PTCAPI clear(const Color &color);
        virtual void PTCAPI clear(const Color &color,const Area &area);

        // surface palette
        virtual void PTCAPI palette(const Palette &palette);
        virtual const Palette& PTCAPI palette() const;
   
        // surface clip area
        virtual void PTCAPI clip(const Area &area);
        
        // data access
        virtual int PTCAPI width() const;
        virtual int PTCAPI height() const;
        virtual int PTCAPI pitch() const;
        virtual const Area& PTCAPI area() const;
        virtual const Area& PTCAPI clip() const;
        virtual const Format& PTCAPI format() const;

        // surface option string
        virtual bool PTCAPI option(const char option[]);

    private:

        // disallow copy construction
        Surface(const Surface &surface);

        // disallow surface assignment
        Surface& operator =(const Surface &surface);

        // data
        int m_width;
        int m_height;
        int m_pitch;
        Area m_area;
        Area m_clip;
        Format m_format;
        bool m_locked;
        void *m_pixels;
        
        // objects
        Copy m_copy;
        Clear m_clear;
        Palette m_palette;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
