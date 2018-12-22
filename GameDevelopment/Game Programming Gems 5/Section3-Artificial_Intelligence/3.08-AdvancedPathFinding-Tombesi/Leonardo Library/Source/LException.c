/* ============================================================================
 *  LException.c
 * ============================================================================

 *  Author:         (c) 2001 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 28, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.5 $
*/

#include "LException.h"
#include "LDebug.h"
#include <stdio.h>
#include <stdlib.h>


/* GLOBAL VARIABLES */
jmp_buf*        _LException_gEnv    = NULL;
Bool            _LException_gThrown = 0;
LException      _LException_gVal;


/* ---------------------------------------------------------------------------------
 *  Dump
 * ---------------------------------------------------------------------------------
 * Print exception info */

void LException_Dump(LException* This) {
    LDebug_Print("Exception %s (0x%lX) in file %s at line %lu.\n",
        LException_GetName(This),
        LException_GetCode(This),
        LException_GetFileName(This),
        LException_GetLine(This)
    );
}


/* ---------------------------------------------------------------------------------
 *  _LException_Panic
 * ---------------------------------------------------------------------------------
 * Handle abnormal exception handling (for debugging purposes) */

void _LException_Panic(){
    LDebug_Print("Throw without Catch error\n");
    LDebug_Exit();
}


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
