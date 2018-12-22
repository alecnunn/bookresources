/* ============================================================================
 *  LGraphGen_RandomGrid.c
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi
 *  License:        See the end of this file for license information
 *  Created:        January 21, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/26 13:23:27 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.3 $    
*/

#include "LGraphGen.h"
#include "LException.h"
#include "LMemory.h"

/* ---------------------------------------------------------------------------------
 *  LGraphGen_RandomGrid
 * ---------------------------------------------------------------------------------
 * Returns a random graph which represents a n * n grid*/

LGraph* LGraphGen_RandomGrid(ui4 inN, Bool inDirected)
{
	ui4 x;
	ui4 y;
	LException* theEx;
	LGraph_TNode*** theMatrix;
	LGraph* theGraph = NULL;
  
	Try
	{		
		theGraph = LGraph_New(inDirected);
		theMatrix = LMemory_Calloc(inN*sizeof(LGraph_TNode**));
		for (x = 0; x < inN; x++)
			theMatrix[x] = LMemory_Calloc(inN*sizeof(LGraph_TNode*));
	}
		
    Catch(theEx)
	{/* if something goes wrong, do cleanup */
		if (theGraph) LGraph_Delete(&theGraph);
		for (x = 0; x < inN; x++)
			LMemory_Free(&theMatrix[x]);
		LMemory_Free(&theMatrix);
		Rethrow;
	}

	for(y=0; y<inN; y++)
		for(x=0; x<inN; x++)
			theMatrix[x][y] = LGraph_NewNode(theGraph);
       
	for(x=0; x<inN; x++)
		for(y=0; y<inN; y++)
		{ 
			if (x < inN-1) LGraph_NewEdge(theGraph, theMatrix[x][y], theMatrix[x+1][y]);
			if (y < inN-1) LGraph_NewEdge(theGraph, theMatrix[x][y], theMatrix[x][y+1]);
		}
	for (x = 0; x < inN; x++)
			LMemory_Free(&theMatrix[x]);
	LMemory_Free(&theMatrix);
	return theGraph;
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
