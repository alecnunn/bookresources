/* ============================================================================
 *  LEdgeInfo.c
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/02/07 10:46:14 $
 *  Changed by:     $Author: stefano $	 
 *  Revision:       $Revision: 1.10 $	
*/

#include"_LEdgeInfo.h"
#include"_LGraph.h"
#include"LMemory.h"
#include"LException.h"

/* ---------------------------------------------------------------------------------
*  LEdgeInfo_New
*  ---------------------------------------------------------------------------------
*  Constructor */
LEdgeInfo* LEdgeInfo_New(LGraph* inGraph, const LType_TType inType)
{
	LEdgeInfo theObject = {0};
	LEdgeInfo* theEdgeInfo = NULL;
	ui4 theCount;
	LException* theException;

    if (inGraph == NULL) Throw(LEdgeInfo_GRAPH_NULL_POINTER);

	Try
	{
		theObject.mData     = LArray_New(inType.mSize);
		theObject.mGraph    = inGraph;
		theObject.mBaseType = inType;
		theEdgeInfo = LMemory_NewObject(LEdgeInfo, theObject);
		theCount = LGraph_GetEdgesCount(inGraph);
		LArray_ResizeBy(theEdgeInfo->mData, theCount);
		_LGraph_RegisterEdgeInfo(theEdgeInfo->mGraph, theEdgeInfo);
	}
	Catch(theException)
	{/* if something goes wrong, do cleanup */
		if ( theEdgeInfo != NULL ) 
		{
			if ( theEdgeInfo->mData != NULL ) LArray_Delete( &(theEdgeInfo->mData) );
			LMemory_DeleteObject(&theEdgeInfo);
		}
		LException_Dump(theException);
	}

	return theEdgeInfo;
}


/* ---------------------------------------------------------------------------------
*  LEdgeInfo_NewCustom
*  ---------------------------------------------------------------------------------
*  Constructor for a user defined type */
LEdgeInfo* LEdgeInfo_NewCustom(LGraph* inGraph, ui4 inItemSize)
{
	LEdgeInfo theObject = {0};
	LEdgeInfo* theEdgeInfo = NULL;
	ui4 theCount;
	LException* theException;

    if (inGraph == NULL) Throw(LEdgeInfo_GRAPH_NULL_POINTER);

	Try
	{
		theObject.mData     = LArray_New(inItemSize);
		theObject.mGraph    = inGraph;
		theEdgeInfo = LMemory_NewObject(LEdgeInfo, theObject);
		theCount = LGraph_GetEdgesCount(inGraph);
		LArray_ResizeBy(theEdgeInfo->mData, theCount);
		_LGraph_RegisterEdgeInfo(theEdgeInfo->mGraph, theEdgeInfo);
	}
	Catch(theException)
	{/* if something goes wrong, do cleanup */
		LException_Dump(theException);
		if ( theEdgeInfo != NULL ) 
		{
			if ( theEdgeInfo->mData != NULL ) LArray_Delete( &(theEdgeInfo->mData) );
			LMemory_DeleteObject(&theEdgeInfo);
		}
	}
	return theEdgeInfo;
}
/* ---------------------------------------------------------------------------------
*  LEdgeInfo_Delete
*  ---------------------------------------------------------------------------------
*  Destructor */
void LEdgeInfo_Delete(LEdgeInfo** ThisA)
{
	LException* theException;
        
    if (ThisA == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);
    if ((*ThisA) == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);

	_LEdgeInfo_CallItemsDestructor((*ThisA));
		
	Try
		LArray_Delete( &( (*ThisA)->mData ) );
	Catch(theException)
		LException_Dump(theException);
	LMemory_DeleteObject(ThisA);
}


/* ---------------------------------------------------------------------------------
*  LEdgeInfo_AssignItemAt
*  ---------------------------------------------------------------------------------
*  Assigns the data pointed by inItem to the Edge inEdge */
void LEdgeInfo_AssignItemAt(LEdgeInfo* This, LGraph_TEdge* inEdge, const void* inItem)
{ /*at this point there MUST be the allocated space for the item */
	LException* theException;
	void* thePtr;
	ui4   theItemSize;

    if (This == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);
    if (inEdge == NULL) Throw(LEdgeInfo_EDGE_NULL_POINTER);
    if (inItem == NULL) Throw(LEdgeInfo_ITEM_NULL_POINTER);

	Try
	{
		thePtr = LArray_ItemAt(This->mData, inEdge->mIndex);
		theItemSize = LArray_GetItemSize(This->mData);
	}
	Catch(theException)
		LException_Dump(theException);

	LMemory_Copy(inItem, thePtr, theItemSize);
}

/* ---------------------------------------------------------------------------------
*  LEdgeInfo_FetchItemAt
*  ---------------------------------------------------------------------------------
*  Returns the info of a specified Edge in the memory pointed by out item *
*  NOTE: only if the methods returns TRUE the data are valid			  */
Bool LEdgeInfo_FetchItemAt  (LEdgeInfo* This, LGraph_TEdge* inEdge, void* outItem)
{
	LException* theException;
	Bool theResult;

    if (This == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);
    if (inEdge == NULL) Throw(LEdgeInfo_EDGE_NULL_POINTER);
    if (outItem == NULL) Throw(LEdgeInfo_ITEM_NULL_POINTER);

	Try
		theResult = LArray_FetchItemAt(This->mData, inEdge->mIndex, outItem);
	Catch(theException)
		LException_Dump(theException);

	return theResult;
}

/* ---------------------------------------------------------------------------------
*  LEdgeInfo_InstallDelEdgeHandler
*  ---------------------------------------------------------------------------------
*  Installs a delete edge handler */
void LEdgeInfo_InstallDelEdgeHandler(LEdgeInfo* This, 
									 void (*inDealloc)(LEdgeInfo*, LGraph_TEdge*) )
{ 
	if (This == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);
	This->mDealloc = inDealloc; 
}

void LEdgeInfo_InstallNewEdgeHandler(LEdgeInfo* This, 
									 void (*inAlloc)(LEdgeInfo*, LGraph_TEdge*) )
{ 
	if (This == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);
	This->mAlloc = inAlloc; 
}

/* PRIVATE METHODS */
/* ---------------------------------------------------------------------------------
*  _LEdgeInfo_DeleteItemAt
*  ---------------------------------------------------------------------------------
*  Delete the info of a specified Edge*/
void _LEdgeInfo_DeleteItemAt(LEdgeInfo* This, LGraph_TEdge* inEdge) 
{
	LException* theException;
	ui4 theIndex;

    if (This == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);

	/* if installed, calls the destructor handler */
	if (This->mDealloc)
		(*(This->mDealloc))(This, inEdge);

	theIndex = inEdge->mIndex;
    LMemory_Copy(LArray_LastItem(This->mData),
                  LArray_ItemAt(This->mData, theIndex),
                  LArray_GetItemSize(This->mData));
	Try
		LArray_ResizeBy(This->mData, -1);
	Catch(theException)
		LException_Dump(theException);
}

/* ---------------------------------------------------------------------------------
*  _LEdgeInfo_AddInfo();
*  ---------------------------------------------------------------------------------
*  Makes room for a new info in the Edge */
void _LEdgeInfo_AddInfo(LEdgeInfo* This, LGraph_TEdge* inEdge)
{ 
	LException* theException;
	
    if (This == NULL) Throw(LEdgeInfo_OBJECT_NULL_POINTER);

	Try
		LArray_ResizeBy(This->mData, 1); 
	Catch(theException)
		LException_Dump(theException);
	/* if the newHandler has been registered, call it */
	if (This->mAlloc)
		(*(This->mAlloc))(This, inEdge);

}

/* ---------------------------------------------------------------------------------
*  _LEdgeInfo_Panic();
*  ---------------------------------------------------------------------------------
*  Used to throw exceptions */
ui4 _LEdgeInfo_Panic(ui4 inValue)
{ 
	Throw(LEdgeInfo_BASE_TYPE_MISMATCH); 
	return inValue;
}

/* ---------------------------------------------------------------------------------
*  _LEdgeInfo_CallItemsDestructor();
*  ---------------------------------------------------------------------------------
*  Used to call the proper items destructor, if registered */
void _LEdgeInfo_CallItemsDestructor(LEdgeInfo* This)
{ 
	LGraph_TEdge* theEdge;

	if ( This->mDealloc ) 
	{/* we have to destroy all the items with the proper destructor */
		LGraph_ForAllEdges(This->mGraph, theEdge)
			(*(This->mDealloc))(This, theEdge);
	}
	if (This->mGraph) 
	{
		_LGraph_UnregisterEdgeInfo(This->mGraph, This);
		This->mGraph = NULL;
	}
	This->mDealloc = NULL;
}

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
