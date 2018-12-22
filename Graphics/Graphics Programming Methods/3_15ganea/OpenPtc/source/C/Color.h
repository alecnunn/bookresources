/*
 * C Color class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_COLOR_H
#define __PTC_C_COLOR_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"


      

/* setup */
CAPI PTC_COLOR PTCAPI ptc_color_create();
CAPI PTC_COLOR PTCAPI ptc_color_create_indexed(int index);
CAPI PTC_COLOR PTCAPI ptc_color_create_direct(float r,float g,float b,float a);
CAPI void PTCAPI ptc_color_destroy(PTC_COLOR object);

/* data access */
CAPI int PTCAPI ptc_color_index(PTC_COLOR object);
CAPI float PTCAPI ptc_color_r(PTC_COLOR object);
CAPI float PTCAPI ptc_color_g(PTC_COLOR object);
CAPI float PTCAPI ptc_color_b(PTC_COLOR object);
CAPI float PTCAPI ptc_color_a(PTC_COLOR object);
CAPI int PTCAPI ptc_color_direct(PTC_COLOR object);
CAPI int PTCAPI ptc_color_indexed(PTC_COLOR object);

/* operators */
CAPI void PTCAPI ptc_color_assign(PTC_COLOR object,PTC_COLOR color);
CAPI int PTCAPI ptc_color_equals(PTC_COLOR object,PTC_COLOR color);




#endif
