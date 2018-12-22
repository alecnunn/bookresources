//
// C Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Except.h"
#include "C/Console.h"
#include "Core/Key.h"
#include "Core/Console.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_CONSOLE PTCAPI ptc_console_create()
{
    try
    {
        // create console object
        return (PTC_CONSOLE) new Console();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_console_destroy(PTC_CONSOLE object)
{
    try
    {
        // destroy console object
        delete (BaseConsole*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_console_configure(PTC_CONSOLE object,char *file)
{
    try
    {
        // configure console
        ((BaseConsole*)object)->configure(file);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}



CAPI int PTCAPI ptc_console_option(PTC_CONSOLE object,char *option)
{
    try
    {
        // console option
        return (int) ((BaseConsole*)object)->option(option);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI PTC_MODE PTCAPI ptc_console_mode(PTC_CONSOLE object,int index)
{
    try
    {
        // get console modes
        const Mode *modes = ((BaseConsole*)object)->modes();

        // return mode at index
        return (PTC_MODE) &modes[index];
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_console_open(PTC_CONSOLE object,char *title,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(title,pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_open_format(PTC_CONSOLE object,char *title,PTC_FORMAT format,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(title,*((Format*)format),pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_open_resolution(PTC_CONSOLE object,char *title,int width,int height,PTC_FORMAT format,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(title,width,height,*((Format*)format),pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_open_mode(PTC_CONSOLE object,char *title,PTC_MODE mode,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(title,*((Mode*)mode),pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_close(PTC_CONSOLE object)
{
    try
    {
        // close console
        ((BaseConsole*)object)->close();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}
       



CAPI void PTCAPI ptc_console_flush(PTC_CONSOLE object)
{
    try
    {
        // flush console
        ((BaseConsole*)object)->flush();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_finish(PTC_CONSOLE object)
{
    try
    {
        // finish console
        ((BaseConsole*)object)->finish();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_update(PTC_CONSOLE object)
{
    try
    {
        // update console
        ((BaseConsole*)object)->update();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_update_area(PTC_CONSOLE object,PTC_AREA area)
{
    try
    {
        // update console area
        ((BaseConsole*)object)->update(*((Area*)area));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_console_key(PTC_CONSOLE object)
{
    try
    {
        // check for key press
        return (int) ((BaseConsole*)object)->key();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_console_read(PTC_CONSOLE object,PTC_KEY key)
{
    try
    {
        // read key press
        *((Key*)key) = ((BaseConsole*)object)->read();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_console_copy(PTC_CONSOLE object,PTC_SURFACE surface)
{
    try
    {
        // copy console to surface
        ((BaseConsole*)object)->copy(*((BaseSurface*)surface));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_copy_area(PTC_CONSOLE object,PTC_SURFACE surface,PTC_AREA source,PTC_AREA destination)
{
    try
    {
        // copy console area to surface area
        ((BaseConsole*)object)->copy(*((BaseSurface*)surface),*((Area*)source),*((Area*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void* PTCAPI ptc_console_lock(PTC_CONSOLE object)
{
    try
    {
        // lock console
        return ((BaseConsole*)object)->lock();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_console_unlock(PTC_CONSOLE object)
{
    try
    {
        // unlock console
        ((BaseConsole*)object)->unlock();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_console_load(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette)
{
    try
    {
        // load pixels to console
        ((BaseConsole*)object)->load(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}                  


CAPI void PTCAPI ptc_console_load_area(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination)
{
    try
    {
        // load pixels to console area
        ((BaseConsole*)object)->load(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette),*((Area*)source),*((Area*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_console_save(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette)
{
    try
    {
        // save console pixels
        ((BaseConsole*)object)->save(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_save_area(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination)
{
    try
    {
        // save console area pixels
        ((BaseConsole*)object)->save(pixels,width,height,pitch,*((Format*)format),*((Palette*)palette),*((Area*)source),*((Area*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_console_clear(PTC_CONSOLE object)
{
    try
    {
        // clear console
        ((BaseConsole*)object)->clear();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_clear_color(PTC_CONSOLE object,PTC_COLOR color)
{
    try
    {
        // clear console color
        ((BaseConsole*)object)->clear(*((Color*)color));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_clear_color_area(PTC_CONSOLE object,PTC_COLOR color,PTC_AREA area)
{
    try
    {
        // clear console color area
        ((BaseConsole*)object)->clear(*((Color*)color),*((Area*)area));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_console_palette_set(PTC_CONSOLE object,PTC_PALETTE palette)
{
    try
    {
        // set console palette
        ((BaseConsole*)object)->palette(*((Palette*)palette));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI PTC_PALETTE PTCAPI ptc_console_palette_get(PTC_CONSOLE object)
{
    try
    {
        // get console palette
        return (PTC_PALETTE) & ((BaseConsole*)object)->palette();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_console_clip_set(PTC_CONSOLE object,PTC_AREA area)
{
    try
    {
        // set console clip area
        ((BaseConsole*)object)->clip(*((Area*)area));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_console_width(PTC_CONSOLE object)
{
    try
    {
        // get console width
        return ((BaseConsole*)object)->width();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_console_height(PTC_CONSOLE object)
{
    try
    {
        // get console height
        return ((BaseConsole*)object)->height();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_console_pages(PTC_CONSOLE object)
{
    try
    {
        // get console pages
        return ((BaseConsole*)object)->pages();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_console_pitch(PTC_CONSOLE object)
{
    try
    {
        // get console pitch
        return ((BaseConsole*)object)->pitch();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_AREA PTCAPI ptc_console_area(PTC_CONSOLE object)
{
    try
    {
        // get console area
        return (PTC_AREA) & ((BaseConsole*)object)->area();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_AREA PTCAPI ptc_console_clip(PTC_CONSOLE object)
{
    try
    {
        // get console clip
        return (PTC_AREA) & ((BaseConsole*)object)->clip();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_FORMAT PTCAPI ptc_console_format(PTC_CONSOLE object)
{
    try
    {
        // get console format
        return (PTC_FORMAT) & ((BaseConsole*)object)->format();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI char* PTCAPI ptc_console_name(PTC_CONSOLE object)
{
    try
    {
        // get console name
        return (char*) ((BaseConsole*)object)->name();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI char* PTCAPI ptc_console_title(PTC_CONSOLE object)
{
    try
    {
        // get console title
        return (char*) ((BaseConsole*)object)->title();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI char* PTCAPI ptc_console_information(PTC_CONSOLE object)
{
    try
    {
        // get console information
        return (char*) ((BaseConsole*)object)->information();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




#ifdef __PTC_WIN32_EXTENSIONS__


CAPI void PTCAPI ptc_console_open_window(PTC_CONSOLE object,HWND window,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(window,pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_open_window_format(PTC_CONSOLE object,HWND window,PTC_FORMAT format,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(window,*((Format*)format),pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_open_window_resolution(PTC_CONSOLE object,HWND window,int width,int height,PTC_FORMAT format,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(window,width,height,*((Format*)format),pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_console_open_window_mode(PTC_CONSOLE object,HWND window,PTC_MODE mode,int pages)
{
    try
    {
        // open console
        ((BaseConsole*)object)->open(window,*((Mode*)mode),pages);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI HWND PTCAPI ptc_console_window(PTC_CONSOLE object)
{
    try
    {
        // get console window
        return ((BaseConsole*)object)->window();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI LPDIRECTDRAW PTCAPI ptc_console_lpDD(PTC_CONSOLE object)
{
    try
    {
        // get console lpDD
        return ((BaseConsole*)object)->lpDD();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI LPDIRECTDRAW2 PTCAPI ptc_console_lpDD2(PTC_CONSOLE object)
{
    try
    {
        // get console lpDD2
        return ((BaseConsole*)object)->lpDD2();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI LPDIRECTDRAWSURFACE PTCAPI ptc_console_lpDDS(PTC_CONSOLE object)
{
    try
    {
        // get console lpDDS
        return ((BaseConsole*)object)->lpDDS();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI LPDIRECTDRAWSURFACE PTCAPI ptc_console_lpDDS_primary(PTC_CONSOLE object)
{
    try
    {
        // get console lpDDS primary
        return ((BaseConsole*)object)->lpDDS_primary();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI LPDIRECTDRAWSURFACE PTCAPI ptc_console_lpDDS_secondary(PTC_CONSOLE object)
{
    try
    {
        // get console lpDDS secondary
        return ((BaseConsole*)object)->lpDDS_secondary();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


#endif
