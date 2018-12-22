/* ============================================================================
 *  LEdgeMap.h
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 24, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/02/04 14:37:23 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.2 $    
*/


#ifndef __LEdgeMap__
#define __LEdgeMap__

#include "LType.h"
#include "LGraph.h"

/* COMPONENT ID */
#define LEdgeMap_ID   0x8030

/* EXCEPTIONS */
enum 
{ 
    LEdgeMap_OBJECT_NULL_POINTER = LEdgeMap_ID<<16, 
    LEdgeMap_GRAPH_NULL_POINTER,
    LEdgeMap_NODE_NULL_POINTER
};

typedef struct LEdgeMap LEdgeMap;

/* PUBLIC FUNCTIONS */
LEdgeMap* LEdgeMap_New        (LGraph* inGraph);
void      LEdgeMap_Delete     (LEdgeMap** ThisA);
Bool      LEdgeMap_EdgeExists (LEdgeMap* This, LGraph_TNode* inSrc, 
							                   LGraph_TNode* inDst);

LGraph_TEdge* LEdgeMap_GetEdge(LEdgeMap* This, LGraph_TNode* inSrc, 
							                   LGraph_TNode* inDst);
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

