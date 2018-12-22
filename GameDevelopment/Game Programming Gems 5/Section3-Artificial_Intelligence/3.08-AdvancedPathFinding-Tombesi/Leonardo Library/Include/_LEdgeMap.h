/* ============================================================================
 *  _LEdgeMap.h
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 24, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/28 17:13:05 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.3 $    
*/


#ifndef ___LEdgeMap__
#define ___LEdgeMap__

#include "LEdgeMap.h"
#include "LNodeInfo.h"
#include "_LGraph.h"

struct LEdgeMap
{
	LNodeInfo* mNodeInfo;
	LGraph*    mGraph;
};

void _LEdgeMap_AddEdge   (LEdgeMap* This, LGraph_TEdge* inEdge);
void _LEdgeMap_DeleteEdge(LEdgeMap* This, LGraph_TEdge* inEdge);

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

