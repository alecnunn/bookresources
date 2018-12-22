
/* ============================================================================
 *  LGraph_Gen.c
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/31 15:54:44 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.3 $    
*/

#include "LGraphGen.h"
#include "LException.h"
#include "LRandSource.h"
#include "LEdgeMap.h"

/* ---------------------------------------------------------------------------------
*  LGraphGen_RndEdgeInfoUI4
*  ---------------------------------------------------------------------------------
*  Returns a LEdgeInfo associated to inGraph with random weights from inA to inB  */
LEdgeInfo* LGraphGen_RndEdgeInfoUI4(LGraph* inGraph, ui4 inA, ui4 inB)
{
	ui4			  i;
	ui4			  theCount;
	ui4			  theRnd;
	LGraph_TEdge* theEdge;
	LArray*       theEdgesList = NULL;
	LRandSource*  theRndSrc    = NULL;
	LEdgeInfo*	  theEdgeInfo  = NULL;
	ui4			  theSeed      = 777;

	Try
	{
		theRndSrc    = LRandSource_New(theSeed);
		theEdgeInfo	 = LEdgeInfo_New(inGraph, LType_UI4);
		theEdgesList = LGraph_GetAllEdges(inGraph);
	}
	CatchAny
	{/* cleanup */
		if (theRndSrc) LRandSource_Delete(&theRndSrc);
		if (theEdgeInfo) LEdgeInfo_Delete(&theEdgeInfo);
		if (theEdgesList) LArray_Delete(&theEdgesList);
	}
	
	theCount = LArray_GetItemsCount(theEdgesList);
	for (i=0; i < theCount; i++)
	{
		theRnd = LRandSource_GetRand(theRndSrc, inA, inB);
		theEdge = *(LGraph_TEdge**)LArray_ItemAt(theEdgesList, i);
		LEdgeInfo_UI4At(theEdgeInfo, theEdge) = theRnd;
	}
	
	LRandSource_Delete(&theRndSrc);
	LArray_Delete(&theEdgesList);
	return theEdgeInfo;	
}

/* ---------------------------------------------------------------------------------
*  LGraphGen_AddCycle
*  ---------------------------------------------------------------------------------
*  Adds a cycle creating a path through all nodes */

void LGraphGen_AddCycle(LGraph* inGraph)
{
	ui4           i;
	ui4           theCount;
	LGraph_TNode* theSource;
	LGraph_TNode* theTarget;
	LEdgeMap*     theMap      = LEdgeMap_New(inGraph);
	LArray*       theNodeList = LGraph_GetAllNodes(inGraph);

	theCount = LArray_GetItemsCount(theNodeList);

	for (i=0; i < theCount-1; i++)
	{
		theSource = *(LGraph_TNode**)LArray_ItemAt(theNodeList, i);
		theTarget = *(LGraph_TNode**)LArray_ItemAt(theNodeList, i+1);

		if (!LEdgeMap_EdgeExists(theMap, theSource, theTarget))
			LGraph_NewEdge(inGraph, theSource, theTarget);

	}
	/* adds the last edge */
	theSource = *(LGraph_TNode**)LArray_ItemAt(theNodeList, i);
	theTarget = *(LGraph_TNode**)LArray_ItemAt(theNodeList, 0);

	if (!LEdgeMap_EdgeExists(theMap, theSource, theTarget))
		LGraph_NewEdge(inGraph, theSource, theTarget);

	LArray_Delete(&theNodeList);
	LEdgeMap_Delete(&theMap);
}

/* Copyright (C) 2003 Stefano Emiliozzi

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