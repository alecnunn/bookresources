/*
 * C Format class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_FORMAT_H
#define __PTC_C_FORMAT_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_FORMAT PTCAPI ptc_format_create();
CAPI PTC_FORMAT PTCAPI ptc_format_create_indexed(int bits);
CAPI PTC_FORMAT PTCAPI ptc_format_create_direct(int bits,ptc_int32 r,ptc_int32 g,ptc_int32 b,ptc_int32 a);
CAPI void PTCAPI ptc_format_destroy(PTC_FORMAT object);

/* data access */
CAPI ptc_int32 PTCAPI ptc_format_r(PTC_FORMAT object);
CAPI ptc_int32 PTCAPI ptc_format_g(PTC_FORMAT object);
CAPI ptc_int32 PTCAPI ptc_format_b(PTC_FORMAT object);
CAPI ptc_int32 PTCAPI ptc_format_a(PTC_FORMAT object);
CAPI int PTCAPI ptc_format_bits(PTC_FORMAT object);
CAPI int PTCAPI ptc_format_bytes(PTC_FORMAT object);
CAPI int PTCAPI ptc_format_direct(PTC_FORMAT object);
CAPI int PTCAPI ptc_format_indexed(PTC_FORMAT object);

/* operators */
CAPI void PTCAPI ptc_format_assign(PTC_FORMAT object,PTC_FORMAT format);
CAPI int PTCAPI ptc_format_equals(PTC_FORMAT object,PTC_FORMAT format);




#endif
