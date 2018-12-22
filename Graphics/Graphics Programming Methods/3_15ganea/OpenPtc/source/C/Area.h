/*
 * C Area class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_AREA_H
#define __PTC_C_AREA_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_AREA PTCAPI ptc_area_create(int left,int top,int right,int bottom);
CAPI void PTCAPI ptc_area_destroy(PTC_AREA object);

/* data access */
CAPI int PTCAPI ptc_area_left(PTC_AREA object);
CAPI int PTCAPI ptc_area_right(PTC_AREA object);
CAPI int PTCAPI ptc_area_top(PTC_AREA object);
CAPI int PTCAPI ptc_area_bottom(PTC_AREA object);
CAPI int PTCAPI ptc_area_width(PTC_AREA object);
CAPI int PTCAPI ptc_area_height(PTC_AREA object);

/* operators */
CAPI void PTCAPI ptc_area_assign(PTC_AREA object,PTC_AREA area);
CAPI int PTCAPI ptc_area_equals(PTC_AREA object,PTC_AREA area);




#endif
