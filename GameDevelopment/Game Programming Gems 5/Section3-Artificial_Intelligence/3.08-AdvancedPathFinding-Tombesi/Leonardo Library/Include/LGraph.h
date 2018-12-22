/* ============================================================================
 *  LGraph.h
 * ============================================================================

 *  Author:         (C) 2003 Stefano Emiliozzi, Camil Demetrescu
 *  License:        See the end of this file for license information
 *  Created:        January 12, 2003
 *  Module:         LL

 *  Last changed:   $Date: 2003/01/26 13:23:27 $
 *  Changed by:     $Author: stefano $   
 *  Revision:       $Revision: 1.8 $    
*/


#ifndef __LGraph__
#define __LGraph__

#include "LType.h"
#include "LArray.h"

/* COMPONENT ID */
#define LGraph_ID   0x8029

/* EXCEPTIONS */
enum 
{ 
    LGraph_OBJECT_NULL_POINTER = LGraph_ID<<16, 
	LGraph_NODE_NULL_POINTER,
	LGraph_EDGE_NULL_POINTER,
	LGraph_NODE_NOT_IN_GRAPH,
	LGraph_EDGE_NOT_IN_GRAPH
};

#define LGraph_DIRECTED     1
#define LGraph_UNDIRECTED   0

typedef struct LGraph LGraph;
typedef struct LGraph_TEdge LGraph_TEdge;
typedef struct LGraph_TNode LGraph_TNode;

/* 36 byte + internal LMemory block header (approx. 8 byte) */
struct LGraph_TEdge 
{
     ui4           mIndex;    /* uniquue 32bit identifier                    */
     LGraph_TNode* mSource;   /* pointer to start node                       */
     LGraph_TNode* mTarget;   /* pointer to target node                      */
     LGraph_TEdge* mNext;     /* pointer to next edge in edge list           */
     LGraph_TEdge* mPrev;     /* pointer to prev edge in edge list           */
     LGraph_TEdge* mNextSource;  /* pointer to next edge in out-list of mSource */
     LGraph_TEdge* mPrevSource;  /* pointer to prev edge in out-list of mSource */
     LGraph_TEdge* mNextTarget;   /* pointer to next edge in in-list of mTarget  */
     LGraph_TEdge* mPrevTarget;   /* pointer to prev edge in in-list of mTarget  */
};

/* 36 byte + internal LMemory block header (approx. 8 byte) */
struct LGraph_TNode 
{
     ui4           mIndex;    /* uniquue 32bit identifier                    */
     ui4           mInDeg;    /* fan-in of the node                          */
     ui4           mOutDeg;   /* fan-out of the node                         */
     LGraph_TEdge* mFirstOut; /* pointer to first edge in out-list           */
     LGraph_TEdge* mLastOut;  /* pointer to last edge in out-list            */
     LGraph_TEdge* mFirstIn;  /* pointer to first edge in in-list            */
     LGraph_TEdge* mLastIn;   /* pointer to last edge in in-list             */
     LGraph_TNode* mNext;     /* pointer to next node in node list           */
     LGraph_TNode* mPrev;     /* pointer to prev node in node list           */
};

/* Public iterators */
#define LGraph_GetNextNode(v) ((v)->mNext)

#define LGraph_ForAllNodes(g,v) \
        for ( (v)=LGraph_GetFirstNode(g) ; (v)!=NULL ; (v)=(v)->mNext )

#define LGraph_GetNextEdge(v) ((v)->mNext)

#define LGraph_ForAllEdges(g,e) \
     for ( (e)=LGraph_GetFirstEdge(g) ; (e)!=NULL ; (e)=(e)->mNext )

#define LGraph_GetSource(e)      ((e)->mSource)

#define LGraph_GetTarget(e)      ((e)->mTarget)

#define LGraph_GetFirstOutEdge(v) ((v)->mFirstOut)

#define LGraph_GetNextOutEdge(v,e)  \
     ((e)->mSource == (v) ? (e)->mNextSource : (e)->mNextTarget)

#define LGraph_ForAllOut(v,e)    \
     for ( (e) = (v)->mFirstOut; (e)!=NULL ; (e) = LGraph_GetNextOutEdge(v,e) )

#define LGraph_GetFirstInEdge(v)  ((v)->mFirstIn)

#define LGraph_GetNextInEdge(v,e) LGraph_GetNextOutEdge(v,e)

#define LGraph_ForAllIn(v,e)  \
        for ((e) = (v)->mFirstIn;  (e)!=NULL ; (e) = LGraph_GetNextInEdge(v,e) )

/* Public Interface Methods */
LGraph*       LGraph_New           (Bool inDirected);
void          LGraph_Delete        (LGraph** AThis);
LGraph_TNode* LGraph_NewNode       (LGraph* This);
void          LGraph_DelNode       (LGraph* This, LGraph_TNode* inNode);
LGraph_TEdge* LGraph_NewEdge       (LGraph* This, LGraph_TNode* inSource, LGraph_TNode* inTarget);
void          LGraph_DelEdge       (LGraph* This, LGraph_TEdge* inEdge);
LGraph_TNode* LGraph_GetFirstNode  (LGraph* This);
LGraph_TNode* LGraph_GetLastNode   (LGraph* This);
LGraph_TEdge* LGraph_GetFirstEdge  (LGraph* This);
LGraph_TEdge* LGraph_GetLastEdge   (LGraph* This);
LArray*       LGraph_GetAllNodes   (LGraph* This);
LArray*       LGraph_GetAllEdges   (LGraph* This);
LArray*       LGraph_GetOutEdges   (LGraph* This, LGraph_TNode* inNode);
LArray*       LGraph_GetInEdges    (LGraph* This, LGraph_TNode* inNode);
LArray*       LGraph_GetAdjNodes   (LGraph* This, LGraph_TNode* inNode);
ui4           LGraph_GetOutDeg     (LGraph_TNode* inNode);
ui4           LGraph_GetInDeg      (LGraph_TNode* inNode);
ui4           LGraph_GetDegree     (LGraph_TNode* inNode);
ui4           LGraph_GetNodesCount (LGraph* This);
ui4           LGraph_GetEdgesCount (LGraph* This);
Bool          LGraph_IsDirected    (LGraph* This);
Bool          LGraph_IsEmpty       (LGraph* This);
ui4           LGraph_GetUsedMem    (LGraph* This);

#ifdef __LSL_DEBUG__
    void      LGraph_Dump(LGraph* This);
#endif

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
