/* ============================================================================
 *  LArray.h
 * ============================================================================

 *  Author:         (C) 2001 Irene Finocchi
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/14 14:21:22 $
 *  Changed by:     $Author: stefano $
 *  Revision:       $Revision: 1.11 $
*/


#ifndef __LArray__
#define __LArray__

#include "LType.h"
#include "LDebug.h"

/* COMPONENT ID */
#define LArray_ID   0x8002

typedef struct LArray LArray;

enum { 
    LArray_INVALID_SIZE = LArray_ID<<16, 
    LArray_OUT_OF_RANGE
};

LArray*     LArray_New              (ui4 inItemSize);
LArray*     LArray_NewFromData      (ui4 inItemSize, void** inDataA, ui4 inDataSize);
void        LArray_Delete           (LArray** ThisA);

void        LArray_InsertItemAt     (LArray* This, const void* inItem, ui4 inIndex);
void        LArray_RemoveItemAt     (LArray* This, ui4 inIndex);
ui4         LArray_AppendItem       (LArray* This, const void* inItem);
void        LArray_ResizeBy         (LArray* This, i4 inDeltaItems);
void        LArray_RemoveLastItem   (LArray* This);
void        LArray_RemoveAllItems   (LArray* This);
void*       LArray_ItemAt           (LArray* This, ui4 inIndex);
void*       LArray_LastItem         (LArray* This);
Bool        LArray_FetchItemAt      (LArray* This, ui4 inIndex, void* outItem);
void*       LArray_GetData          (LArray* This);
ui4         LArray_GetDataSize      (LArray* This);
ui4         LArray_GetItemsCount    (LArray* This);
ui4         LArray_GetUsedMem       (LArray* This);
void        LArray_InstallSyncPtr   (LArray* This, void** thruDataPtr);
ui4			LArray_GetItemSize		(LArray* This);

#ifdef __LSL_DEBUG__
void        LArray_Dump             (LArray* This);
#endif

#endif

/* Copyright (C) 2001 Irene Finocchi

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
