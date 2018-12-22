/*
 * C Surface class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_SURFACE_H
#define __PTC_C_SURFACE_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_SURFACE PTCAPI ptc_surface_create(int width,int height,PTC_FORMAT format);
CAPI void PTCAPI ptc_surface_destroy(PTC_SURFACE object);

/* copy to surface */
CAPI void PTCAPI ptc_surface_copy(PTC_SURFACE object,PTC_SURFACE surface);
CAPI void PTCAPI ptc_surface_copy_area(PTC_SURFACE object,PTC_SURFACE surface,PTC_AREA source,PTC_AREA destination);

/* memory access */
CAPI void* PTCAPI ptc_surface_lock(PTC_SURFACE object);
CAPI void PTCAPI ptc_surface_unlock(PTC_SURFACE object);

/* load pixels to surface */
CAPI void PTCAPI ptc_surface_load(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette);
CAPI void PTCAPI ptc_surface_load_area(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination);

/* save surface pixels */
CAPI void PTCAPI ptc_surface_save(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette);
CAPI void PTCAPI ptc_surface_save_area(PTC_SURFACE object,void *pixels,int width,int height,int pitch,PTC_FORMAT format,PTC_PALETTE palette,PTC_AREA source,PTC_AREA destination);

/* clear surface */
CAPI void PTCAPI ptc_surface_clear(PTC_SURFACE object);
CAPI void PTCAPI ptc_surface_clear_color(PTC_SURFACE object,PTC_COLOR color);
CAPI void PTCAPI ptc_surface_clear_color_area(PTC_SURFACE object,PTC_COLOR color,PTC_AREA area);

/* surface palette */
CAPI void PTCAPI ptc_surface_palette_set(PTC_SURFACE object,PTC_PALETTE palette);
CAPI PTC_PALETTE PTCAPI ptc_surface_palette_get(PTC_SURFACE object);

/* surface clip area */
CAPI void PTCAPI ptc_surface_clip_set(PTC_SURFACE object,PTC_AREA area);

/* data access */
CAPI int PTCAPI ptc_surface_width(PTC_SURFACE object);
CAPI int PTCAPI ptc_surface_height(PTC_SURFACE object);
CAPI int PTCAPI ptc_surface_pitch(PTC_SURFACE object);
CAPI PTC_AREA PTCAPI ptc_surface_area(PTC_SURFACE object);
CAPI PTC_AREA PTCAPI ptc_surface_clip(PTC_SURFACE object);
CAPI PTC_FORMAT PTCAPI ptc_surface_format(PTC_SURFACE object);

/* surface option string */
CAPI int PTCAPI ptc_surface_option(PTC_SURFACE object,char *option);




#endif
