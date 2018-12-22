/*
 * C Copy class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org) 
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_COPY_H
#define __PTC_C_COPY_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_COPY PTCAPI ptc_copy_create();
CAPI void PTCAPI ptc_copy_destroy(PTC_COPY object);

/* set source and destination formats */
CAPI void PTCAPI ptc_copy_request(PTC_COPY object,PTC_FORMAT source,PTC_FORMAT destination);

/* set source and destination palettes */
CAPI void PTCAPI ptc_copy_palette(PTC_COPY object,PTC_PALETTE source,PTC_PALETTE destination);

/* copy pixels */
CAPI void PTCAPI ptc_copy_copy(PTC_COPY object,void *source_pixels,int source_x,int source_y,int source_width,int source_height,int source_pitch,
                               void *destination_pixels,int destination_x,int destination_y,int destination_width,int destination_height,int destination_pitch);

/* copy option string */
CAPI int PTCAPI ptc_copy_option(PTC_COPY object,char *option);




#endif
