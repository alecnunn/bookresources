/* ============================================================================
 *  LHeap.h
 * ============================================================================

 *  Author:         (c) 2002 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.3 $
*/


#ifndef __LHeap__
#define __LHeap__

#include "LType.h"

/* COMPONENT ID */
#define LHeap_ID   0x8026

/* TYPEDEFS */
typedef struct tagLHeap LHeap;
typedef Bool (*LHeap_TComparator)(const void* inA, const void* inB);

/* EXCEPTION CODES */
enum { 
    LHeap_BAD_INDEX = LHeap_ID<<16,
    LHeap_EMPTY_HEAP
};

/* PUBLIC FUNCTION PROTOTYPES */
LHeap*      LHeap_New        (LHeap_TComparator inComparator);
void        LHeap_Delete     (LHeap** ThisA);

Bool        LHeap_Empty      (LHeap* This);
ui4         LHeap_Insert     (LHeap* This, const void* inItem, const void* inKey);
void        LHeap_Update     (LHeap* This, const void* inItem, const void* inKey, ui4 inIdx);
void        LHeap_ExtractMin (LHeap* This, void** outItem, void** outKey);
void        LHeap_GetMin     (LHeap* This, void** outItem, void** outKey);
void        LHeap_GetEntryAt (LHeap* This, void** outItem, void** outKey, ui4 inIdx);

ui4         LHeap_GetUsedMem (LHeap* This);
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
