/* ============================================================================
 *  LDebug.h
 * ============================================================================

 *  Author:         (c) 2001-2002 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 28, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.10 $
*/


#ifndef __LDebug__
#define __LDebug__

#include "LConfig.h"
#include "LType.h"

/* COMPONENT ID */
#define LDebug_ID   0x8004

enum { 
    LDebug_INTERNAL_ERROR  = LDebug_ID<<16
};

typedef void (*LDebug_THandler)(const i1*);

void        LDebug_GetString(i1* outStr, ui4 inSize);
void        LDebug_Print(const i1* inMsg, ...);
void        LDebug_Write(const i1* inMsg, ui4 inSize);
void        LDebug_Exit();
void        LDebug_InstallPrintHandler(LDebug_THandler);
void        LDebug_OpenBlock();
void        LDebug_CloseBlock(i1** outBlock,ui4* outSize);

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
