/*
 * C Clear class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_CLEAR_H
#define __PTC_C_CLEAR_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_CLEAR PTCAPI ptc_clear_create();
CAPI void PTCAPI ptc_clear_destroy(PTC_CLEAR object);

/* request clear */
CAPI void PTCAPI ptc_clear_request(PTC_CLEAR object,PTC_FORMAT format);

/* clear pixels */
CAPI void PTCAPI ptc_clear_clear(PTC_CLEAR object,void *pixels,int x,int y,int width,int height,int pitch,PTC_COLOR color);




#endif
