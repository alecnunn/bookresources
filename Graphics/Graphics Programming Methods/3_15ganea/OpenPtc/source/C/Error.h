/*
 * C Error class for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_C_ERROR_H
#define __PTC_C_ERROR_H

/* include files */
#include "config.h"
#include "C/Index.h"
#include "C/Types.h"




/* setup */
CAPI PTC_ERROR PTCAPI ptc_error_create(char *message);
CAPI PTC_ERROR PTCAPI ptc_error_create_composite(char *message,PTC_ERROR error);
CAPI void PTCAPI ptc_error_destroy(PTC_ERROR object);

/* report error */
CAPI void PTCAPI ptc_error_report(PTC_ERROR object);

/* get error message */
CAPI char* PTCAPI ptc_error_message(PTC_ERROR object);

/* operators */
CAPI void PTCAPI ptc_error_assign(PTC_ERROR object,PTC_ERROR error);
CAPI int PTCAPI ptc_error_equals(PTC_ERROR object,PTC_ERROR error);

#ifdef __COMPILER_IS_GNU__

    /* error management (gnu style) */
    typedef void (*PTC_ERROR_HANDLER)(PTC_ERROR error);
    CAPI void PTCAPI ptc_error_handler(PTC_ERROR_HANDLER handler);

#else

    /* error management */
    typedef void (PTC_CALLBACK *PTC_ERROR_HANDLER)(PTC_ERROR error);
    CAPI void PTCAPI ptc_error_handler(PTC_ERROR_HANDLER handler);

#endif


#endif
