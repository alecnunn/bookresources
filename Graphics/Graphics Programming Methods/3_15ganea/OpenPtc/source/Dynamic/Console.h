//
// Dynamic Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_CONSOLE_H
#define __PTC_DYNAMIC_CONSOLE_H

// include files
#include "config.h"
#include "C/Extra.h"
#include "C/Console.h"
#include "Core/Types.h"
#include "Dynamic/Base.h"
#include "Dynamic/Copy.h"
#include "Dynamic/Mode.h"
#include "Dynamic/Clear.h"
#include "Dynamic/Format.h"
#include "Dynamic/Palette.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicConsole : public DynamicBaseConsole
{
    public:

        DynamicConsole()
        {
            // use exceptions
            ptc_use_exceptions();

            // create console object
            object = ptc_console_create();
        }

        ~DynamicConsole()
        {
            // destroy console object
            ptc_console_destroy(object);
        }
        
        void configure(const char file[])
        {
            // configure console
            ptc_console_configure(object,(char*)file);
        }

        bool option(const char option[])
        {
            // console option
            return (ptc_console_option(object,(char*)option)==1);
        }

        const DynamicMode* modes()
        {
            // mode index
            int index = 0;

            // iterate
            while (true)
            {
                // get console mode at index
                PTC_MODE mode = ptc_console_mode(object,index);

                // assign object data
                ptc_mode_assign(kludge_modes[index].object,mode);

                // stop iterating if mode is not valid
                if (ptc_mode_valid(mode)==0) break;

                // next mode
                index ++;
            }

            // finished kludge
            return kludge_modes;
        }
        
        void open(const char title[],int pages = 0)
        {
            // open console
            ptc_console_open(object,(char*)title,pages);
        }

        void open(const char title[],const DynamicFormat &format,int pages = 0)
        {
            // open console
            ptc_console_open_format(object,(char*)title,format.object,pages);
        }

        void open(const char title[],int width,int height,const DynamicFormat &format,int pages = 0)
        {
            // open console
            ptc_console_open_resolution(object,(char*)title,width,height,format.object,pages);
        }

        void open(const char title[],const DynamicMode &mode,int pages = 0)
        {
            // open console
            ptc_console_open_mode(object,(char*)title,mode.object,pages);
        }

        void close()
        {
            // close console
            ptc_console_close(object);
        }

        void flush()
        {
            // flush console
            ptc_console_flush(object);
        }
        
        void finish()
        {
            // finish console
            ptc_console_finish(object);
        }
        
        void update()
        {
            // update console
            ptc_console_update(object);
        }

        void update(const DynamicArea &area)
        {
            // update console area
            ptc_console_update_area(object,area.object);
        }
        
        bool key()
        {
            // check for key press
            return (ptc_console_key(object)==1);
        }

        DynamicKey read()
        {
            // create key
            DynamicKey key;

            // read key press
            ptc_console_read(object,key.object);

            // finished
            return key;
        }
        
        void copy(DynamicBaseSurface &surface)
        {
            // copy console to surface
            ptc_console_copy(object,surface.surface());
        }

        void copy(DynamicBaseSurface &surface,const DynamicArea &source,const DynamicArea &destination)
        {
            // copy console area to surface area
            ptc_console_copy_area(object,surface.surface(),source.object,destination.object);
        }

        void* lock()
        {
            // lock console
            return ptc_console_lock(object);
        }

        void unlock()
        {
            // unlock console
            ptc_console_unlock(object);
        }

        void load(const void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette)
        {
            // load pixels to console
            ptc_console_load(object,(void*)pixels,width,height,pitch,format.object,palette.object);
        }

        void load(const void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette,const DynamicArea &source,const DynamicArea &destination)
        {
            // load pixels to console area
            ptc_console_load_area(object,(void*)pixels,width,height,pitch,format.object,palette.object,source.object,destination.object);
        }
        
        void save(void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette)
        {
            // save console pixels
            ptc_console_save(object,pixels,width,height,pitch,format.object,palette.object);
        }

        void save(void *pixels,int width,int height,int pitch,const DynamicFormat &format,const DynamicPalette &palette,const DynamicArea &source,const DynamicArea &destination)
        {
            // save console area pixels
            ptc_console_save_area(object,pixels,width,height,pitch,format.object,palette.object,source.object,destination.object);
        }
        
        void clear()
        {
            // clear console
            ptc_console_clear(object);
        }

        void clear(const DynamicColor &color)
        {
            // clear console color
            ptc_console_clear_color(object,color.object);
        }

        void clear(const DynamicColor &color,const DynamicArea &area)
        {
            // clear console color area
            ptc_console_clear_color_area(object,color.object,area.object);
        }
        
        void palette(const DynamicPalette &palette)
        {
            // set console palette
            ptc_console_palette_set(object,palette.object);
        }

        const DynamicPalette& palette() const
        {
            // get console palette
            ptc_palette_assign(kludge_palette.object,ptc_console_palette_get(object));
            return kludge_palette;
        }
        
        void clip(const DynamicArea &area)
        {
            // set console clip area
            ptc_console_clip_set(object,area.object);
        }

        int width() const
        {
            // get console width
            return ptc_console_width(object);
        }

        int height() const
        {
            // get console height
            return ptc_console_height(object);
        }

        int pages() const
        {
            // get console pages
            return ptc_console_pages(object);
        }

        int pitch() const
        {
            // get console pitch
            return ptc_console_pitch(object);
        }

        const DynamicArea& area() const
        {
            // get console area
            ptc_area_assign(kludge_area.object,ptc_console_area(object));
            return kludge_area;
        }

        const DynamicArea& clip() const
        {
            // get console clip area
            ptc_area_assign(kludge_clip.object,ptc_console_clip(object));
            return kludge_clip;
        }

        const DynamicFormat& format() const
        {
            // get console format
            ptc_format_assign(kludge_format.object,ptc_console_format(object));
            return kludge_format;
        }

        const char* name() const
        {
            // get console name
            return ptc_console_name(object);
        }

        const char* title() const
        {
            // get console title
            return ptc_console_title(object);
        }

        const char* information() const
        {
            // get console information
            return ptc_console_information(object);
        }

#ifdef __PTC_WIN32_EXTENSIONS__

        void open(HWND window,int pages = 0)
        {
            // open console
            ptc_console_open_window(object,window,pages);
        }

        void open(HWND window,const DynamicFormat &format,int pages = 0)
        {
            // open console
            ptc_console_open_window_format(object,window,format.object,pages);
        }

        void open(HWND window,int width,int height,const DynamicFormat &format,int pages = 0)
        {
            // open console
            ptc_console_open_window_resolution(object,window,width,height,format.object,pages);
        }

        void open(HWND window,const DynamicMode &mode,int pages = 0)
        {
            // open console
            ptc_console_open_window_mode(object,window,mode.object,pages);
        }

        HWND window() const
        {
            // get window
            return ptc_console_window(object);
        }
        
        LPDIRECTDRAW lpDD() const
        {
            // get lpDD
            return ptc_console_lpDD(object);
        }
        
        LPDIRECTDRAW2 lpDD2() const
        {
            // get lpDD2
            return ptc_console_lpDD2(object);
        }

        LPDIRECTDRAWSURFACE lpDDS() const
        {
            // get lpDDS
            return ptc_console_lpDDS(object);
        }

        LPDIRECTDRAWSURFACE lpDDS_primary() const
        {
            // get lpDDS primary
            return ptc_console_lpDDS_primary(object);
        }

        LPDIRECTDRAWSURFACE lpDDS_secondary() const
        {
            // get lpDDS secondary
            return ptc_console_lpDDS_secondary(object);
        }

#endif

    private:

        // disallow copy construction
        DynamicConsole(const DynamicConsole &console);

        // disallow assignment
        DynamicConsole& operator =(const DynamicConsole &console);

        PTC_SURFACE surface()
        {
            // get console surface
            return (PTC_SURFACE) object;
        }

        // console object
        PTC_CONSOLE object;

        // kludge objects
        DynamicArea kludge_area;
        DynamicArea kludge_clip;
        DynamicFormat kludge_format;
        DynamicPalette kludge_palette;
        DynamicMode kludge_modes[256];
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
