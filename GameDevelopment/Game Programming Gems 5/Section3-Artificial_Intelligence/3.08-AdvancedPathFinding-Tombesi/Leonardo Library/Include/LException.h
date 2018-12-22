/* ============================================================================
 *  LException.h
 * ============================================================================

 *  Author:         (c) 2001-2002 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL
 *  Notes:          The implementation of macros Try and Catch is inspired by 
 *                  cexcept.h, by Adam M. Costello and Cosmin Truta.
 *                  See: http://www.cs.berkeley.edu/~amc/cexcept/

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.7 $
*/


#ifndef __LException__
#define __LException__

#include "LType.h"
#include "LConfig.h"
#include <setjmp.h>

/* COMPONENT ID */
#define LException_ID   0x8005

/* MEMBER VARIABLE */
typedef struct LException {
    ui4 mCode;          /* Exception code                                        */
    i1* mName;          /* Exception name (e.g., LDebug_INTERNAL_ERROR)          */
    i1* mFileName;      /* Name of source file where exception was thrown        */
    ui4 mLine;          /* Line number in source file where exception was thrown */
} LException;

/* STATIC ENVIRONMENT CONTEXT */
extern jmp_buf*     _LException_gEnv;
extern LException   _LException_gVal;
extern Bool         _LException_gThrown;

/* MACROS FOR EXCEPTION HANDLING */
#define Try {                                               \
    jmp_buf *_thePrevEnv, _theEnv;                          \
    _thePrevEnv = _LException_gEnv;                         \
    _LException_gEnv = &_theEnv;                            \
    if (!setjmp(_theEnv)) {                                 \
        if (&_theEnv)

#define _LException_Catch(a)                                \
        else { }                                            \
        _LException_gThrown = FALSE;                        \
    }                                                       \
    else {                                                  \
        _LException_gThrown = TRUE;                         \
    }                                                       \
    _LException_gEnv = _thePrevEnv;                         \
    }                                                       \
    if (!_LException_gThrown || a) { }                      \
    else

#define Catch(e) _LException_Catch(((e) = &_LException_gVal, 0))
#define CatchAny _LException_Catch(0)

#define Throw(e) {                                          \
    if (_LException_gEnv==NULL) _LException_Panic();        \
    _LException_gVal.mCode      = (ui4)(e);                 \
    _LException_gVal.mLine      = __LINE__;                 \
    _LException_gVal.mFileName  = __FILE__;                 \
    _LException_gVal.mName      = #e;                       \
    longjmp(*_LException_gEnv,0);                           \
}

#define Rethrow {                                           \
    if (_LException_gEnv==NULL) _LException_Panic();        \
    longjmp(*_LException_gEnv,0);                           \
}

/* PUBLIC FUNCTIONS AND MACROS */
#define LException_GetName(e)       (e->mName)
#define LException_GetCode(e)       (e->mCode)
#define LException_GetLine(e)       (e->mLine)
#define LException_GetFileName(e)   (e->mFileName)

#ifdef __LSL_DEBUG__
void    LException_Dump             (LException* This);
#else
#define LException_Dump(x)
#endif

/* EXCEPTION PANIC (for debugging purposes) */
void _LException_Panic();

#endif


/* Copyright (C) 2001-2002 Camil Demetrescu

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 * Notes: The implementation of macros Try and Catch is inspired by 
 *        cexcept.h, by Adam M. Costello and Cosmin Truta.
 *        See: http://www.cs.berkeley.edu/~amc/cexcept/
*/
