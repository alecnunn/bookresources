/* ============================================================================
 *  LNodeInfo.h
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/02/07 10:46:14 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.9 $    
*/


#ifndef __LNodeInfo__
#define __LNodeInfo__

#include "LType.h"
#include "LArray.h"
#include "LGraph.h"

/* COMPONENT ID */
#define LNodeInfo_ID   0x802A

/* EXCEPTIONS */
enum 
{ 
    LNodeInfo_OBJECT_NULL_POINTER = LNodeInfo_ID<<16, 
    LNodeInfo_GRAPH_NULL_POINTER,
    LNodeInfo_NODE_NULL_POINTER,
    LNodeInfo_ITEM_NULL_POINTER,
	LNodeInfo_BASE_TYPE_MISMATCH
};


typedef struct LNodeInfo LNodeInfo;

struct LNodeInfo
{
    LGraph*		 mGraph;
    LArray*		 mData;
	LType_TType  mBaseType;
	void         (*mAlloc)  (LNodeInfo*, LGraph_TNode*);
	void         (*mDealloc)(LNodeInfo*, LGraph_TNode*);
};

/* PUBLIC MACROS */
#define LNodeInfo_ItemAt(n,i) LArray_ItemAt((n)->mData, (i)->mIndex)

/* MACROS TO ACCESS BASE TYPES */
#define LNodeInfo_UI1At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_UI1))? _LNodeInfo_Panic(0) : 0, ((ui1*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_UI2At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_UI2))? _LNodeInfo_Panic(0) : 0, ((ui2*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_UI4At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_UI4))? _LNodeInfo_Panic(0) : 0,((ui4*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_I1At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_I1))? _LNodeInfo_Panic(0) : 0, ((i1*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_I2At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_I2))? _LNodeInfo_Panic(0) : 0, ((i2*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_I4At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_I4))? _LNodeInfo_Panic(0) : 0, ((i4*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_F4At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_F4))? _LNodeInfo_Panic(0) : 0, ((f4*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_F8At(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_F8))? _LNodeInfo_Panic(0) : 0, ((f8*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_BoolAt(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_Bool))? _LNodeInfo_Panic(0) : 0, ((Bool*)LNodeInfo_ItemAt(n,i)) ))

#define LNodeInfo_PointerAt(n,i) \
	(*( !(LType_EqualTypes((n)->mBaseType, LType_Ptr))? _LNodeInfo_Panic(0) : 0, ((void*)LNodeInfo_ItemAt(n,i)) ))


/* PUBLIC FUNCTIONS */
LNodeInfo* LNodeInfo_New          (LGraph* inGraph, const LType_TType inType);
LNodeInfo* LNodeInfo_NewCustom    (LGraph* inGraph, ui4 inItemSize);
void LNodeInfo_Delete       (LNodeInfo** ThisA);
void LNodeInfo_AssignItemAt (LNodeInfo* This, LGraph_TNode* inNode, 
                                   const void* inItem);
Bool LNodeInfo_FetchItemAt  (LNodeInfo* This, LGraph_TNode* inNode, void* outItem);
void LNodeInfo_InstallDelNodeHandler(LNodeInfo* This, void (*inDealloc)
									                  (LNodeInfo*, LGraph_TNode*) );
void LNodeInfo_InstallNewNodeHandler(LNodeInfo* This, void (*inAlloc)
									                  (LNodeInfo*, LGraph_TNode*) );

/*PRIVATE FUNCTIONS */
ui4		   _LNodeInfo_Panic	      (ui4 inValue);


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

