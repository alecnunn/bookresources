//
// DirectX Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <stdio.h>
#include <string.h>
#include <fstream.h>
#include "Core/Key.h"
#include "Core/Log.h"
#include "Core/Area.h"
#include "Core/Color.h"
#include "Core/Error.h"
#include "Core/Clipper.h"
#include "Win32/Cursor.h"
#include "DirectX/Hook.h"
#include "DirectX/Console.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif

// define defaults
#define DEFAULT_WIDTH 320;
#define DEFAULT_HEIGHT 200;
#define DEFAULT_FORMAT Format(32,0x00FF0000,0x0000FF00,0x000000FF);
#ifdef __DEBUG__
#define DEFAULT_OUTPUT WINDOWED
#else
#define DEFAULT_OUTPUT DEFAULT
#endif

#ifdef __DEBUG__

    // debug checking defines
    #define CHECK_OPEN(function) if (!m_open) try { throw Error("console is not open"); } catch (Error &error) { throw Error(function,error); };
    #define CHECK_LOCK(function) if (m_locked) try { throw Error("console is locked"); } catch (Error &error) { throw Error(function,error); };

#else

    // release checking defines
    #define CHECK_OPEN(function)
    #define CHECK_LOCK(function)

#endif




PTCAPI DirectXConsole::DirectXConsole()
{
    // defaults
    m_open   = false;
    m_locked = false;
    m_cursor = true;

    // clear strings
    m_title[0] = 0;

    // clear objects
    m_hook = 0;
    m_window = 0;
    m_keyboard = 0;

    // default option data
    m_frequency = 0;
    m_default_width = DEFAULT_WIDTH;
    m_default_height = DEFAULT_HEIGHT;
    m_default_format = DEFAULT_FORMAT;
    m_center_window = false;
    m_synchronized_update = true;
    m_output_mode = DEFAULT_OUTPUT;
    m_window_mode = RESIZABLE;
    m_primary_mode_windowed = SECONDARY;
    m_primary_mode_fullscreen = DIRECT;
    m_nearest_mode = NEAREST_DEFAULT;
    m_cursor_mode = CURSOR_DEFAULT;

    // configure console
    configure("ptc.cfg");

    // setup display object
    m_display.setup(m_library.lpDD2());
}


PTCAPI DirectXConsole::~DirectXConsole()
{
    // close
    close();
}




void PTCAPI DirectXConsole::configure(const char file[])
{
    // open configuration file
    ifstream is(file,ios::in | ios::nocreate);
    
    // check stream
    if (!is) return;

    // read option strings
    while (!is.eof())
    {
        // option line
        char line[1024];

        // read line from stream
        is.getline(line,1024);

        // process option
        option(line);
    }
}




bool PTCAPI DirectXConsole::option(const char option[])
{
    // log console option string
    LOG("console option",option);

    // check for "default output" option
    if (stricmp(option,"default output")==0)
    {
        // default output mode
        m_output_mode = DEFAULT;

        // recognized
        return true;
    }

    // check for "windowed output" option
    if (stricmp(option,"windowed output")==0)
    {
        // windowed output mode
        m_output_mode = WINDOWED;

        // recognized
        return true;
    }

    // check for "fullscreen output" option
    if (stricmp(option,"fullscreen output")==0)
    {
        // fullscreen output mode
        m_output_mode = FULLSCREEN;
       
        // recognized
        return true;
    }

    // check for "default width" option
    if (strnicmp(option,"default width",13)==0)
    {
        // look for parameter
        if (option[13]!=0)
        {
            // we have a parameter
            m_default_width = atoi(&option[13]);
        }
        else
        {
            // no parameter
            m_default_width = DEFAULT_WIDTH;
        }

        // recognized
        return true;
    }

    // check for "default height" option
    if (strnicmp(option,"default height",14)==0)
    {                            
        // look for parameter
        if (option[14]!=0)
        {
            // we have a parameter
            m_default_height = atoi(&option[14]);
        }
        else
        {
            // no parameter
            m_default_height = DEFAULT_HEIGHT;
        }

        // recognized
        return true;
    }

    // check for "default bits" option
    if (strnicmp(option,"default bits",12)==0)
    {                            
        // look for parameter
        if (option[12]!=0)
        {
            // we have a parameter
            int bits = atoi(&option[12]);
            
            // setup format
            switch (bits)
            {
                case 8:  m_default_format = Format(8); break;
                case 16: m_default_format = Format(16,0xF800,0x07E0,0x001F); break;
                case 24: m_default_format = Format(24,0x00FF0000,0x0000FF00,0x000000FF); break;
                case 32: m_default_format = Format(32,0x00FF0000,0x0000FF00,0x000000FF); break;
                default: return false;
            }
        }
        else
        {
            // no parameter
            m_default_format = DEFAULT_FORMAT;
        }

        // recognized
        return true;
    }

    // check for "resizable window" option
    if (stricmp(option,"resizable window")==0)
    {
        // set window mode to resizable
        m_window_mode = RESIZABLE;

        // recognized
        return true;
    }

    // check for "fixed window" option
    if (stricmp(option,"fixed window")==0)
    {
        // set window mode to fixed
        m_window_mode = FIXED;

        // recognized
        return true;
    }

    // check for "windowed primary direct" option
    if (stricmp(option,"windowed primary direct")==0)
    {
        // set windowed primary mode to direct
        m_primary_mode_windowed = DIRECT;

        // recognized
        return true;
    }

    // check for "windowed primary secondary" option
    if (stricmp(option,"windowed primary secondary")==0)
    {
        // set windowed primary mode to secondary
        m_primary_mode_windowed = SECONDARY;

        // recognized
        return true;
    }

    // check for "fullscreen primary direct" option
    if (stricmp(option,"fullscreen primary direct")==0)
    {
        // set fullscreen primary mode to direct
        m_primary_mode_fullscreen = DIRECT;

        // recognized
        return true;
    }

    // check for "fullscreen primary secondary" option
    if (stricmp(option,"fullscreen primary secondary")==0)
    {
        // set fullscreen primary mode to secondary
        m_primary_mode_fullscreen = SECONDARY;

        // recognized
        return true;
    }

    // check for "center window" option
    if (stricmp(option,"center window")==0)
    {
        // set window centering on
        m_center_window = true;

        // recognized
        return true;
    }

    // check for "default window position" option
    if (stricmp(option,"default window position")==0)
    {
        // set window centering off
        m_center_window = false;

        // recognized
        return true;
    }

    // check for "synchronized update" option
    if (stricmp(option,"synchronized update")==0)
    {
        // set synchronized update to true
        m_synchronized_update = true;

        // recognized
        return true;
    }

    // check for "unsynchronized update" option
    if (stricmp(option,"unsynchronized update")==0)
    {
        // set synchronized update to false
        m_synchronized_update = false;

        // recognized
        return true;
    }

    // check for "default nearest" option
    if (stricmp(option,"default nearest")==0)
    {
        // set nearest mode to default
        m_nearest_mode = NEAREST_DEFAULT;

        // recognized
        return true;
    }

    // check for "center nearest" option
    if (stricmp(option,"center nearest")==0)
    {
        // set nearest mode to center
        m_nearest_mode = NEAREST_CENTERING;

        // recognized
        return true;
    }

    // check for "default stretch" option
    if (stricmp(option,"default stretch")==0)
    {
        // set nearest mode to default
        m_nearest_mode = NEAREST_STRETCHING;

        // recognized
        return true;
    }

    // check for "default cursor" option
    if (stricmp(option,"default cursor")==0)
    {
        // set cursor mode to default
        m_cursor_mode = CURSOR_DEFAULT;

        // update cursor
        update_cursor();

        // recognized
        return true;
    }

    // check for "show cursor" option
    if (stricmp(option,"show cursor")==0)
    {
        // set cursor mode to show
        m_cursor_mode = CURSOR_SHOW;

        // update cursor
        update_cursor();

        // recognized
        return true;
    }

    // check for "hide cursor" option
    if (stricmp(option,"hide cursor")==0)
    {
        // set cursor mode to hide
        m_cursor_mode = CURSOR_HIDE;

        // update cursor
        update_cursor();

        // recognized
        return true;
    }

    // check for "frequency [n]" option
    if (strnicmp(option,"frequency",9)==0)
    {                            
        // look for parameter
        if (option[9]!=0)
        {
            // we have a parameter
            m_frequency = atoi(&option[9]);
        }
        else
        {
            // no parameter
            m_frequency = 0;
        }

        // recognized
        return true;
    }

    // check for "enable key buffering" option
    if (stricmp(option,"enable key buffering")==0)
    {
        // check keyboard object
        if (!m_keyboard) return false;

        // enable key buffering
        m_keyboard->enable();

        // recognized
        return true;
    }

    // check for "disable key buffering" option
    if (stricmp(option,"disable key buffering")==0)
    {
        // check keyboard object
        if (!m_keyboard) return false;

        // disable key buffering
        m_keyboard->disable();

        // recognized
        return true;
    }

    // check for "enable blocking" option
    if (stricmp(option,"enable blocking")==0)
    {
        // enable blocking
        m_primary.blocking(true);
    }

    // check for "disable blocking" option
    if (stricmp(option,"disable blocking")==0)
    {
        // disable blocking
        m_primary.blocking(false);
    }

#ifdef __PTC_LOGGING__

    // check for "enable logging" option
    if (stricmp(option,"enable logging")==0)
    {
        // enable logging
        Log::enable();

        // recognized
        return true;
    }

    // check for "disable logging" option
    if (stricmp(option,"disable logging")==0)
    {
        // disable logging
        Log::disable();

        // recognized
        return true;
    }

#endif

    // pass the option to the copy object
    return m_copy.option(option);
}




const Mode* PTCAPI DirectXConsole::modes()
{
    // get display modes
    return m_display.modes();
}




void PTCAPI DirectXConsole::open(const char title[],int pages)
{
    // open console
    open(title,m_default_format,pages);
}


void PTCAPI DirectXConsole::open(const char title[],const Format &format,int pages)
{
    // open console
    open(title,m_default_width,m_default_height,m_default_format,pages);
}


void PTCAPI DirectXConsole::open(const char title[],int width,int height,const Format &format,int pages)
{
    // internal open nearest mode
    internal_open(title,0,Mode(width,height,format),pages,false);
}


void PTCAPI DirectXConsole::open(const char title[],const Mode &mode,int pages)
{
    // internal open exact mode
    internal_open(title,0,mode,pages,true);
}


void PTCAPI DirectXConsole::close()
{
    // check if open
    if (m_open)
    {
        // the console must be unlocked when closed
        if (m_locked) throw Error("console is still locked");

        // flush all key presses
        while (key()) read();
    }

    // close console
    internal_close();

    // show cursor
    Win32Cursor::show();
}





void PTCAPI DirectXConsole::flush()
{
    // debug checks
    CHECK_OPEN("DirectXConsole::flush()")
    CHECK_LOCK("DirectXConsole::flush()")

    // [platform dependent code to flush all console operations]

    // handle cursor show flag
    if (!m_cursor) SetCursor(0);

    // update window
    m_window->update();
}


void PTCAPI DirectXConsole::finish()
{
    // debug check
    CHECK_OPEN("DirectXConsole::finish()")
    CHECK_LOCK("DirectXConsole::finish()")

    // [platform dependent code to finish all console operations]

    // handle cursor show flag
    if (!m_cursor) SetCursor(0);

    // update window
    m_window->update();
}


void PTCAPI DirectXConsole::update()
{
    // debug check
    CHECK_OPEN("DirectXConsole::update()")
    CHECK_LOCK("DirectXConsole::update()")

    // update primary surface
    m_primary.update();

    // handle cursor show flag
    if (!m_cursor) SetCursor(0);

    // update window
    m_window->update();
}


void PTCAPI DirectXConsole::update(const Area &area)
{
    // update
    update();
}




bool PTCAPI DirectXConsole::key()
{
    // debug check
    CHECK_OPEN("DirectXConsole::key()")
    
    // check for key press
    return m_keyboard->key(*m_window);
}

                         
Key PTCAPI DirectXConsole::read()
{
    // debug check
    CHECK_OPEN("DirectXConsole::read()")

    // read key press from keyboard
    return m_keyboard->read(*m_window);
}




void PTCAPI DirectXConsole::copy(BaseSurface &surface)
{
    // debug check
    CHECK_OPEN("DirectXConsole::copy(surface)")
    CHECK_LOCK("DirectXConsole::copy(surface)")
    
    // lock console
    void *pixels = lock();

    try
    {
        // load surface pixels to other surface
        surface.load(pixels,width(),height(),pitch(),format(),palette());

        // unlock
        unlock();
    }
    catch (Error &error)
    {
        // unlock
        unlock();

        // error message
        throw Error("failed to copy console to surface",error);
    }
}


void PTCAPI DirectXConsole::copy(BaseSurface &surface,const Area &source,const Area &destination)
{
    // debug check
    CHECK_OPEN("DirectXConsole::copy(surface,source,destination)")
    CHECK_LOCK("DirectXConsole::copy(surface,source,destination)")
    
    // lock console
    void *pixels = lock();

    try
    {
        // load surface pixels to other surface
        surface.load(pixels,width(),height(),pitch(),format(),palette(),source,destination);

        // unlock
        unlock();
    }
    catch (Error &error)
    {
        // unlock
        unlock();

        // error message
        throw Error("failed to copy console area to surface area",error);
    }
}




void* PTCAPI DirectXConsole::lock()
{
    // debug check
    CHECK_OPEN("DirectXConsole::lock()")
    
    // fail if the console is already locked
    if (m_locked) throw Error("console is already locked");

    // lock primary surface
    void *pixels = m_primary.lock();

    // surface is locked
    m_locked = true;

    // return pixels
    return pixels;
}


void PTCAPI DirectXConsole::unlock()
{
    // debug check
    CHECK_OPEN("DirectXConsole::unlock()")
    
    // fail if the console is not locked
    if (!m_locked) throw Error("console is not locked");

    // unlock primary surface
    m_primary.unlock();

    // we are unlocked
    m_locked = false;
}




void PTCAPI DirectXConsole::load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette)
{
    // debug check
    CHECK_OPEN("DirectXConsole::load(pixels,width,height,pitch,format,palette)")
    CHECK_LOCK("DirectXConsole::load(pixels,width,height,pitch,format,palette)")

    // check clip area
    if (clip()==area())
    {
        // lock console
        void *console_pixels = lock();

        try
        {
            // request format conversion
            m_copy.request(format,this->format());
    
            // set copy palettes
            m_copy.palette(palette,this->palette());

            // copy pixels to surface
            m_copy.copy(pixels,0,0,width,height,pitch,console_pixels,0,0,this->width(),this->height(),this->pitch());

            // unlock
            unlock();
        }
        catch (Error &error)
        {
            // unlock
            unlock();

            // error message
            throw Error("failed to load pixels to console",error);
        }
    }
    else
    {
        // load explicit areas
        load(pixels,width,height,pitch,format,palette,Area(0,0,width,height),area());
    }
}


void PTCAPI DirectXConsole::load(const void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination)
{
    // debug check
    CHECK_OPEN("DirectXConsole::load(pixels,width,height,pitch,format,palette,source,destination)");
    CHECK_LOCK("DirectXConsole::load(pixels,width,height,pitch,format,palette,source,destination)");

    // lock console
    void *console_pixels = lock();

    try
    {
        // clip source and destination areas
        Area clipped_source,clipped_destination;
        Clipper::clip(source,Area(0,0,width,height),clipped_source,destination,clip(),clipped_destination);

        // request format conversion
        m_copy.request(format,this->format());
    
        // set copy palettes
        m_copy.palette(palette,this->palette());

        // copy pixels to surface
        m_copy.copy(pixels,clipped_source.left(),clipped_source.top(),clipped_source.width(),clipped_source.height(),pitch,
                    console_pixels,clipped_destination.left(),clipped_destination.top(),clipped_destination.width(),clipped_destination.height(),this->pitch());

        // unlock
        unlock();
    }
    catch (Error &error)
    {
        // unlock
        unlock();
        
        // error message
        throw Error("failed to load pixels to console area",error);
    }
}




void PTCAPI DirectXConsole::save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette)
{
    // debug check
    CHECK_OPEN("DirectXConsole::save(pixels,width,height,pitch,format,palette)")
    CHECK_LOCK("DirectXConsole::save(pixels,width,height,pitch,format,palette)")

    // check clip area
    if (clip()==area())
    {
        // lock console
        void *console_pixels = lock();

        try
        {
            // request format conversion
            m_copy.request(this->format(),format);
    
            // set copy palettes
            m_copy.palette(this->palette(),palette);

            // copy console pixels to 'pixels' buffer
            m_copy.copy(console_pixels,0,0,this->width(),this->height(),this->pitch(),pixels,0,0,width,height,pitch);

            // unlock
            unlock();
        }
        catch (Error &error)
        {
            // unlock
            unlock();
        
            // error message
            throw Error("failed to save console pixels",error);
        }
    }
    else
    {
        // save explicit areas
        save(pixels,width,height,pitch,format,palette,area(),Area(0,0,width,height));
    }
}


void PTCAPI DirectXConsole::save(void *pixels,int width,int height,int pitch,const Format &format,const Palette &palette,const Area &source,const Area &destination)
{
    // debug check
    CHECK_OPEN("DirectXConsole::save(pixels,width,height,pitch,format,palette,source,destination)")
    CHECK_LOCK("DirectXConsole::save(pixels,width,height,pitch,format,palette,source,destination)")

    // lock console
    void *console_pixels = lock();

    try
    {
        // clip source and destination areas
        Area clipped_source,clipped_destination;
        Clipper::clip(source,clip(),clipped_source,destination,Area(0,0,width,height),clipped_destination);

        // request format conversion
        m_copy.request(this->format(),format);
    
        // set copy palettes
        m_copy.palette(this->palette(),palette);

        // copy console pixels to 'pixels' buffer
        m_copy.copy(console_pixels,clipped_source.left(),clipped_source.top(),clipped_source.width(),clipped_source.height(),this->pitch(),
                    pixels,clipped_destination.left(),clipped_destination.top(),clipped_destination.width(),clipped_destination.height(),pitch);
    }
    catch (Error &error)
    {
        // unlock
        unlock();

        // error message
        throw Error("failed to save console area pixels",error);
    }
}




void PTCAPI DirectXConsole::clear()
{
    // debug check
    CHECK_OPEN("DirectXConsole::clear()")
    CHECK_LOCK("DirectXConsole::clear()")
    
    // check console format
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


void PTCAPI DirectXConsole::clear(const Color &color)
{
    // debug check
    CHECK_OPEN("DirectXConsole::clear(color)")
    CHECK_LOCK("DirectXConsole::clear(color)")

    // clear console
    clear(color,area());
}


void PTCAPI DirectXConsole::clear(const Color &color,const Area &area)
{
    // debug check
    CHECK_OPEN("DirectXConsole::clear(color,area)")
    CHECK_LOCK("DirectXConsole::clear(color,area)")

    // clear primary surface
    m_primary.clear(color,area);
}




void PTCAPI DirectXConsole::palette(const Palette &palette)
{
    // debug check
    CHECK_OPEN("DirectXConsole::palette(palette)")
    
    // set primary surface palette
    m_primary.palette(palette);
}


const Palette& PTCAPI DirectXConsole::palette() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::palette()")
    
    // get primary surface palette
    return m_primary.palette();
}




void PTCAPI DirectXConsole::clip(const Area &area)
{
    // debug check
    CHECK_OPEN("DirectXConsole::clip(area)")

    // set clip area
    m_primary.clip(area);
}




int PTCAPI DirectXConsole::width() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::width()")
    
    // get primary width
    return m_primary.width();
}


int PTCAPI DirectXConsole::height() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::height()")
    
    // get primary height
    return m_primary.height();
}


int PTCAPI DirectXConsole::pages() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::pages()")
    
    // get primary pages
    return m_primary.pages();
}


int PTCAPI DirectXConsole::pitch() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::pitch()")
    
    // get primary pitch
    return m_primary.pitch();
}


const Area& PTCAPI DirectXConsole::area() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::area()")
    
    // get primary area
    return m_primary.area();
}


const Area& PTCAPI DirectXConsole::clip() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::clip()")
    
    // get primary clip
    return m_primary.clip();
}


const Format& PTCAPI DirectXConsole::format() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::format()")
    
    // get primary format
    return m_primary.format();
}


const char* PTCAPI DirectXConsole::name() const
{
    // get name
    return "DirectX";
}


const char* PTCAPI DirectXConsole::title() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::title()")
    
    // get title
    return m_title;
}


const char* PTCAPI DirectXConsole::information() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::information()")
    
    // get information
    return m_display.information();
}




#ifdef __PTC_WIN32_EXTENSIONS__


void PTCAPI DirectXConsole::open(HWND window,int pages)
{
    // open console
    open(window,m_default_format,pages);
}


void PTCAPI DirectXConsole::open(HWND window,const Format &format,int pages)
{
    // open console
    open(window,m_default_width,m_default_height,m_default_format,pages);
}


void PTCAPI DirectXConsole::open(HWND window,int width,int height,const Format &format,int pages)
{
    // internal open nearest mode
    internal_open("",window,Mode(width,height,format),pages,false);
}


void PTCAPI DirectXConsole::open(HWND window,const Mode &mode,int pages)
{
    // internal open exact mode
    internal_open("",window,mode,pages,true);
}


HWND PTCAPI DirectXConsole::window() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::window()")
    
    // get window handle
    return m_window->handle();
}


LPDIRECTDRAW PTCAPI DirectXConsole::lpDD() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::lpDD()")

    // get directdraw interface
    return m_library.lpDD();
}



LPDIRECTDRAW2 PTCAPI DirectXConsole::lpDD2() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::lpDD2()")

    // get directdraw 2 interface
    return m_library.lpDD2();
}



LPDIRECTDRAWSURFACE PTCAPI DirectXConsole::lpDDS() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::lpDDS()")

    // get directdraw surface interface
    return m_primary.lpDDS();
}


LPDIRECTDRAWSURFACE PTCAPI DirectXConsole::lpDDS_primary() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::lpDDS_primary()")

    // get directdraw primary surface interface
    return m_primary.lpDDS_primary();
}


LPDIRECTDRAWSURFACE PTCAPI DirectXConsole::lpDDS_secondary() const
{
    // debug check
    CHECK_OPEN("DirectXConsole::lpDDS_secondary()")

    // get directdraw secondary surface interface
    return m_primary.lpDDS_secondary();
}


#endif




void DirectXConsole::internal_open(const char title[],HWND window,const Mode &mode,int pages,bool exact)
{
    try
    {
        // recycle an already open console
        internal_recycle(title,window,mode,pages,exact);

        // success
        return;
    }
    catch (Error&)
    {
        // could not recycle
    }

    // check that the mode is valid
    if (!mode.valid()) throw Error("invalid mode");

    // get mode information
    int width = mode.width();
    int height = mode.height();
    Format format = mode.format();

    // start internal open
    internal_open_start(title,window);

    // check output mode
    switch (m_output_mode)
    {
        case DEFAULT:
        {
            try
            {
                // start fullscreen open
                internal_open_fullscreen_start(window,mode,exact);
                
                // change fullscreen display
                internal_open_fullscreen_change(mode,exact);

                // setup fullscreen display surfaces
                internal_open_fullscreen_surface(mode,pages);

                // finish fullscreen open
                internal_open_fullscreen_finish();
            }
            catch (Error&)
            {
                // internal open reset
                internal_open_reset();

                // start windowed open
                internal_open_windowed_start(window,mode,exact);
                
                // change windowed display mode
                internal_open_windowed_change(mode,exact);

                // setup windowed display
                internal_open_windowed_surface(mode,pages);

                // finish windowed open
                internal_open_windowed_finish();
            }
        }
        break;

        case WINDOWED:
        {
            // start windowed open
            internal_open_windowed_start(window,mode,exact);
            
            // change windowed display mode
            internal_open_windowed_change(mode,exact);

            // setup windowed display
            internal_open_windowed_surface(mode,pages);

            // finish windowed open
            internal_open_windowed_finish();
        }
        break;

        case FULLSCREEN:
        {
            // start fullscreen open
            internal_open_fullscreen_start(window,mode,exact);
            
            // change fullscreen display
            internal_open_fullscreen_change(mode,exact);

            // setup fullscreen display surfaces
            internal_open_fullscreen_surface(mode,pages);

            // finish fullscreen open
            internal_open_fullscreen_finish();
        }
        break;
    }

    // finish internal open
    internal_open_finish();
}


void DirectXConsole::internal_recycle(const char title[],HWND window,const Mode &mode,int pages,bool exact)
{
    // check if the console is open
    if (!m_open) throw Error("cannot recycle because console is not already open");

    // check window
    if (window!=0)
    {
        // check if existing window matches new one
        if (m_window->handle()!=window && !m_window->managed()) throw Error("cannot recycle with this user window");
    }

    // check output mode
    switch (m_output_mode)
    {
        case DEFAULT:
        {
            // check existing output mode
            if (m_display.fullscreen())
            {   
                try
                {
                    // recycle fullscreen
                    internal_recycle_fullscreen(title,window,mode,pages,exact);
                }
                catch (Error&)
                {
                    // recycle fullscreen -> windowed ?
                    throw Error("recycling fullscreen to windowed is not implemented");
                }
            }
            else
            {
                // recycle windowed -> fullscreen first, then windowed ?
                throw Error("recycling windowed to fullscreen is not implemented");
            }
        }
        break;

        case FULLSCREEN:
        {
            // recycle fullscreen
            internal_recycle_fullscreen(title,window,mode,pages,exact);
        }
        break;

        case WINDOWED:
        {
            // recycle windowed
            internal_recycle_windowed(title,window,mode,pages,exact);
        }
        break;
    }
}

    
void DirectXConsole::internal_close()
{
    // clear flags
    m_open = false;

    // delete keyboard
    delete m_keyboard;

    // delete hook
    delete m_hook;

    // close primary
    m_primary.close();

    // close display
    m_display.close();

    // delete window
    delete m_window;

    // restore display
    m_display.restore();

    // clear pointers
    m_keyboard = 0;
    m_window = 0;
    m_hook = 0;
}


void DirectXConsole::internal_shutdown()
{
    // close library
    m_library.close();
}




void DirectXConsole::internal_open_start(const char title[],HWND window)
{
    // close down
    internal_close();
    
    // check window
    if (!window)
    {
        // check length of console title
        if (strlen(title)+1<=sizeof(m_title))
        {
            // update console title
            strcpy(m_title,title);
        }
        else
        {
            // update console title truncated
            memcpy(m_title,title,sizeof(m_title));
            m_title[sizeof(m_title)-1] = 0;
        }
    }
    else
    {
        // get existing window title
        GetWindowText(window,m_title,sizeof m_title);
    }
}


void DirectXConsole::internal_open_finish()
{
    // create win32 keyboard
    m_keyboard = new Win32Keyboard(m_window->handle(),m_window->thread(),false);

    // update window
    m_window->update();

    // set open flag
    m_open = true;
}


void DirectXConsole::internal_open_reset()
{
    // delete keyboard
    delete m_keyboard;

    // delete hook
    delete m_hook;

    // close primary
    m_primary.close();

    // delete window
    delete m_window;

    // restore display
    m_display.restore();

    // clear pointers
    m_keyboard = 0;
    m_window = 0;
    m_hook = 0;
}




void DirectXConsole::internal_open_fullscreen_start(HWND window,const Mode &mode,bool exact)
{
    // test if display mode exists...
    if (!m_display.test(mode,exact)) throw Error("display mode test failed!");

    // handle cursor show mode
    if (m_cursor_mode==CURSOR_SHOW) m_cursor = true;
    else m_cursor = false;

    // save display
    m_display.save();

    // check window
    if (!window)
    {
        // create fullscreen window
        m_window = new Win32Window("PTC_DIRECTX_FULLSCREEN",m_title,WS_EX_TOPMOST,WS_POPUP|WS_SYSMENU|WS_VISIBLE,SW_NORMAL,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),false,false);
    }
    else
    {
        // use existing window
        m_window = new Win32Window(window);
    }
    
    // set window cursor
    m_window->cursor(m_cursor);

    // set cooperative level
    m_display.cooperative(m_window->handle(),true);
}


void DirectXConsole::internal_open_fullscreen_change(const Mode &mode,bool exact)
{
    // open display mode
    m_display.open(mode,exact,m_frequency);

    // enable primary blocking
    m_primary.blocking(true);
}


void DirectXConsole::internal_open_fullscreen_surface(const Mode &mode,int pages)
{
    // determine if we need to create a secondary surface
    const bool secondary = m_primary_mode_fullscreen==SECONDARY || mode!=m_display.mode();

    // determine if the primary has a palette
    const bool palette = m_display.mode().format().indexed();

    // initialize primary surface
    m_primary.initialize(*m_window,m_library.lpDD2());

    // complex flag
    bool complex = false;

    // primary flag
    bool primary = false;

    // randy heit's primary setup
    while (!primary && !complex)
    {
        // check pages
        if (pages>=1)
        {
            try
            {
                // n buffered video memory
                m_primary.primary(pages,true,true,palette,complex);

                // success
                primary = true;
            }
            catch (Error&)
            {
                // failure
            }
        }

        // check primary
        if (!primary)
        {
            try
            {
                // triple buffered video memory
                m_primary.primary(3,true,true,palette,complex);
                
                // set primary
                primary = true;
            }
            catch (Error&)
            {
                try
                {
                    // double buffered video memory
                    m_primary.primary(2,true,true,palette,complex);
                    
                    // set primary
                    primary = true;
                }
                catch (Error&)
                {
                    try
                    {
                        // check secondary
                        if (!secondary)
                        {
                            // double buffered video or system memory
                            m_primary.primary(2,false,true,palette,complex);
                        }
                        else
                        {
                            // single buffered video or system memory
                            m_primary.primary(1,false,true,palette,complex);
                        }

                        // set primary
                        primary = true;
                    }
                    catch (Error&)
                    {
                        // invert complex
                        complex = !complex;
                    }
                }
            }
        }
    }

    // check secondary
    if (secondary)
    {
        // create secondary surface
        m_primary.secondary(mode.width(),mode.height());
    }

    // check nearest mode
    if (m_nearest_mode==NEAREST_CENTERING) m_primary.centering(true);
    else if (m_nearest_mode==NEAREST_STRETCHING) m_primary.centering(false);

    /*

    // original primary setup code (1.0.17)
  
    // check pages
    if (pages>=1)
    {
        try
        {
            // n buffered video memory
            m_primary.primary(pages,true,true,palette);

            // success
            primary = true;
        }
        catch (Error&)
        {
            // failure
        }
    }

    // check primary
    if (!primary)
    {
        try
        {
            // triple buffered video memory
            m_primary.primary(3,true,true,palette);
        }
        catch (Error&)
        {
            try
            {
                // double buffered video memory
                m_primary.primary(2,true,true,palette);
            }
            catch (Error&)
            {
                // check secondary
                if (!secondary)
                {
                    // double buffered video or system memory
                    m_primary.primary(2,false,true,palette);
                }
                else
                {
                    // single buffered video or system memory
                    m_primary.primary(1,false,true,palette);
                }
            }
        }
    }

    // check secondary
    if (secondary)
    {
        // create secondary surface
        m_primary.secondary(mode.width(),mode.height());
    }

    // check nearest mode
    if (m_nearest_mode==NEAREST_CENTERING) m_primary.centering(true);
    else if (m_nearest_mode==NEAREST_STRETCHING) m_primary.centering(false);
    */

    // setup primary syncronization
    m_primary.synchronize(m_synchronized_update);
}


void DirectXConsole::internal_open_fullscreen_finish()
{
    // create hook on window
    m_hook = new DirectXHook(*this,m_window->handle(),GetCurrentThreadId(),m_cursor,m_window->managed(),true);
}




void DirectXConsole::internal_open_windowed_start(HWND window,const Mode &mode,bool exact)
{
    // handle cursor show mode
    if (m_cursor_mode==CURSOR_HIDE) m_cursor = false;
    else m_cursor = true;

    // check window
    if (window)
    {
        // use existing window
        m_window = new Win32Window(window);
    }
    else
    {    
        // extended style
        int extended = 0;

        // make the window topmost if direct primary writing is setup
        if (m_primary_mode_windowed==DIRECT) extended = WS_EX_TOPMOST;

        // check window mode
        switch (m_window_mode)
        {
            case RESIZABLE:
            {
                // create resizable window
                m_window = new Win32Window("PTC_DIRECTX_WINDOWED_RESIZABLE",m_title,extended,WS_OVERLAPPEDWINDOW|WS_VISIBLE,SW_NORMAL,CW_USEDEFAULT,CW_USEDEFAULT,mode.width(),mode.height(),m_center_window,false);
            }
            break;

            case FIXED:
            {
                // create fixed window
                m_window = new Win32Window("PTC_DIRECTX_WINDOWED_FIXED",m_title,extended,WS_VISIBLE|WS_SYSMENU|WS_CAPTION|WS_MINIMIZE,SW_NORMAL,CW_USEDEFAULT,CW_USEDEFAULT,mode.width(),mode.height(),m_center_window,false);
            }
            break;
        }
    }

    // set window cursor
    m_window->cursor(m_cursor);

    // set cooperative level for windowed output
    m_display.cooperative(m_window->handle(),false);
}


void DirectXConsole::internal_open_windowed_change(const Mode &mode,bool exact)
{
    // open windowed display
    m_display.open();

    // configure primary blocking
    if (m_primary_mode_windowed==DIRECT)
    {
        // enable blocking
        m_primary.blocking(true);
    }
    else
    {
        // disable blocking
        m_primary.blocking(false);
    }
}


void DirectXConsole::internal_open_windowed_surface(const Mode &mode,int pages)
{
    // initialize primary surface
    m_primary.initialize(*m_window,m_library.lpDD2());

    // create a single paged primary surface
    m_primary.primary(1,false,false,false,false);

    // check for secondary primary mode
    if (m_primary_mode_windowed==SECONDARY)
    {
        // setup secondary surface
        m_primary.secondary(mode.width(),mode.height());
    }
}

void DirectXConsole::internal_open_windowed_finish()
{
    // create hook on window
    m_hook = new DirectXHook(*this,m_window->handle(),GetCurrentThreadId(),m_cursor,m_window->managed(),false);
}




void DirectXConsole::internal_recycle_fullscreen(const char title[],HWND window,const Mode &mode,int pages,bool exact)
{
    // log fullscreen open recycle
    LOG("fullscreen open recycle");

    // close primary
    m_primary.close();

    // change display mode
    internal_open_fullscreen_change(mode,exact);
    
    // setup display surfaces again
    internal_open_fullscreen_surface(mode,pages);
}


void DirectXConsole::internal_recycle_windowed(const char title[],HWND window,const Mode &mode,int pages,bool exact)
{
    // log windowed open recycle
    LOG("fullscreen open recycle");

    // close primary
    m_primary.close();

    // resize window to new mode dimensions
    m_window->resize(mode.width(),mode.height());

    // change windowed output mode
    internal_open_windowed_change(mode,exact);
    
    // setup display surfaces again
    internal_open_windowed_surface(mode,pages);
}




void DirectXConsole::paint()
{
    // no painting when locked or not open
    if (m_locked || !m_open) return;

    // paint primary
    m_primary.paint();
}




void DirectXConsole::update_cursor()
{
    // we must be open
    if (!m_open) return;

    // check if we are fullscreen
    if (m_display.fullscreen())
    {
        // fullscreen cursor handling
        if (m_cursor_mode==CURSOR_SHOW) m_cursor = true;
        else m_cursor = false;
    }
    else
    {
        // windowed cursor handling
        if (m_cursor_mode==CURSOR_HIDE) m_cursor = false;
        else m_cursor = true;
    }

    // update hook cursor
    m_hook->cursor(m_cursor);

    // update window cursor
    m_window->cursor(m_cursor);
}
