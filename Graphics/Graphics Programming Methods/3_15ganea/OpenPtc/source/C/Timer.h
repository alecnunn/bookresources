/*
 * C Timer class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_TIMER_H
#define __PTC_C_TIMER_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_TIMER PTCAPI ptc_timer_create();
CAPI void PTCAPI ptc_timer_destroy(PTC_TIMER object);

/* set time */
CAPI void PTCAPI ptc_timer_set(PTC_TIMER object,double time);

/* control */
CAPI void PTCAPI ptc_timer_start(PTC_TIMER object);
CAPI void PTCAPI ptc_timer_stop(PTC_TIMER object);

/* time data */
CAPI double PTCAPI ptc_timer_time(PTC_TIMER object);
CAPI double PTCAPI ptc_timer_delta(PTC_TIMER object);
CAPI double PTCAPI ptc_timer_resolution(PTC_TIMER object);

/* operators */
CAPI void PTCAPI ptc_timer_assign(PTC_TIMER object,PTC_TIMER timer);
CAPI int PTCAPI ptc_timer_equals(PTC_TIMER object,PTC_TIMER timer);




#endif
