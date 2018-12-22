//
// Surface class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <malloc.h>
#include "Core/Log.h"
#include "Core/Area.h"
#include "Core/Copy.h"
#include "Core/Clear.h"
#include "Core/Error.h"
#include "Core/Color.h"
#include "Core/Format.h"
#include "Core/Clipper.h"
#include "Core/Surface.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Surface::Surface(int width,int height,const Format &format)
{
    // log surface creation
    LOG("creating surface");
    LOG("width",width);
    LOG("height",height);
    LOG("format",format);
    
    // setup data
    m_width  = width;
    m_height = height;
    m_format = format;
    m_locked = false;
    m_pixels = 0;

    // setup surface area
    m_area = Area(0,0,width,height);

    // setup clip area
    m_clip = m_area;

    // surface memory is always linear
    m_pitch  = width * format.bytes();

    // calculate size of pixels
    int size = width * height * format.bytes();

    // check size
    if (!size) throw Error("zero surface size");

    // allocate pixels
    m_pixels = malloc(size);

    // check pixels were correctly allocated
    if (!m_pixels) throw Error("could not allocate surface pixels");

    // clear
    clear();
}


PTCAPI Surface::~Surface()
{
    // surface must be unlocked when it is destroyed
    if (m_locked) throw Error("surface is still locked");

    // free pixels
    free(m_pixels);
}




void PTCAPI Surface::copy(BaseSurface &surface)
{
    // load surface pixels to other surface
    surface.load(m_pixels,m_width,m_height,m_pitch,m_format,m_palette);
}


void PTCAPI Surface::copy(BaseSurface &surface,const Area &source,const Area &destination)
{
    // load surface pixels to other surface
    surface.load(m_pixels,m_width,m_height,m_pitch,m_format,m_palette,source,destination);
}




void* PTCAPI Surface::lock()
{
    // fail if surface is already locked
    if (m_locked) throw Error("surface is already locked");

    // set locked flag
    m_locked = true;

    // lock surface
    return m_pixels;
}


void PTCAPI Surface::unlock()
{
    // fail if surface is not locked
    if (!m_locked) throw Error("surface is not locked");

    // reset locked flag
    m_locked = false;
}




void PTCAPI Surface::load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette)
{
    // check clip area
    if (m_clip==m_area)
    {
        // request format conversion
        m_copy.request(format,m_format);
    
        // set copy palettes
        m_copy.palette(palette,m_palette);
    
        // copy pixels to surface
        m_copy.copy(pixels,0,0,width,height,pitch,m_pixels,0,0,m_width,m_height,m_pitch);
    }
    else
    {
        // load explicit areas
        load(pixels,width,height,pitch,format,palette,Area(0,0,width,height),m_area);
    }
}


void PTCAPI Surface::load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination)
{
    // clip source and destination areas
    Area clipped_source,clipped_destination;
    Clipper::clip(source,Area(0,0,width,height),clipped_source,
               destination,m_clip,clipped_destination);

    // request format conversion
    m_copy.request(format,m_format);
    
    // set copy palettes
    m_copy.palette(palette,m_palette);

    // copy pixels to surface
    m_copy.copy(pixels,clipped_source.left(),clipped_source.top(),clipped_source.width(),clipped_source.height(),pitch,
                m_pixels,clipped_destination.left(),clipped_destination.top(),clipped_destination.width(),clipped_destination.height(),m_pitch);
}




void PTCAPI Surface::save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette)
{
    // check clip area
    if (clip()==area())
    {
        // request format conversion
        m_copy.request(m_format,format);
    
        // set copy palettes
        m_copy.palette(m_palette,palette);

        // copy surface pixels to 'pixels' buffer
        m_copy.copy(m_pixels,0,0,m_width,m_height,m_pitch,pixels,0,0,width,height,pitch);
    }
    else
    {
        // save explicit areas
        save(pixels,width,height,pitch,format,palette,m_area,Area(0,0,width,height));
    }
}


void PTCAPI Surface::save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination)
{
    // clip source and destination areas
    Area clipped_source,clipped_destination;
    Clipper::clip(source,m_clip,clipped_source,destination,Area(0,0,width,height),clipped_destination);

    // request format conversion
    m_copy.request(m_format,format);
    
    // set copy palettes
    m_copy.palette(m_palette,palette);

    // copy surface pixels to 'pixels' buffer
    m_copy.copy(m_pixels,clipped_source.left(),clipped_source.top(),clipped_source.width(),clipped_source.height(),m_pitch,
                pixels,clipped_destination.left(),clipped_destination.top(),clipped_destination.width(),clipped_destination.height(),pitch);
}




void PTCAPI Surface::clear()
{
    // check surface format
    if (format().direct())
    {
        // direct color
        clear(Color(0,0,0,0));
    }
    else
    {
        // indexed color
        clear(Color(0));
    }
}


void PTCAPI Surface::clear(const Color &color)
{
    // clear surface
    clear(color,m_area);
}


void PTCAPI Surface::clear(const Color &color,const Area &area)
{
    // clip clear area
    Area clipped_area = Clipper::clip(area,m_clip);
    
    // request clear
    m_clear.request(m_format);

    // clear surface area
    m_clear.clear(m_pixels,clipped_area.left(),clipped_area.top(),clipped_area.width(),clipped_area.height(),m_pitch,color);
}




void PTCAPI Surface::palette(const Palette &palette)
{
    // set palette
    m_palette.load(palette.data());
}


const Palette& PTCAPI Surface::palette() const
{
    // get palette
    return m_palette;
}




void PTCAPI Surface::clip(const Area &area)
{
    // set new clip area
    m_clip = Clipper::clip(area,m_area);
}




int PTCAPI Surface::width() const
{
    // get width
    return m_width;
}


int PTCAPI Surface::height() const
{
    // get height
    return m_height;
}


int PTCAPI Surface::pitch() const
{
    // get pitch
    return m_pitch;
}


const Area& PTCAPI Surface::area() const
{
    // get area
    return m_area;
}


const Area& PTCAPI Surface::clip() const
{
    // get clip
    return m_clip;
}


const Format& PTCAPI Surface::format() const
{
    // get format
    return m_format;
}


bool PTCAPI Surface::option(const char option[])
{
    // pass the option to the copy object
    return m_copy.option(option);
}
