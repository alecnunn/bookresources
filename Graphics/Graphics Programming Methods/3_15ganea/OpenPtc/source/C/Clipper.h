/*
 * C Clipper class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_CLIPPER_H
#define __PTC_C_CLIPPER_H

/* include files */
#include "config.h"
#include "C/Index.h"




/* clip a single area against clip area */
CAPI void PTCAPI ptc_clipper_clip(PTC_AREA area,PTC_AREA clip,PTC_AREA clipped);

/* clip source and destination areas against source and destination clip areas */
CAPI void PTCAPI ptc_clipper_clip_complex(PTC_AREA source,PTC_AREA clip_source,PTC_AREA clipped_source,
                                          PTC_AREA destination,PTC_AREA clip_destination,PTC_AREA clipped_destination);




#endif
