//
// C Surface class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Except.h"
#include "C/Surface.h"
#include "Core/Surface.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_SURFACE PTCAPI ptc_surface_create(int width,int height,PTC_FORMAT format)
{
    try
    {
        // create surface object
        return (PTC_SURFACE) new Surface(width,height,*((Format*)format));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_surface_destroy(PTC_SURFACE object)
{
    try
    {
        // destroy console object
        delete (BaseSurface*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_surface_copy(PTC_SURFACE object,PTC_SURFACE surface)
{
    try
    {
        // copy surface to surface
        ((BaseSurface*)object)->copy(*((BaseSurface*)surface));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_surface_copy_area(PTC_SURFACE object,PTC_SURFACE surface,PTC_AREA source,PTC_AREA destination)
{
    try
    {
        // copy surface area to surface area
        ((BaseSurface*)object)->copy(*((BaseSurface*)surface),*((Area*)source),*((Area*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void* PTCAPI ptc_surface_lock(PTC_SURFACE object)
{
    try
    {
        // lock surface
        return ((BaseSurface*)object)->lock();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_surface_unlock(PTC_SURFACE object)
{
    try
    {
        // unlock surface
        ((BaseSurface*)object)->unlock();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_surface_load(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette)
{
    try
    {
        // load pixels to surface
        ((BaseSurface*)object)->load(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_surface_load_area(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination)
{
    try
    {
        // load pixels to surface area
        ((BaseSurface*)object)->load(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette),*((Area*)source),*((Area*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_surface_save(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette)
{
    try
    {
        // save surface pixels
        ((BaseSurface*)object)->save(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_surface_save_area(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination)
{
    try
    {
        // save surface area pixels
        ((BaseSurface*)object)->save(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette),*((Area*)source),*((Area*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_surface_clear(PTC_SURFACE object)
{
    try
    {
        // clear surface
        ((BaseSurface*)object)->clear();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_surface_clear_color(PTC_SURFACE object,PTC_COLOR color)
{
    try
    {
        // clear surface color
        ((BaseSurface*)object)->clear(*((Color*)color));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_surface_clear_color_area(PTC_SURFACE object,PTC_COLOR color,PTC_AREA area)
{
    try
    {
        // clear surface color area
        ((BaseSurface*)object)->clear(*((Color*)color),*((Area*)area));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_surface_palette_set(PTC_SURFACE object,PTC_PALETTE palette)
{
    try
    {
        // set surface palette
        ((BaseSurface*)object)->palette(*((Palette*)palette));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI PTC_PALETTE PTCAPI ptc_surface_palette_get(PTC_SURFACE object)
{
    try
    {
        // get surface palette
        return (PTC_PALETTE) & ((BaseSurface*)object)->palette();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_surface_clip_set(PTC_SURFACE object,PTC_AREA area)
{
    try
    {
        // set surface clip area
        ((BaseSurface*)object)->clip(*((Area*)area));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_surface_width(PTC_SURFACE object)
{
    try
    {
        // get surface width
        return ((BaseSurface*)object)->width();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_surface_height(PTC_SURFACE object)
{
    try
    {
        // get surface height
        return ((BaseSurface*)object)->height();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_surface_pitch(PTC_SURFACE object)
{
    try
    {
        // get surface pitch
        return ((BaseSurface*)object)->pitch();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_AREA PTCAPI ptc_surface_area(PTC_SURFACE object)
{
    try
    {
        // get surface area
        return (PTC_AREA) & ((BaseSurface*)object)->area();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_AREA PTCAPI ptc_surface_clip(PTC_SURFACE object)
{
    try
    {
        // get surface clip
        return (PTC_AREA) & ((BaseSurface*)object)->clip();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_FORMAT PTCAPI ptc_surface_format(PTC_SURFACE object)
{
    try
    {
        // get surface format
        return (PTC_FORMAT) & ((BaseSurface*)object)->format();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI int PTCAPI ptc_surface_option(PTC_SURFACE object,char *option)
{
    try
    {
        // surface option
        return (int) ((BaseSurface*)object)->option(option);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
