/*
 * C Mode class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_MODE_H
#define __PTC_C_MODE_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_MODE PTCAPI ptc_mode_create(int width,int height,PTC_FORMAT format);
CAPI PTC_MODE PTCAPI ptc_mode_create_invalid();
CAPI void PTCAPI ptc_mode_destroy(PTC_MODE object);

/* valid mode flag */
CAPI int PTCAPI ptc_mode_valid(PTC_MODE object);

/* data access */
CAPI int PTCAPI ptc_mode_width(PTC_MODE object);
CAPI int PTCAPI ptc_mode_height(PTC_MODE object);
CAPI PTC_FORMAT PTCAPI ptc_mode_format(PTC_MODE object);

/* operators */
CAPI void PTCAPI ptc_mode_assign(PTC_MODE object,PTC_MODE mode);
CAPI int PTCAPI ptc_mode_equals(PTC_MODE object,PTC_MODE mode);




#endif
