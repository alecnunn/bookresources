/*
 * C Palette class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_PALETTE_H
#define __PTC_C_PALETTE_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_PALETTE PTCAPI ptc_palette_create();
CAPI PTC_PALETTE PTCAPI ptc_palette_create_data(ptc_int32 *data);
CAPI void PTCAPI ptc_palette_destroy(PTC_PALETTE object);

/* memory access */
CAPI ptc_int32* PTCAPI ptc_palette_lock(PTC_PALETTE object);
CAPI void PTCAPI ptc_palette_unlock(PTC_PALETTE object);

/* load palette data */
CAPI void PTCAPI ptc_palette_load(PTC_PALETTE object,ptc_int32 *data);

/* save palette data */
CAPI void PTCAPI ptc_palette_save(PTC_PALETTE object,ptc_int32 *data);

/* get palette data */
CAPI ptc_int32* PTCAPI ptc_palette_data(PTC_PALETTE object);

/* operators */
CAPI void PTCAPI ptc_palette_assign(PTC_PALETTE object,PTC_PALETTE palette);
CAPI int PTCAPI ptc_palette_equals(PTC_PALETTE object,PTC_PALETTE palette);




#endif
