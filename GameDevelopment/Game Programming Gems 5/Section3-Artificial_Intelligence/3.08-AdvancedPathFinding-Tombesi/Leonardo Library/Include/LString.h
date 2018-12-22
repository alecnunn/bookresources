/* ============================================================================
 *  LString.h
 * ============================================================================

 *  Author:         (c) 2001 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.10 $
*/


#ifndef __LString__
#define __LString__

#include "LType.h"

/* COMPONENT ID */
#define LString_ID  0x8009

/* CONSTANTS */
typedef enum {
    LString_CR,
    LString_LF,
    LString_CRLF
} LString_TNewline;

/* FUNCTIONS */
i1*    LString_Clone     (const i1* inStr);
void   LString_Copy      (const i1* inStr, i1* outStr);
void   LString_Append    (const i1* inStr, i1* thruStr);
ui4    LString_Len       (const i1* inStr);
i1     LString_Compare   (const i1* inStr1, const i1* inStr2);
void   LString_Format    (i1* outStr, const i1* inFormatStr, ...);
i1*    LString_Tokenizer (i1* inLine, const i1* inSeparators);
i4     LString_ToI4      (const i1* inStr);
ui4    LString_ToUI4     (const i1* inStr);
f8     LString_ToF8      (const i1* inStr);

void   LString_PToXP     (i1** thruText, ui4* thruSize);
void   LString_XPToP     (i1** thruText, ui4* thruSize);
#endif


/* Copyright (C) 2001 Camil Demetrescu

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
*/
