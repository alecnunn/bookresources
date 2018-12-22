//
// GDI Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <stdio.h>
#include <string.h>
#include <fstream.h>
#include "Core/Key.h"
#include "Core/Area.h"
#include "Core/Error.h"
#include "GDI/Console.h"




PTCAPI GDIConsole::GDIConsole()
{
    // error message
    throw Error("GDI console is not implemented yet!");
}


PTCAPI GDIConsole::~GDIConsole()
{
}




void PTCAPI GDIConsole::configure(const char file[])
{
}




bool PTCAPI GDIConsole::option(const char option[])
{
    return false;
}




const Mode* PTCAPI GDIConsole::modes()
{
    // fudge mode list...
    m_modes[0] = Mode(320,200,Format(8));
    m_modes[1] = Mode(320,240,Format(8));
    m_modes[2] = Mode(512,384,Format(8));
    return m_modes;
}




void PTCAPI GDIConsole::open(const char title[],int pages)
{
}


void PTCAPI GDIConsole::open(const char title[],const Format &format,int pages)
{
}


void PTCAPI GDIConsole::open(const char title[],int width,int height,const Format &format,int pages)
{
}


void PTCAPI GDIConsole::open(const char title[],const Mode &mode,int pages)
{
}


void PTCAPI GDIConsole::close()
{
}





void PTCAPI GDIConsole::flush()
{
}


void PTCAPI GDIConsole::finish()
{
}


void PTCAPI GDIConsole::update()
{
}


void PTCAPI GDIConsole::update(const Area &area)
{
}




bool PTCAPI GDIConsole::key()
{
    return false;
}

                         
Key PTCAPI GDIConsole::read()
{
    static Key key;
    return key;
}




void PTCAPI GDIConsole::copy(BaseSurface &surface)
{
}


void PTCAPI GDIConsole::copy(BaseSurface &surface,const Area &source,const Area &destination)
{
}




void* PTCAPI GDIConsole::lock()
{
    return 0;
}


void PTCAPI GDIConsole::unlock()
{
}




void PTCAPI GDIConsole::load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette)
{
}


void PTCAPI GDIConsole::load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination)
{
}




void PTCAPI GDIConsole::save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette)
{
}


void PTCAPI GDIConsole::save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination)
{
}




void PTCAPI GDIConsole::clear()
{
}


void PTCAPI GDIConsole::clear(const Color &color)
{
}


void PTCAPI GDIConsole::clear(const Color &color,const Area &area)
{
}




void PTCAPI GDIConsole::palette(const Palette &palette)
{
}


const Palette& PTCAPI GDIConsole::palette() const
{
    static Palette palette;
    return palette;
}




void PTCAPI GDIConsole::clip(const Area &area)
{
}




int PTCAPI GDIConsole::width() const
{
    return 0;
}


int PTCAPI GDIConsole::height() const
{
    return 0;
}


int PTCAPI GDIConsole::pages() const
{
    return 0;
}


int PTCAPI GDIConsole::pitch() const
{
    return 0;
}


const Area& PTCAPI GDIConsole::area() const
{
    static Area area;
    return area;
}


const Area& PTCAPI GDIConsole::clip() const
{
    static Area area;
    return area;
}


const Format& PTCAPI GDIConsole::format() const
{
    static Format format;
    return format;
}


const char* PTCAPI GDIConsole::name() const
{
    // get name
    return "GDI";
}


const char* PTCAPI GDIConsole::title() const
{
    return "title";
}


const char* PTCAPI GDIConsole::information() const
{
    return "information";
}




#ifdef __PTC_WIN32_EXTENSIONS__


void PTCAPI GDIConsole::open(HWND window,int pages)
{
}


void PTCAPI GDIConsole::open(HWND window,const Format &format,int pages)
{
}


void PTCAPI GDIConsole::open(HWND window,int width,int height,const Format &format,int pages)
{
}


void PTCAPI GDIConsole::open(HWND window,const Mode &mode,int pages)
{
}


HWND PTCAPI GDIConsole::window() const
{
    return 0;
}


LPDIRECTDRAW PTCAPI GDIConsole::lpDD() const
{
    return 0;
}



LPDIRECTDRAW2 PTCAPI GDIConsole::lpDD2() const
{
    return 0;
}


LPDIRECTDRAWSURFACE PTCAPI GDIConsole::lpDDS() const
{
    return 0;
}


LPDIRECTDRAWSURFACE PTCAPI GDIConsole::lpDDS_primary() const
{
    return 0;
}


LPDIRECTDRAWSURFACE PTCAPI GDIConsole::lpDDS_secondary() const
{
    return 0;
}


#endif
