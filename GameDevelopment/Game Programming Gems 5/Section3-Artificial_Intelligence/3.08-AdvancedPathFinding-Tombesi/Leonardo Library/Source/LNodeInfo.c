/* ============================================================================
 *  LNodeInfo.c
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/02/07 10:46:14 $
 *  Changed by:     $Author: stefano $	 
 *  Revision:       $Revision: 1.10 $	
*/

#include"_LNodeInfo.h"
#include"_LGraph.h"
#include"LMemory.h"
#include"LException.h"

/* ---------------------------------------------------------------------------------
*  LNodeInfo_New
*  ---------------------------------------------------------------------------------
*  Constructor */
LNodeInfo* LNodeInfo_New(LGraph* inGraph, const LType_TType inType)
{
	LNodeInfo theObject = {0};
	LNodeInfo* theNodeInfo = NULL;
	ui4 theCount;
	LException* theException;

    if (inGraph == NULL) Throw(LNodeInfo_GRAPH_NULL_POINTER);

	Try
	{
		theObject.mData     = LArray_New(inType.mSize);
		theObject.mGraph    = inGraph;
		theObject.mBaseType = inType;
		theNodeInfo = LMemory_NewObject(LNodeInfo, theObject);
		theCount = LGraph_GetNodesCount(inGraph);
		LArray_ResizeBy(theNodeInfo->mData, theCount);
		_LGraph_RegisterNodeInfo(theNodeInfo->mGraph, theNodeInfo);
	}
	Catch(theException)
	{/* if something goes wrong, do cleanup */
		if ( theNodeInfo != NULL ) 
		{
			if ( theNodeInfo->mData != NULL ) LArray_Delete( &(theNodeInfo->mData) );
			LMemory_DeleteObject(&theNodeInfo);
		}
		LException_Dump(theException);
	}

	return theNodeInfo;
}


/* ---------------------------------------------------------------------------------
*  LNodeInfo_NewCustom
*  ---------------------------------------------------------------------------------
*  Constructor for a user defined type */
LNodeInfo* LNodeInfo_NewCustom(LGraph* inGraph, ui4 inItemSize)
{
	LNodeInfo theObject = {0};
	LNodeInfo* theNodeInfo = NULL;
	ui4 theCount;
	LException* theException;

    if (inGraph == NULL) Throw(LNodeInfo_GRAPH_NULL_POINTER);

	Try
	{
		theObject.mData     = LArray_New(inItemSize);
		theObject.mGraph    = inGraph;
		theNodeInfo = LMemory_NewObject(LNodeInfo, theObject);
		theCount = LGraph_GetNodesCount(inGraph);
		LArray_ResizeBy(theNodeInfo->mData, theCount);
		_LGraph_RegisterNodeInfo(theNodeInfo->mGraph, theNodeInfo);
	}
	Catch(theException)
	{/* if something goes wrong, do cleanup */
		LException_Dump(theException);
		if ( theNodeInfo != NULL ) 
		{
			if ( theNodeInfo->mData != NULL ) LArray_Delete( &(theNodeInfo->mData) );
			LMemory_DeleteObject(&theNodeInfo);
		}
	}
	return theNodeInfo;
}
/* ---------------------------------------------------------------------------------
*  LNodeInfo_Delete
*  ---------------------------------------------------------------------------------
*  Destructor */
void LNodeInfo_Delete(LNodeInfo** ThisA)
{
	LException*		theException;
        
    if (ThisA == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);
    if ((*ThisA) == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);

	_LNodeInfo_CallItemsDestructor(*(ThisA));

	Try
		LArray_Delete( &( (*ThisA)->mData ) );
	Catch(theException)
		LException_Dump(theException);
	
	LMemory_DeleteObject(ThisA);
}


/* ---------------------------------------------------------------------------------
*  LNodeInfo_AssignItemAt
*  ---------------------------------------------------------------------------------
*  Assigns the data pointed by inItem to the node inNode */
void LNodeInfo_AssignItemAt(LNodeInfo* This, LGraph_TNode* inNode, const void* inItem)
{ /*at this point there MUST be the allocated space for the item */
	LException* theException;
	void* thePtr;
	ui4   theItemSize;

    if (This == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);
    if (inNode == NULL) Throw(LNodeInfo_NODE_NULL_POINTER);
    if (inItem == NULL) Throw(LNodeInfo_ITEM_NULL_POINTER);

	Try
	{
		thePtr = LArray_ItemAt(This->mData, inNode->mIndex);
		theItemSize = LArray_GetItemSize(This->mData);
	}
	Catch(theException)
		LException_Dump(theException);

	LMemory_Copy(inItem, thePtr, theItemSize);
}

/* ---------------------------------------------------------------------------------
*  LNodeInfo_FetchItemAt
*  ---------------------------------------------------------------------------------
*  Returns the info of a specified node in the memory pointed by out item *
*  NOTE: only if the methods returns TRUE the data are valid			  */
Bool LNodeInfo_FetchItemAt  (LNodeInfo* This, LGraph_TNode* inNode, void* outItem)
{
	LException* theException;
	Bool theResult;

    if (This == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);
    if (inNode == NULL) Throw(LNodeInfo_NODE_NULL_POINTER);
    if (outItem == NULL) Throw(LNodeInfo_ITEM_NULL_POINTER);

	Try
		theResult = LArray_FetchItemAt(This->mData, inNode->mIndex, outItem);
	Catch(theException)
		LException_Dump(theException);

	return theResult;
}


/* ---------------------------------------------------------------------------------
*  LNodeInfo_InstallNewNodeHandler, _InstallDelNodeHandler 
*  ---------------------------------------------------------------------------------
*  Installs a new/delete node handler */
void LNodeInfo_InstallDelNodeHandler(LNodeInfo* This, 
									 void (*inDealloc)(LNodeInfo*, LGraph_TNode*) )
{ 
	if (This == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);
	This->mDealloc = inDealloc; 
}

void LNodeInfo_InstallNewNodeHandler(LNodeInfo* This, 
									 void (*inAlloc)(LNodeInfo*, LGraph_TNode*) )
{ 
	if (This == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);
	This->mAlloc = inAlloc; 
}

/* PRIVATE METHODS */

/* ---------------------------------------------------------------------------------
*  _LNodeInfo_DeleteItemAt
*  ---------------------------------------------------------------------------------
*  Deletes the info of a specified node*/
void _LNodeInfo_DeleteItemAt(LNodeInfo* This, LGraph_TNode* inNode) 
{
	ui4 theIndex;

	LException* theException;

    if (This == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);
	
	/* if installed, calls the destructor handler */
	if (This->mDealloc)
		(*(This->mDealloc))(This, inNode);
    
	theIndex = inNode->mIndex;
    LMemory_Copy(LArray_LastItem(This->mData),
                  LArray_ItemAt(This->mData, theIndex),
                  LArray_GetItemSize(This->mData));
	Try
		LArray_ResizeBy(This->mData, -1);
	Catch(theException)
		LException_Dump(theException);
}

/* ---------------------------------------------------------------------------------
*  _LNodeInfo_AddInfo();
*  ---------------------------------------------------------------------------------
*  Makes room for a new info in the node */
void _LNodeInfo_AddInfo(LNodeInfo* This, LGraph_TNode* inNode)
{ 
	LException* theException;
	
    if (This == NULL) Throw(LNodeInfo_OBJECT_NULL_POINTER);

	Try
		LArray_ResizeBy(This->mData, 1); 
	Catch(theException)
		LException_Dump(theException);
	/* if the newHandler has been registered, call it */
	if (This->mAlloc)
		(*(This->mAlloc))(This, inNode);
}

/* ---------------------------------------------------------------------------------
*  _LNodeInfo_Panic();
*  ---------------------------------------------------------------------------------
*  Used to throw exceptions */
ui4 _LNodeInfo_Panic(ui4 inValue)
{ 
	Throw(LNodeInfo_BASE_TYPE_MISMATCH); 
	return inValue;
}

/* ---------------------------------------------------------------------------------
*  _LNodeInfo_CallItemsDestructor();
*  ---------------------------------------------------------------------------------
*  Used to call the proper items destructor, if registered */
void _LNodeInfo_CallItemsDestructor(LNodeInfo* This)
{
	LGraph_TNode* theNode;

	if ( This->mDealloc ) 
	{/* we have to destroy all the items with the proper destructor */
		LGraph_ForAllNodes(This->mGraph, theNode)
			(*(This->mDealloc))(This, theNode);
	}
	if (This->mGraph) 
	{
		_LGraph_UnregisterNodeInfo(This->mGraph, This);
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
