/* ============================================================================
 *  LArray.c
 * ============================================================================

 *  Author:         (c) 2001 Irene Finocchi
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/14 20:51:33 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.17 $
*/


#include "LArray.h"
#include "LException.h"
#include "LMemory.h"
#include "LMath.h"


/* MEMBER VARIABLES */
struct LArray {
    i1*         mData;
    ui4         mDataSize;
    ui4         mItemsCount;
    ui4         mItemSize;
    void**      mSyncData;
};

#ifdef __LSL_DEBUG__
/* #define TRACE_OPERATIONS_ */
#endif

/* Initially, space for BASEITEMS entries */
#define BASEITEMS           50

/* Expansion factor */
#define GROWFACTOR          2

/* The size of the data segment is always BASEITEMS*This->mItemSize*2^k bytes, for some k */
#define DataSize_(itemsCount,itemSize) (                                                    \
    (ui4)(itemSize*BASEITEMS*                                                               \
        (LMath_Pow((f8)GROWFACTOR,                                                          \
            LMath_Ceil(LMath_Log(LMath_Ceil((itemsCount)/(f8)(itemSize*BASEITEMS)))/        \
                LMath_Log((f8)GROWFACTOR)))))                                               \
)

/* In the current implementation the size of the data segment is equal to the
 * maximum size reached during the sequence of operations. The data segment is 
 * never shrunk and grows exponentially. */


/* ---------------------------------------------------------------------------------
 *  LArray_New
 * ---------------------------------------------------------------------------------
 * Constructor */

LArray* LArray_New(ui4 inItemSize) {

    LArray theObject;

    if (inItemSize<1) Throw(LArray_INVALID_SIZE);

    theObject.mData          = NULL;
    theObject.mDataSize      = 0;
    theObject.mItemsCount    = 0;
    theObject.mItemSize      = inItemSize;
    theObject.mSyncData      = NULL;

    #ifdef TRACE_OPERATIONS_
        LDebug_Print("LArray_Constructor [itemsize=%lu]\n", inItemSize);
    #endif

    return LMemory_NewObject(LArray,theObject);
}


/* ---------------------------------------------------------------------------------
 *  LArray
 * ---------------------------------------------------------------------------------
 * Constructor */

LArray* LArray_NewFromData(ui4 inItemSize, void** inDataA, ui4 inDataSize) {

    LArray theObject;

    /* Set item size and number of items */
    if (inItemSize<1 || inDataSize%inItemSize) Throw(LArray_INVALID_SIZE);

    theObject.mItemSize      = inItemSize;
    theObject.mItemsCount    = inDataSize/inItemSize;
    
    /* Added IF 021112 */
    theObject.mSyncData      = NULL;

    /* Set data segment */
    theObject.mData = (i1*)*inDataA;

    /* Flag block is now incorporated into newly created LArray */
    (*inDataA) = NULL;

    /* Set array size */
    theObject.mDataSize = inDataSize;

    #ifdef TRACE_OPERATIONS_
        LDebug_Print("LArray_Constructor [itemsize=%lu  datasize=%lu]\n",
                      inItemSize,inDataSize);
    #endif

    return LMemory_NewObject(LArray,theObject);
}


/* ---------------------------------------------------------------------------------
 *  LArray_Delete
 * ---------------------------------------------------------------------------------
 * Destructor */

void LArray_Delete(LArray** ThisA){
    if ((*ThisA)->mData!=NULL) {
        LMemory_Free(&(*ThisA)->mData);
        if ((*ThisA)->mSyncData) *(*ThisA)->mSyncData = (*ThisA)->mData;
    }
    LMemory_DeleteObject(ThisA);
}


/* ---------------------------------------------------------------------------------
 *  InsertItemAt
 * ---------------------------------------------------------------------------------
 * Insert item inItem at entry with index inIndex. */

void LArray_InsertItemAt(LArray* This, const void* inItem, ui4 inIndex){

    /* Range adjustment */
    if (inIndex > This->mItemsCount) inIndex = This->mItemsCount;

    /* Make room for the new entry */
    LArray_ResizeBy(This,+1);

    /* Scroll items with indices > inIndex */
    if (This->mItemsCount -1 -inIndex > 0)
        LMemory_Move(This->mData + (inIndex)  * This->mItemSize,
                    This->mData + (inIndex+1) * This->mItemSize,
                    This->mItemSize * (This->mItemsCount -1 -inIndex));

    /* Assign the new entry */
    LMemory_Copy(inItem, This->mData+(inIndex)*This->mItemSize, This->mItemSize);
}


/* ---------------------------------------------------------------------------------
 *  RemoveItemAt
 * ---------------------------------------------------------------------------------
 * Remove item with index inIndex. Compact the array after the deletion. */

void LArray_RemoveItemAt(LArray* This, ui4 inIndex){

    /* Range check */
    if (inIndex>=This->mItemsCount) Throw(LArray_OUT_OF_RANGE);

    /* Scroll items with indices > inIndex */
    LMemory_Move(This->mData + (inIndex+1) * This->mItemSize,
                 This->mData + (inIndex)   * This->mItemSize,
                 This->mItemSize * (This->mItemsCount -1 -inIndex));

    /* Shrink the array */
    LArray_ResizeBy(This,-1);
}


/* ---------------------------------------------------------------------------------
 *  AppendItem
 * ---------------------------------------------------------------------------------
 * Append an item at the end of the array */

ui4 LArray_AppendItem(LArray* This, const void* inItem){

    /* Make room for the new entry */
    LArray_ResizeBy(This,+1);

    /* Copy the input item at the end of the array */
    LMemory_Copy(inItem, This->mData+(This->mItemsCount-1)*This->mItemSize, This->mItemSize);

    #ifdef TRACE_OPERATIONS_
        LDebug_Print("LArray_AppendItem [This->mData size=%lu, This->mItemsCount=%lu]\n",
                      This->mDataSize,This->mItemsCount);
    #endif

    /* Return the index of added item */
    return This->mItemsCount-1;
}


/* ---------------------------------------------------------------------------------
 *  ResizeBy
 * ---------------------------------------------------------------------------------
 * Resize data segment */

void LArray_ResizeBy(LArray* This, i4 inDeltaItems){

    /* SE021225: bug fix */

    ui4 theArraySize;

    /* Increase or decrease the number of items in the array */
    if (inDeltaItems < 0)
    {
        if ((ui4)-inDeltaItems >= This->mItemsCount)
            This->mItemsCount = 0;
        else
            This->mItemsCount += inDeltaItems;
    }
    else
        This->mItemsCount += inDeltaItems;

    /* Compute current array size */
    theArraySize = This->mItemsCount*This->mItemSize;

    /* Return if the memory chunk is still large enough to contain the array */
    if (theArraySize<=This->mDataSize) return;

    /* If the array was empty, allocate new memory */
    if (This->mDataSize==0) {

        /* Compute the initial size of the memory chunk */
        This->mDataSize = DataSize_(theArraySize,This->mItemSize);

        /* Allocate memory chunk */
        This->mData = (i1*) LMemory_Malloc(This->mDataSize);
        if (This->mSyncData) *This->mSyncData = This->mData;

        return;
    }

    /* Compute the new size of the memory chunk */
    This->mDataSize = DataSize_(theArraySize,This->mItemSize);

    /* ... otherwise reallocate memory */
    This->mData = (i1*) LMemory_Realloc(This->mData,This->mDataSize);
    if (This->mSyncData) *This->mSyncData = This->mData;
}


/* ---------------------------------------------------------------------------------
 *  RemoveLastItem
 * ---------------------------------------------------------------------------------
 * Remove the last item of the array. Deallocate data segment if it gets empty. */

void LArray_RemoveLastItem(LArray* This) {
    if (This->mItemsCount<1) Throw(LArray_OUT_OF_RANGE);
    if (--This->mItemsCount==0) {
        LMemory_Free(&This->mData);
        if (This->mSyncData) *This->mSyncData = This->mData;
        This->mDataSize=0;
    }

    #ifdef TRACE_OPERATIONS_
        LDebug_Print("LArray_RemoveLastItem [This->mData size=%lu, This->mItemsCount=%lu]\n",
                      This->mDataSize,This->mItemsCount);
    #endif
}


/* ---------------------------------------------------------------------------------
 *  RemoveAllItems
 * ---------------------------------------------------------------------------------
 * Reset the array to contain 0 items. Deallocate data segment if necessary. */

void LArray_RemoveAllItems(LArray* This){
    if (This->mItemsCount<1) return;
    LMemory_Free(&This->mData);
    if (This->mSyncData) *This->mSyncData = This->mData;
    This->mDataSize=This->mItemsCount=0;

    #ifdef TRACE_OPERATIONS_
        LDebug_Print("LArray_RemoveAllItems [This->mData size=%lu, This->mItemsCount=%lu]\n",
                      This->mDataSize,This->mItemsCount);
    #endif
}


/* ---------------------------------------------------------------------------------
 *  ItemAt
 * ---------------------------------------------------------------------------------
 * Get address of item with index inIndex. */

void* LArray_ItemAt(LArray* This, ui4 inIndex){

    /* Range check */
    if (inIndex>=This->mItemsCount) Throw(LArray_OUT_OF_RANGE);

    /* Get item reference */
    return (void*)(This->mData+(inIndex)*This->mItemSize);
}


/* ---------------------------------------------------------------------------------
 *  LastItem
 * ---------------------------------------------------------------------------------
 * Get address of last item. */

void* LArray_LastItem(LArray* This){

    /* Existence check */
    if (This->mItemsCount<1) Throw(LArray_OUT_OF_RANGE);

    /* Get item reference */
    return (void*)(This->mData+(This->mItemsCount-1)*This->mItemSize);
}


/* ---------------------------------------------------------------------------------
 *  FetchItemAt
 * ---------------------------------------------------------------------------------
 * Copy item with index inIndex to buffer outItem */

Bool LArray_FetchItemAt(LArray* This, ui4 inIndex, void* outItem){

    /* Range check */
    if (inIndex>=This->mItemsCount) return FALSE;

    /* Pass back item */
    LMemory_Copy(This->mData+(inIndex)*This->mItemSize,(i1 *) outItem,This->mItemSize);

    return TRUE;
}


/* ---------------------------------------------------------------------------------
 *  GetData
 * ---------------------------------------------------------------------------------
 * Give external access to the data (NULL if array is empty) */

void* LArray_GetData(LArray* This){
    return (void*)This->mData;
}


/* ---------------------------------------------------------------------------------
 *  GetDataSize
 * ---------------------------------------------------------------------------------
 * Return the number of bytes of the array containing initialized data (the array 
 * may be larger and its real size is This->mDataSize) */

ui4 LArray_GetDataSize(LArray* This){
    return This->mItemsCount*This->mItemSize;
}


/* ---------------------------------------------------------------------------------
 *  GetItemsCount
 * ---------------------------------------------------------------------------------
 * Return the number of items in the array */

ui4 LArray_GetItemsCount(LArray* This){
    return This->mItemsCount;
}

/* ---------------------------------------------------------------------------------
 *  GetItemSize
 * ---------------------------------------------------------------------------------
 * Return the number of items in the array *
 * Added by SE.14/03/2003                  */   
ui4 LArray_GetItemSize(LArray* This){
    return This->mItemSize;
}


/* ---------------------------------------------------------------------------------
 *  GetUsedMem
 * ---------------------------------------------------------------------------------
 * Return the number of bytes used to maintain the LArray object This */

ui4 LArray_GetUsedMem(LArray* This){
    return This->mDataSize + sizeof(LArray);
}


/* ---------------------------------------------------------------------------------
 *  InstallSyncPtr
 * ---------------------------------------------------------------------------------
 * (*inDataPtr) will always be equal to the value returned by GetData() */

void LArray_InstallSyncPtr(LArray* This, void** inDataPtr){
    This->mSyncData = inDataPtr;
    if (This->mSyncData) *This->mSyncData = This->mData;
}

/* ---------------------------------------------------------------------------------
 *  Dump
 * ---------------------------------------------------------------------------------
 * Dump object status */

#ifdef __LSL_DEBUG__
void LArray_Dump(LArray* This){
    LDebug_Print("LArray: [@%p - mDataSize=%lu - mItemsCount=%lu - mItemSize=%lu]\n",
                  This, This->mDataSize, This->mItemsCount, This->mItemSize);
}
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
