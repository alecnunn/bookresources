//
// Dynamic Surface class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_SURFACE_H
#define __PTC_DYNAMIC_SURFACE_H

// include files
#include "config.h"
#include "C/Extra.h"
#include "C/Surface.h"
#include "Core/Types.h"
#include "Dynamic/Base.h"
#include "Dynamic/Copy.h"
#include "Dynamic/Area.h"
#include "Dynamic/Clear.h"
#include "Dynamic/Format.h"
#include "Dynamic/Palette.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicSurface : public DynamicBaseSurface
{
    public:

        DynamicSurface(int width,int height,const DynamicFormat &format)
        {
            // use exceptions
            ptc_use_exceptions();

            // setup surface object
            object = ptc_surface_create(width,height,format.object);
        }

        ~DynamicSurface()
        {
            // destroy surface object
            ptc_surface_destroy(object);
        }

        void copy(DynamicBaseSurface &surface)
        {
            // copy surface to surface
            ptc_surface_copy(object,surface.surface());
        }

        void copy(DynamicBaseSurface &surface,const DynamicArea &source,const DynamicArea &destination)
        {
            // copy surface area to surface area
            ptc_surface_copy_area(object,surface.surface(),source.object,destination.object);
        }

        void* lock()
        {
            // lock surface
            return ptc_surface_lock(object);
        }

        void unlock()
        {
            // unlock surface
            ptc_surface_unlock(object);
        }

        void load(const void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette)
        {
            // load pixels to surface
            ptc_surface_load(object,(void*)pixels,width,height,pitch,format.object,palette.object);
        }

        void load(const void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette,const DynamicArea &source,const DynamicArea &destination)
        {
            // load pixels to surface area
            ptc_surface_load_area(object,(void*)pixels,width,height,pitch,format.object,palette.object,source.object,destination.object);
        }

        void save(void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette)
        {
            // save surface pixels
            ptc_surface_save(object,pixels,width,height,pitch,format.object,palette.object);
        }

        void save(void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette,const DynamicArea &source,const DynamicArea &destination)
        {
            // save surface area pixels
            ptc_surface_save_area(object,pixels,width,height,pitch,format.object,palette.object,source.object,destination.object);
        }

        void clear()
        {
            // clear surface
            ptc_surface_clear(object);
        }

        void clear(const DynamicColor &color)
        {
            // clear surface color
            ptc_surface_clear_color(object,color.object);
        }

        void clear(const DynamicColor &color,const DynamicArea &area)
        {
            // clear surface color area
            ptc_surface_clear_color_area(object,color.object,area.object);
        }

        void palette(const DynamicPalette &palette)
        {
            // set surface palette
            ptc_surface_palette_set(object,palette.object);
        }

        const DynamicPalette& palette() const
        {
            // get surface palette
            ptc_palette_assign(kludge_palette.object,ptc_surface_palette_get(object));
            return kludge_palette;
        }
   
        void clip(const DynamicArea &area)
        {
            // set surface clip area
            ptc_surface_clip_set(object,area.object);
        }

        int width() const
        {
            // get surface width
            return ptc_surface_width(object);
        }

        int height() const
        {
            // get surface height
            return ptc_surface_height(object);
        }

        int pitch() const
        {
            // get surface pitch
            return ptc_surface_pitch(object);
        }

        const DynamicArea& area() const
        {
            // get surface area
            ptc_area_assign(kludge_area.object,ptc_surface_area(object));
            return kludge_area;
        }

        const DynamicArea& clip() const
        {
            // get surface clip
            ptc_area_assign(kludge_clip.object,ptc_surface_clip(object));
            return kludge_clip;
        }

        const DynamicFormat& format() const
        {
            // setup kludge format object
            ptc_format_assign(kludge_format.object,ptc_surface_format(object));
            return kludge_format;
        }

        bool option(const char option[])
        {
            // surface option
            return (ptc_surface_option(object,(char*)option)==1);
        }
        
    private:

        // disallow copy construction
        DynamicSurface(const DynamicSurface &surface);

        // disallow assignment
        DynamicSurface& operator =(const DynamicSurface &surface);

        PTC_SURFACE surface()
        {
            // get surface
            return object;
        }

        // surface object
        PTC_SURFACE object;

        // kludge objects
        DynamicArea kludge_area;
        DynamicArea kludge_clip;
        DynamicFormat kludge_format;
        DynamicPalette kludge_palette;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
