/* ============================================================================
 *  LMemory.c
 * ============================================================================

 *  Author:         (c) 2001 Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        November 29, 2001
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/29 13:46:37 $
 *  Changed by:     $Author: demetres $
 *  Revision:       $Revision: 1.12 $
*/


#include "LMemory.h"
#include "LException.h"
#include "LDebug.h"
#include <stdlib.h>
#include <string.h>


/* GLOBAL VARIABLES */
void* _LMemory_gTmp;

#ifdef __LSL_DEBUG__
static i4 sBlocksCount=0;
#endif

/* ----------------------------------------------------------------------------
 *  Malloc
 * ----------------------------------------------------------------------------
*/
void* LMemory_Malloc(ui4 inSize){
    void* theTemp;

    if (inSize==0) 
        Throw(LMemory_ZERO_BYTE_ALLOCATION_REQUEST);

    theTemp=malloc(inSize);
    if (theTemp==NULL) Throw(LMemory_OUT_OF_MEMORY);

    #ifdef __LSL_DEBUG__
    sBlocksCount++;
    #endif

    return theTemp;
}


/* ----------------------------------------------------------------------------
 *  Calloc
 * ----------------------------------------------------------------------------
*/
void* LMemory_Calloc(ui4 inSize){
    void* theTemp;
    
    theTemp=calloc(1,inSize);
    if (theTemp==NULL) Throw(LMemory_OUT_OF_MEMORY);

    #ifdef __LSL_DEBUG__
    sBlocksCount++;
    #endif

    return theTemp;
}


/* ----------------------------------------------------------------------------
 *  Realloc
 * ----------------------------------------------------------------------------
*/
void* LMemory_Realloc(void* inPtr, ui4 inSize){
    void* theTemp;

    theTemp=realloc(inPtr,inSize);
    if (theTemp==NULL) 
        Throw(LMemory_OUT_OF_MEMORY);

    return theTemp;
}


/* ----------------------------------------------------------------------------
 *  Free
 * ----------------------------------------------------------------------------
*/
void _LMemory_Free(void* inDummy, void** inPtrA){

    #ifdef __LSL_DEBUG__
    if ((*inPtrA) == NULL) Throw(LMemory_NULL_POINTER_FREE_REQUEST);
    #endif

    #ifdef __LSL_DEBUG__
    sBlocksCount--;
    #endif

    if ((*inPtrA) != NULL) free(*(inPtrA));
    (*(inPtrA)) = NULL;
}


/* ----------------------------------------------------------------------------
 *  Copy
 * ----------------------------------------------------------------------------
*/
void  LMemory_Copy(const void* inSource, void* outDest, ui4 inSize){
    memcpy(outDest,inSource,inSize);
}


/* ----------------------------------------------------------------------------
 *  Move
 * ----------------------------------------------------------------------------
*/
void  LMemory_Move(const void* inSource, void* outDest, ui4 inSize){
    memmove(outDest,inSource,inSize);
}


/* ----------------------------------------------------------------------------
 *  Set
 * ----------------------------------------------------------------------------
*/
void  LMemory_Set(void* outPtr, i1 inByte, ui4 inSize){
    memset(outPtr, inByte, inSize);
}


/* ----------------------------------------------------------------------------
 *  Clear
 * ----------------------------------------------------------------------------
*/
void  LMemory_Clear(void* outPtr, ui4 inSize){
    memset(outPtr, 0, inSize);
}


/* ----------------------------------------------------------------------------
 *  Compare
 * ----------------------------------------------------------------------------
*/
i4  LMemory_Compare(const void* inPtr1, const void* inPtr2, ui4 inSize){
    return memcmp(inPtr1,inPtr2,inSize);
}

#ifdef __LSL_DEBUG__
/* ----------------------------------------------------------------------------
 *  GetBlocksCount
 * ----------------------------------------------------------------------------
 * return number of blocks allocated with Malloc and Calloc and still not deallocated */
i4  LMemory_GetBlocksCount(){
    return sBlocksCount;
}
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
