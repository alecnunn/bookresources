/* ============================================================================
 *  LEdgeInfo.h
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/02/07 10:46:13 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.10 $    
*/


#ifndef __LEdgeInfo__
#define __LEdgeInfo__

#include "LType.h"
#include "LArray.h"
#include "LGraph.h"

/* COMPONENT ID */
#define LEdgeInfo_ID   0x802B

/* EXCEPTIONS */
enum 
{ 
    LEdgeInfo_OBJECT_NULL_POINTER = LEdgeInfo_ID<<16, 
    LEdgeInfo_GRAPH_NULL_POINTER,
    LEdgeInfo_EDGE_NULL_POINTER,
    LEdgeInfo_ITEM_NULL_POINTER,
	LEdgeInfo_BASE_TYPE_MISMATCH
};


typedef struct LEdgeInfo LEdgeInfo;

struct LEdgeInfo
{
    LGraph*		 mGraph;
    LArray*		 mData;
	LType_TType  mBaseType;
	void         (*mDealloc)(LEdgeInfo*, LGraph_TEdge*);
	void         (*mAlloc)  (LEdgeInfo*, LGraph_TEdge*);
};

/* PUBLIC MACROS */
#define LEdgeInfo_ItemAt(n,i) LArray_ItemAt((n)->mData, (i)->mIndex)

/* MACROS TO ACCESS BASE TYPES */
#define LEdgeInfo_UI1At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_UI1))? _LEdgeInfo_Panic(0) : 0, ((ui1*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_UI2At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_UI2))? _LEdgeInfo_Panic(0) : 0, ((ui2*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_UI4At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_UI4))? _LEdgeInfo_Panic(0) : 0,((ui4*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_I1At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_I1))? _LEdgeInfo_Panic(0) : 0, ((i1*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_I2At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_I2))? _LEdgeInfo_Panic(0) : 0, ((i2*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_I4At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_I4))? _LEdgeInfo_Panic(0) : 0, ((i4*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_F4At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_F4))? _LEdgeInfo_Panic(0) : 0, ((f4*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_F8At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_F8))? _LEdgeInfo_Panic(0) : 0, ((f8*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_BoolAt(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_Bool))? _LEdgeInfo_Panic(0) : 0, ((Bool*)LEdgeInfo_ItemAt(n,i)) ))

#define LEdgeInfo_PointerAt(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_Ptr))? _LEdgeInfo_Panic(0) : 0, ((void*)LEdgeInfo_ItemAt(n,i)) ))


/* PUBLIC FUNCTIONS */
LEdgeInfo* LEdgeInfo_New          (LGraph* inGraph, const LType_TType inType);
LEdgeInfo* LEdgeInfo_NewCustom    (LGraph* inGraph, ui4 inItemSize);
void       LEdgeInfo_Delete       (LEdgeInfo** ThisA);
void       LEdgeInfo_AssignItemAt (LEdgeInfo* This, LGraph_TEdge* inEdge, 
                                   const void* inItem);
Bool	   LEdgeInfo_FetchItemAt  (LEdgeInfo* This, LGraph_TEdge* inEdge, void* outItem);
void       LEdgeInfo_InstallDelEdgeHandler(LEdgeInfo* This, void (*inDealloc)
										                    (LEdgeInfo*, LGraph_TEdge*) );
void       LEdgeInfo_InstallNewEdgeHandler(LEdgeInfo* This, void (*inAlloc)
									                  (LEdgeInfo*, LGraph_TEdge*) );


/*PRIVATE FUNCTIONS */
ui4		   _LEdgeInfo_Panic	      (ui4 inValue);


#endif

/* Copyright (C) 2003 Stefano Emiliozzi, Camil Demetrescu

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

