/* ============================================================================
 *  LHeap.c
 * ============================================================================

 *  Author:         (c) 2002 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        October 4, 2002
 *  Module:         LSL

 *  Last changed:   $Date: 2002/12/20 14:30:31 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.4 $
*/


#include "LHeap.h"
#include "LException.h"
#include "LMemory.h"
#include "LDebug.h"


/* TYPEDEFS */
typedef struct {
    void* mItem;
    void* mKey;
    ui4   mIdx;
} TMapItem;

struct tagLHeap { /* 32 bytes */
    ui4*              mHeap;
    TMapItem*         mMap;
    ui4*              mHoles;
    ui4               mSize;
    ui4               mHolesSize;
    ui4               mCount;
    ui4               mHolesCount;
    LHeap_TComparator mComp;
};


/* DEFINES */
/* empty heap requires approx 16KB (1024*(sizeof(TMapItem)+4+4) byte) */
#define _INIT_SIZE 1024

/* blocks grow by 50% when they get full */
#define _GROW_FACTOR 0.5

/* DEBUGGING/TRACING */
#if 0
#define __TRACE_REALLOC__
#endif

/* ---------------------------------------------------------------------------------
 *  New
 * ---------------------------------------------------------------------------------
 * Constructor */
LHeap* LHeap_New(LHeap_TComparator inComparator) {

    LHeap theObject = { 0 };

    Try {
        theObject.mCount = 0;
        theObject.mHolesCount = 0;
        theObject.mSize  = _INIT_SIZE;
        theObject.mHolesSize  = _INIT_SIZE;
        theObject.mComp = inComparator;
        theObject.mHeap  = (ui4*)LMemory_Malloc(theObject.mSize*4+4);
        theObject.mMap   = (TMapItem*)LMemory_Malloc(theObject.mSize*sizeof(TMapItem));
        theObject.mHoles = (ui4*)LMemory_Malloc(theObject.mHolesSize*4);
    }

    CatchAny {
        if (theObject.mHeap!=NULL)  LMemory_Free(&theObject.mHeap);
        if (theObject.mMap!=NULL)   LMemory_Free(&theObject.mMap);
        if (theObject.mHoles!=NULL) LMemory_Free(&theObject.mHoles);
        Rethrow;
    }

    return LMemory_NewObject(LHeap,theObject);
}


/* ---------------------------------------------------------------------------------
 *  Delete
 * ---------------------------------------------------------------------------------
 * Destructor */
void LHeap_Delete(LHeap** ThisA){
    LMemory_Free(&(*ThisA)->mHeap);
    LMemory_Free(&(*ThisA)->mMap);
    LMemory_Free(&(*ThisA)->mHoles);
    LMemory_DeleteObject(ThisA);
}


/* ---------------------------------------------------------------------------------
 *  Empty
 * ---------------------------------------------------------------------------------
*/
Bool LHeap_Empty(LHeap* This){
    return (Bool)(This->mCount == 0);
}


/* ---------------------------------------------------------------------------------
 *  Insert
 * ---------------------------------------------------------------------------------
*/
ui4 LHeap_Insert(LHeap* This, const void* inItem, const void* inKey){
    ui4 theSlot, theIdx;

    /* resize block, if necessary */
    if (This->mCount == This->mSize) {
        This->mSize += (ui4)(This->mSize*_GROW_FACTOR);
        #ifdef __TRACE_REALLOC__
        LDebug_Print("*** Resizing Heap to contain %-10lu elements...  ",This->mSize);
        #endif
        This->mHeap  = (ui4*)LMemory_Realloc(This->mHeap,This->mSize*4+4);
        This->mMap   = (TMapItem*)LMemory_Realloc(This->mMap,This->mSize*sizeof(TMapItem));
        #ifdef __TRACE_REALLOC__
        LDebug_Print("[DONE]\n");
        #endif
    }

    /* reuse hole, or manage to append at the end of the map */
    theSlot = (This->mHolesCount>0)? This->mHoles[--This->mHolesCount] : This->mCount;
    ++This->mCount;

    /* find correct position in heap of the item to be added */
    for (theIdx = This->mCount; theIdx>1; theIdx >>= 1) {
        if (!This->mComp(inKey,This->mMap[This->mHeap[theIdx>>1]].mKey)) break;
        This->mHeap[theIdx] = This->mHeap[theIdx>>1];
        This->mMap[This->mHeap[theIdx]].mIdx = theIdx;
    }

    /* add item to heap and map */
    This->mHeap[theIdx] = theSlot;
    This->mMap[theSlot].mItem = (void*)inItem;
    This->mMap[theSlot].mKey  = (void*)inKey;
    This->mMap[theSlot].mIdx = theIdx;

    return theSlot;
}


/* ---------------------------------------------------------------------------------
 *  Update
 * ---------------------------------------------------------------------------------
 * CD021005: this version supports decreases only... */
void LHeap_Update(LHeap* This, const void* inItem, const void* inKey, ui4 inIdx){
    ui4 theIdx;

    /* check index */
    if (/*inIdx >= This->mCount ||*/ This->mMap[inIdx].mIdx == 0xFFFFFFFF) 
        Throw(LHeap_BAD_INDEX);

    /* find correct position in heap of the item to be added */
    for (theIdx = This->mMap[inIdx].mIdx; theIdx>1; theIdx >>= 1) {
        if (!This->mComp(inKey,This->mMap[This->mHeap[theIdx>>1]].mKey)) break;
        This->mHeap[theIdx] = This->mHeap[theIdx>>1];
        This->mMap[This->mHeap[theIdx]].mIdx = theIdx;
    }

    /* add item to heap and map */
    This->mHeap[theIdx] = inIdx;
    This->mMap[inIdx].mItem = (void*)inItem;
    This->mMap[inIdx].mKey = (void*)inKey;
    This->mMap[inIdx].mIdx = theIdx;
}


/* ---------------------------------------------------------------------------------
 *  ExtractMin
 * ---------------------------------------------------------------------------------
*/
void LHeap_ExtractMin(LHeap* This, void** outItem, void** outKey){

    ui4   theIdx;
    void *theLastKey;

    if (This->mCount == 0) Throw(LHeap_EMPTY_HEAP);

    /* fetch item and key */
    if (outItem!=NULL) *outItem = This->mMap[This->mHeap[1]].mItem;
    if (outKey!=NULL)  *outKey  = This->mMap[This->mHeap[1]].mKey;

    /* get key of last item */
    theLastKey = This->mMap[This->mHeap[This->mCount]].mKey;

    /* resize holes list if necessary */
    if (This->mHolesCount == This->mHolesSize) {
        This->mHolesSize += (ui4)(This->mHolesSize*_GROW_FACTOR);
        This->mHoles = (ui4*)LMemory_Realloc((void*)This->mHoles,This->mHolesSize*4);
    }

    /* stack empty slot into holes list */
    This->mHoles[This->mHolesCount++] = This->mHeap[1];
    This->mMap[This->mHeap[1]].mIdx = 0xFFFFFFFF;

    /* find correct position in heap of the last item */
    for (theIdx=1; ;) {
        ui4 theMinIdx = theIdx<<1;
        if (theMinIdx>This->mCount) break;
        if (theMinIdx<This->mCount && 
            This->mComp(This->mMap[This->mHeap[theMinIdx+1]].mKey,
                        This->mMap[This->mHeap[theMinIdx]].mKey)) ++theMinIdx;
        if (!This->mComp(This->mMap[This->mHeap[theMinIdx]].mKey,theLastKey)) break;
        This->mHeap[theIdx] = This->mHeap[theMinIdx];
        This->mMap[This->mHeap[theIdx]].mIdx = theIdx;
        theIdx = theMinIdx;
    }

    This->mHeap[theIdx] = This->mHeap[This->mCount];
    This->mMap[This->mHeap[theIdx]].mIdx = theIdx;

    --This->mCount;
}


/* ---------------------------------------------------------------------------------
 *  GetMin
 * ---------------------------------------------------------------------------------
*/
void LHeap_GetMin(LHeap* This, void** outItem, void** outKey){
    if (This->mCount == 0) Throw(LHeap_EMPTY_HEAP);
    if (outItem!=NULL) *outItem = This->mMap[This->mHeap[1]].mItem;
    if (outKey!=NULL)  *outKey  = This->mMap[This->mHeap[1]].mKey;
}


/* ---------------------------------------------------------------------------------
 *  GetItemAt
 * ---------------------------------------------------------------------------------
*/
void LHeap_GetEntryAt(LHeap* This, void** outItem, void** outKey, ui4 inIdx){
    /* check index */
    if (/*inIdx >= This->mCount ||*/ This->mMap[inIdx].mIdx == 0xFFFFFFFF) 
        Throw(LHeap_BAD_INDEX);

    if (outItem!=NULL) *outItem = This->mMap[inIdx].mItem;
    if (outKey!=NULL)  *outKey  = This->mMap[inIdx].mKey;
}


/* ---------------------------------------------------------------------------------
 *  GetUsedMem
 * ---------------------------------------------------------------------------------
*/
ui4 LHeap_GetUsedMem (LHeap* This){
    return sizeof(LHeap) + 
           (This->mSize*4+4) + 
           (This->mSize*sizeof(TMapItem)) + 
           (This->mHolesSize*4);
}


/* Copyright (C) 2002 Camil Demetrescu

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
