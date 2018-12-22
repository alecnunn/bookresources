/* ============================================================================
 *  LEdgeMap.c
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 24, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/02/04 14:37:23 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.7 $    
*/


#include "_LEdgeMap.h"
#include "LHash.h"
#include "LArray.h"
#include "LNodeInfo.h"
#include "LException.h"
#include "LMemory.h"

/* Private methods declarations */

void _Sync(LEdgeMap* This, LGraph* inGraph);
void _Alloc(LNodeInfo* inNodeInfo, LGraph_TNode* inNode);
void _Dealloc(LNodeInfo* inNodeInfo, LGraph_TNode* inNode);


/* ---------------------------------------------------------------------------------
*  LEdgeInfo_New
*  ---------------------------------------------------------------------------------
*  Constructor */
LEdgeMap* LEdgeMap_New(LGraph* inGraph)
{
	LEdgeMap theObject = {0};
	LEdgeMap* theEdgeMap;
		
	if (!inGraph) Throw(LEdgeMap_GRAPH_NULL_POINTER);
	Try
	{
		theObject.mNodeInfo  = LNodeInfo_New(inGraph, LType_Ptr);
		theObject.mGraph     = inGraph;
	}
	CatchAny
	{
		if (theObject.mNodeInfo)  LNodeInfo_Delete(&(theObject.mNodeInfo));
		Rethrow;
	}

	theEdgeMap = LMemory_NewObject(LEdgeMap, theObject);
	_Sync(theEdgeMap, inGraph);
	_LGraph_RegisterEdgeMap(inGraph, theEdgeMap);
	LNodeInfo_InstallDelNodeHandler(theEdgeMap->mNodeInfo, _Dealloc);
	LNodeInfo_InstallNewNodeHandler(theEdgeMap->mNodeInfo, _Alloc);
	return theEdgeMap;
}


/* ---------------------------------------------------------------------------------
*  LEdgeMap_Delete
*  ---------------------------------------------------------------------------------
*  Destructor */
void LEdgeMap_Delete(LEdgeMap** ThisA)
{
	if (!ThisA)    Throw(LEdgeMap_OBJECT_NULL_POINTER);
	if (!(*ThisA)) Throw(LEdgeMap_OBJECT_NULL_POINTER);
	
	if ( (*ThisA)->mGraph)
		_LGraph_UnregisterEdgeMap( (*ThisA)->mGraph);

	LNodeInfo_Delete(&((*ThisA)->mNodeInfo));
	LMemory_DeleteObject(ThisA);
}


/* ---------------------------------------------------------------------------------
*  LEdgeMap_EdgeExist
*  ---------------------------------------------------------------------------------
*  Returns TRUE <-> there's an edge from inSrc to inDst */
Bool LEdgeMap_EdgeExists(LEdgeMap* This, LGraph_TNode* inSrc, LGraph_TNode* inDst)
{
	LHash* theHash;

	if (!This) Throw(LEdgeMap_OBJECT_NULL_POINTER);
	if ( (!inSrc) || (!inDst) ) Throw(LEdgeMap_NODE_NULL_POINTER);

	theHash = *(LHash**)LNodeInfo_ItemAt(This->mNodeInfo, inSrc);
	return LHash_IsInTable(theHash, (ui4)inDst);
}

/* ---------------------------------------------------------------------------------
*  LEdgeMap_GetEdge
*  ---------------------------------------------------------------------------------
*  Returns the edge from inSrc to inDst, NULL if there's no edge */
LGraph_TEdge* LEdgeMap_GetEdge(LEdgeMap* This, LGraph_TNode* inSrc, LGraph_TNode* inDst)
{
	LHash* theHash;
	LGraph_TEdge* theEdge = NULL;

	if (!This) Throw(LEdgeMap_OBJECT_NULL_POINTER);
	if ( (!inSrc) || (!inDst) ) Throw(LEdgeMap_NODE_NULL_POINTER);

	theHash = *(LHash**)LNodeInfo_ItemAt(This->mNodeInfo, inSrc);
	if (LHash_IsInTable(theHash, (ui4)inDst))
		theEdge =  (LGraph_TEdge*)LHash_GetItemByKey(theHash, (ui4)inDst);
	return theEdge;
}

/* PRIVATE METHODS */

/* ---------------------------------------------------------------------------------
*  _Sync
*  ---------------------------------------------------------------------------------
*  Syncronizes the LEdgeMap with the bounded Graph */
void _Sync(LEdgeMap* This, LGraph* inGraph)
{
	LHash* theHash;
	ui4 theNodesCount;
	ui4 theAdjNodesCnt;
	LArray* theNodesList;
	LArray* theAdjList;
	LGraph_TNode* theNode;
	LGraph_TNode* theAdjNode;
	LGraph_TEdge* theEdge;
	ui4 i;
	ui4 j;

	theNodesCount = LGraph_GetNodesCount(This->mGraph);
	theNodesList  = LGraph_GetAllNodes(This->mGraph);

	for (i=0; i < theNodesCount; i++)
	{
		theNode = *(LGraph_TNode**)LArray_ItemAt(theNodesList, i);
		theAdjList = LGraph_GetOutEdges(inGraph, theNode);
		theAdjNodesCnt = LArray_GetItemsCount(theAdjList);
		theHash = LHash_New();
		for (j=0; j < theAdjNodesCnt; j++)
		{
			theEdge = *(LGraph_TEdge**)LArray_ItemAt(theAdjList, j);
			theAdjNode = (theNode == theEdge->mSource)? theEdge->mTarget : theEdge->mSource;
			LHash_InsertItem(theHash, (ui4)theEdge, (ui4)theAdjNode);
		}
		LNodeInfo_AssignItemAt(This->mNodeInfo, theNode, &theHash);
		LArray_Delete(&theAdjList);
	}
	LArray_Delete(&theNodesList);
}	

/* ---------------------------------------------------------------------------------
*  _LEdgeMapAddEdge
*  ---------------------------------------------------------------------------------
*  Adds an edge from the LEdgeMap */
void _LEdgeMap_AddEdge(LEdgeMap* This, LGraph_TEdge* inEdge)
{
	LHash* theHash;

	theHash = *(LHash**)LNodeInfo_ItemAt(This->mNodeInfo, inEdge->mSource);
	LHash_InsertItem(theHash, (ui4)inEdge, (ui4)(inEdge->mTarget));
}

/* ---------------------------------------------------------------------------------
*  _LEdgeMapDeleteEdge
*  ---------------------------------------------------------------------------------
*  Removes an edge from the LEdgeMap */
void _LEdgeMap_DeleteEdge(LEdgeMap* This, LGraph_TEdge* inEdge)
{
	LHash* theHash;

	theHash = *(LHash**)LNodeInfo_ItemAt(This->mNodeInfo, inEdge->mSource);
	LHash_RemoveItem(theHash, (ui4)(inEdge->mTarget));
}


/* ---------------------------------------------------------------------------------
*  _Alloc
*  ---------------------------------------------------------------------------------
*  Handler to properly allocate the data structure used in LNodeInfo */
void _Alloc(LNodeInfo* inNodeInfo, LGraph_TNode* inNode)
{
	LHash* theHash = LHash_New();
	LNodeInfo_AssignItemAt(inNodeInfo, inNode, &theHash);
}

/* ---------------------------------------------------------------------------------
*  _Dealloc
*  ---------------------------------------------------------------------------------
*  Handler to properly deallocate the data structure used in LNodeInfo */
void _Dealloc(LNodeInfo* inNodeInfo, LGraph_TNode* inNode)
{
	LHash* theHash = *(LHash**)LNodeInfo_ItemAt(inNodeInfo, inNode);
	LHash_Delete(&theHash);
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

