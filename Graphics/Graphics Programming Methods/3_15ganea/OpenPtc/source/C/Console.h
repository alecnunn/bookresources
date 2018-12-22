/*
 * C Console class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_CONSOLE_H
#define __PTC_C_CONSOLE_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"
#ifdef __COMPILER_SUPPORTS_DDRAW__
#include <ddraw.h>
#else
#include <windows.h>
#endif




/* setup */
CAPI PTC_CONSOLE PTCAPI ptc_console_create();
CAPI void PTCAPI ptc_console_destroy(PTC_CONSOLE object);

/* console configuration */
CAPI void PTCAPI ptc_console_configure(PTC_CONSOLE object,char *file);

/* console option string */
CAPI int PTCAPI ptc_console_option(PTC_CONSOLE object,char *option);

/* console modes */
CAPI PTC_MODE PTCAPI ptc_console_mode(PTC_CONSOLE object,int index);

/* console management */
CAPI void PTCAPI ptc_console_open(PTC_CONSOLE object,char *title,int pages);
CAPI void PTCAPI ptc_console_open_format(PTC_CONSOLE object,char *title,PTC_FORMAT format,int pages);
CAPI void PTCAPI ptc_console_open_resolution(PTC_CONSOLE object,char *title,int width,int height,PTC_FORMAT format,int pages);
CAPI void PTCAPI ptc_console_open_mode(PTC_CONSOLE object,char *title,PTC_MODE mode,int pages);
CAPI void PTCAPI ptc_console_close(PTC_CONSOLE object);
       
/* synchronization */
CAPI void PTCAPI ptc_console_flush(PTC_CONSOLE object);
CAPI void PTCAPI ptc_console_finish(PTC_CONSOLE object);
CAPI void PTCAPI ptc_console_update(PTC_CONSOLE object);
CAPI void PTCAPI ptc_console_update_area(PTC_CONSOLE object,PTC_AREA area);

/* keyboard input */
CAPI int PTCAPI ptc_console_key(PTC_CONSOLE object);
CAPI void PTCAPI ptc_console_read(PTC_CONSOLE object,PTC_KEY key);

/* copy to surface */
CAPI void PTCAPI ptc_console_copy(PTC_CONSOLE object,PTC_SURFACE surface);
CAPI void PTCAPI ptc_console_copy_area(PTC_CONSOLE object,PTC_SURFACE surface,PTC_AREA source,PTC_AREA destination);

/* memory access */
CAPI void* PTCAPI ptc_console_lock(PTC_CONSOLE object);
CAPI void PTCAPI ptc_console_unlock(PTC_CONSOLE object);

/* load pixels to console */
CAPI void PTCAPI ptc_console_load(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette);
CAPI void PTCAPI ptc_console_load_area(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination);

/* save console pixels */
CAPI void PTCAPI ptc_console_save(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette);
CAPI void PTCAPI ptc_console_save_area(PTC_CONSOLE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination);

/* clear console */
CAPI void PTCAPI ptc_console_clear(PTC_CONSOLE object);
CAPI void PTCAPI ptc_console_clear_color(PTC_CONSOLE object,PTC_COLOR color);
CAPI void PTCAPI ptc_console_clear_color_area(PTC_CONSOLE object,PTC_COLOR color,PTC_AREA area);

/* console palette */
CAPI void PTCAPI ptc_console_palette_set(PTC_CONSOLE object,PTC_PALETTE palette);
CAPI PTC_PALETTE PTCAPI ptc_console_palette_get(PTC_CONSOLE object);

/* console clip area */
CAPI void PTCAPI ptc_console_clip_set(PTC_CONSOLE object,PTC_AREA area);

/* data access */
CAPI int PTCAPI ptc_console_width(PTC_CONSOLE object);
CAPI int PTCAPI ptc_console_height(PTC_CONSOLE object);
CAPI int PTCAPI ptc_console_pages(PTC_CONSOLE object);
CAPI int PTCAPI ptc_console_pitch(PTC_CONSOLE object);
CAPI PTC_AREA PTCAPI ptc_console_area(PTC_CONSOLE object);
CAPI PTC_AREA PTCAPI ptc_console_clip(PTC_CONSOLE object);
CAPI PTC_FORMAT PTCAPI ptc_console_format(PTC_CONSOLE object);
CAPI char* PTCAPI ptc_console_name(PTC_CONSOLE object);
CAPI char* PTCAPI ptc_console_title(PTC_CONSOLE object);
CAPI char* PTCAPI ptc_console_information(PTC_CONSOLE object);

/* extension functions */
#ifdef __PTC_WIN32_EXTENSIONS__
CAPI void PTCAPI ptc_console_open_window(PTC_CONSOLE object,HWND window,int pages);
CAPI void PTCAPI ptc_console_open_window_format(PTC_CONSOLE object,HWND window,PTC_FORMAT format,int pages);
CAPI void PTCAPI ptc_console_open_window_resolution(PTC_CONSOLE object,HWND window,int width,int height,PTC_FORMAT format,int pages);
CAPI void PTCAPI ptc_console_open_window_mode(PTC_CONSOLE object,HWND window,PTC_MODE mode,int pages);
CAPI HWND PTCAPI ptc_console_window(PTC_CONSOLE object);
CAPI LPDIRECTDRAW PTCAPI ptc_console_lpDD(PTC_CONSOLE object);
CAPI LPDIRECTDRAW2 PTCAPI ptc_console_lpDD2(PTC_CONSOLE object);
CAPI LPDIRECTDRAWSURFACE PTCAPI ptc_console_lpDDS(PTC_CONSOLE object);
CAPI LPDIRECTDRAWSURFACE PTCAPI ptc_console_lpDDS_primary(PTC_CONSOLE object);
CAPI LPDIRECTDRAWSURFACE PTCAPI ptc_console_lpDDS_secondary(PTC_CONSOLE object);
#endif




#endif
