/* ============================================================================
 *  _LGraph.h
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/26 13:23:27 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.3 $    
*/

#ifndef ___LGraph__
#define ___LGraph__

#include"LGraph.h"
#include"_LNodeInfo.h"
#include"_LEdgeInfo.h"
#include"_LEdgeMap.h"

struct LGraph 
{
     Bool          mDirected;
     LGraph_TNode* mFirstNode;
     LGraph_TNode* mLastNode;
     LGraph_TEdge* mFirstEdge;
     LGraph_TEdge* mLastEdge;
     ui4           mNodesCount;
     ui4           mEdgesCount;
     LNodeInfo**   mNodeList;
     LArray*       mNodeListArray;
     LEdgeInfo**   mEdgeList;
     LArray*       mEdgeListArray;
	 LEdgeMap*	   mEdgeMap;
};

void _LGraph_RegisterNodeInfo(LGraph* inGraph, LNodeInfo* inNodeInfo);
void _LGraph_UnregisterNodeInfo(LGraph* inGraph, LNodeInfo* inNodeInfo);

void _LGraph_RegisterEdgeInfo(LGraph* inGraph, LEdgeInfo* inEdgeInfo);
void _LGraph_UnregisterEdgeInfo(LGraph* inGraph, LEdgeInfo* inEdgeInfo);

void _LGraph_RegisterEdgeMap(LGraph* inGraph, LEdgeMap* inEdgeMap);
void _LGraph_UnregisterEdgeMap(LGraph* inGraph);

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



