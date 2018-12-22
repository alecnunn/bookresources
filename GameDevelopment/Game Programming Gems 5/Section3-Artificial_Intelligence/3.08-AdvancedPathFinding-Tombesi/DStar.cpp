/**********************************************************************

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2003
 **********************************************************************/
#include "DStar.h"

extern "C"
{
	#include "LType.h"
	#include "LGraph.h"
	#include "LNodeInfo.h"
	#include "LEdgeInfo.h"
	#include "LGraphGen.h"
	#include "LHash.h"
	#include "LEdgeMap.h"
	#include "LMemory.h"
}

#include <stdlib.h>
#include <stdio.h>

#define NO_VAL   LType_MAX_UI4
#define NO_PATH  20

/* Utility Macros */
#define MIN(a, b) ((a<=b)? a : b)

/* Typecast Macros */
#define NODE *(LGraph_TNode**)
#define UI4  *(ui4*)

DStar::DStar(unsigned long n, point start, point goal)
{
	base			 = n;
	Map				 = LGraphGen_RandomGrid(base, FALSE);
	NodesList		 = LGraph_GetAllNodes(Map);
    Cost			 = LGraphGen_RndEdgeInfoUI4(Map, 0, 0);
	Tag				 = LNodeInfo_New(Map, LType_UI1);
	BackPtr			 = LNodeInfo_New(Map, LType_Ptr);
	H				 = LNodeInfo_New(Map, LType_UI4);
	NumberIterations = LNodeInfo_New(Map, LType_UI4);
	K		         = (Slot*)malloc(base*base*sizeof(Slot));
	KIndices         = LHash_New();
	EdgeMap          = LEdgeMap_New(Map);
	ProcessedNodes   = (point*)malloc(base*base*sizeof(point));
	this->start      = NODE LArray_ItemAt(NodesList, start.x*n+start.y);
	current          = this->start;
	this->goal       = NODE LArray_ItemAt(NodesList, goal.x*n+goal.y);
	PathCost         = 0;
	finished         = false;
	total_processed_nodes      = 0;
	nodes_processed_since_last = 0;
	OpenListCount              = 0;

	LGraph_TNode* node;

	LGraph_ForAllNodes(Map, node)
	{
		LNodeInfo_UI1At(Tag, node) = NEW;
		LNodeInfo_UI4At(H, node) = 0;
		NODE LNodeInfo_ItemAt(BackPtr, node) = NULL;
	}
}

DStar::~DStar(void)
{
	LEdgeMap_Delete(&EdgeMap);
	LGraph_Delete(&Map);
	LArray_Delete(&NodesList);
	LEdgeInfo_Delete(&Cost);
	LNodeInfo_Delete(&Tag);
	LNodeInfo_Delete(&BackPtr);
	LNodeInfo_Delete(&H);
	LNodeInfo_Delete(&NumberIterations);
	free(K);
	free(ProcessedNodes);
	LHash_Delete(&KIndices);
}

void DStar::SetEdge(point p0, point p1, unsigned long value)
{
	LGraph_TEdge* edge;
	LGraph_TNode* x;
	LGraph_TNode* y;
	ui4 val;
	Bool modified = FALSE;

	if (current != goal)
	{

		x = NODE LArray_ItemAt(NodesList, p0.x*base+p0.y);
		y = NODE LArray_ItemAt(NodesList, p1.x*base+p1.y);
		edge = LEdgeMap_GetEdge(EdgeMap, x, y);
		if (LEdgeInfo_UI4At(Cost, edge) != (ui4)value)
		{
			modified = TRUE;
			if (current != start)
				{
					ui4 h_x = LNodeInfo_UI4At(H, x);
					ui4 h_y = LNodeInfo_UI4At(H, y);

					if (h_x < h_y)
						val = ModifyCost(x, y, (ui4)value);
					else
						val = ModifyCost(y, x, (ui4)value);
				}
				else
					if (edge) LEdgeInfo_UI4At(Cost, edge) = (ui4)value;
		}
		if ( (current != start) && (modified) )
		{
			while ( (val < LNodeInfo_UI4At(H, current)) && (val != NO_VAL) )
						val = ProcessState();
		}
	}
}

node_info DStar::GetNodeInfo(point p)
{
	LGraph_TNode* x;
	LGraph_TNode* b;
	node_info info;

	info.pos.x = p.x;
	info.pos.y = p.y;
	x = NODE LArray_ItemAt(NodesList, p.x*base+p.y);
	info.iterations = LNodeInfo_UI4At(NumberIterations, x);
	info.h = LNodeInfo_UI4At(H, x);
	info.tag = LNodeInfo_UI1At(Tag, x);
	if ( (info.tag) == OPEN)
	{
		ui4 idx = LHash_GetItemByKey(KIndices, (ui4)x);
		if ( K[idx].value < info.h)
			info.status = IS_RAISE;
		else
			if ( K[idx].value == info.h)
				info.status = IS_LOWER;
	}
	b = NODE LNodeInfo_ItemAt(BackPtr, x);
	if (b)
	{
		info.bptr.x = b->mIndex / base;
		info.bptr.y = b->mIndex % base;
	}
	return info;
}

point* DStar::Play(long n, long& len)
{
	ui4 idx;
	point* path;
	ui4 val = 0;
	
	if (finished) 
	{
		len = 0;
		return NULL;
	}
	/* first path finding */
	if (current == start)
	{
		LGraph_TNode* node;
		K_Init();
		LGraph_ForAllNodes(Map, node)
		{	
			LNodeInfo_UI1At(Tag, node) = NEW;
			LNodeInfo_UI4At(H, node) = 0;
			LNodeInfo_UI4At(NumberIterations, node) = 0;
			NODE LNodeInfo_ItemAt(BackPtr, node) = NULL;
		}
		idx = K_Insert(0, goal);
		OpenListCount++;
		LHash_InsertItem(KIndices, idx, (ui4)goal);
		while ((LNodeInfo_UI1At(Tag, current) != CLOSED) && (val != NO_VAL))
			val = ProcessState();
		if (LNodeInfo_UI1At(Tag, current) == NEW)
		{// no path has been found
			len = 0;
			finished = true;
			return NULL;
		}	
	}

	if ( n < 0)
		n = base*base;
	path = (point*)malloc(n*sizeof(point));
	for (long i=0; i < n; i++)
	{
		path[i].x = (current->mIndex) / base;
		path[i].y = (current->mIndex) % base;
		LGraph_TNode* b_current = NODE LNodeInfo_ItemAt(BackPtr, current); 
		if (b_current)
		{
			LGraph_TEdge* edge = LEdgeMap_GetEdge(EdgeMap, current, b_current);
			PathCost += LEdgeInfo_UI4At(Cost, edge);
		}
		if (current == goal)
		{
			finished = true;
			break;
		}
		current = b_current;
	}
	len = i + 1;
	return path;
}

point DStar::CurrentLocation()
{
	point p;

	p.x = current->mIndex / base;
	p.y = current->mIndex % base;
	return p;
}

unsigned long DStar::GetEdgeCost(point p0, point p1)
{
	LGraph_TNode* x;
	LGraph_TNode* y;
	LGraph_TEdge* edge;

	x = NODE LArray_ItemAt(NodesList, p0.x*base+p0.y);
	y = NODE LArray_ItemAt(NodesList, p1.x*base+p1.y);
	edge = LEdgeMap_GetEdge(EdgeMap, x, y);
	if (edge) 
		return LEdgeInfo_UI4At(Cost, edge);
	else
		return 0;
}

void DStar::K_Init()
{
	ui4     i;
	ui4 count = base*base;

	for (i=0; i < count; i++)
	{
		K[i].value = LType_MAX_UI4;
		K[i].state = NULL;
	}
}

ui4 DStar::K_GetMin()
{
	ui4     i;
	ui4   min = LType_MAX_UI4;
	ui4 count = base*base;

	for (i=0; i < count; i++)
		if ( K[i].value < min)
		{
			min = K[i].value;
			if (K[i].state) minState = K[i].state;
		}
	return min;
}

LGraph_TNode* DStar::K_GetState()
{

	if (!OpenListCount) 
		return NULL;
	else
		return minState;
}

ui4 DStar::K_Insert(ui4 value, LGraph_TNode* state)
{
	ui4 i;
	ui4 count = base*base;

	for (i=0; i < count; i++)
		if (!K[i].state)
		{
			K[i].state = state;
			K[i].value = value;
			return i;
		}
	return count;
}

ui4 DStar::ModifyCost(LGraph_TNode* x, LGraph_TNode* y, ui4 value)
{
	ui4            h_x = LNodeInfo_UI4At(H, x);
    LGraph_TEdge* edge = LEdgeMap_GetEdge(EdgeMap, x, y);


	LEdgeInfo_UI4At(Cost, edge) = value;
	if (LNodeInfo_UI1At(Tag, x) == CLOSED)
		Insert(x, h_x);
	nodes_processed_since_last = 0;
	return GetKMin();
}

LGraph_TNode* DStar::MinState()
{
	LGraph_TNode* state;

	if (!OpenListCount)
		return NULL;
	else
		state = K_GetState();
	return state;
}

void DStar::Insert(LGraph_TNode* x, ui4 h_new)
{
	ui1	t_x;
	ui4	k_x;
	ui4	h_x;	
	ui4 idx;

    t_x = LNodeInfo_UI1At(Tag, x);
	h_x = LNodeInfo_UI4At(H,x);

    if (t_x == NEW) 
	{
		idx = K_Insert(h_new, x);
		OpenListCount++;
		LHash_InsertItem(KIndices, (ui4)idx, (ui4)x);
	}
	else
	{
		if (t_x == OPEN)
		{
			idx = LHash_GetItemByKey(KIndices, (ui4)x);
			if (x != K[idx].state) printf("********ERROR: %p != %p at index %d\n", x, K[idx].state, idx);///////////////////////////////////////////DEBUG
			k_x = K[idx].value;
			K[idx].value = MIN(k_x, h_new);
		}
		else
		{
			
			h_x = LNodeInfo_UI4At(H,x);
			idx = K_Insert(MIN(h_x, h_new), x);
			OpenListCount++;
			LHash_InsertItem(KIndices, (ui4)idx, (ui4)x);
		}
	}
	LNodeInfo_UI4At(H,x) = h_new;
	LNodeInfo_UI1At(Tag, x) = OPEN;
	printf("Inserting node [%p] in the open list,  Tag[%p] = OPEN\n", x, x);///////////////////////////////////////////DEBUG
}


void DStar::Delete(LGraph_TNode* x)
{
	ui4 idx;

	/* we must update K(x) */
	idx = LHash_GetItemByKey(KIndices, (ui4)x);
	if (x != K[idx].state) printf("********ERROR: %p != %p at index %d\n", x, K[idx].state, idx);///////////////////////////////////////////DEBUG
	LHash_RemoveItem(KIndices, (ui4)x);
	OpenListCount--;
	K[idx].state = NULL;
	K[idx].value = LType_MAX_UI4;
	
	LNodeInfo_UI1At(Tag, x) = CLOSED;
	printf("Deleting node [%p] from the open list, Tag[%p] = CLOSED\n", x, x);///////////////////////////////////////////DEBUG
}

ui4 DStar::GetKMin()
{
	ui4 k_min;

	if (!OpenListCount) return NO_VAL;
	else
		k_min = K_GetMin();
	return k_min;
}

ui4 DStar::ProcessState()
{
	ui4				k_old;
	ui4				h_y;
	ui4				h_x;
	ui4				c_xy;
	ui1				t_y;
	LGraph_TNode*	x;
	LGraph_TNode*	y;
	LGraph_TNode*	b_y;
	LGraph_TEdge*	edge;
	
	k_old = K_GetMin();
	x = K_GetState();
	if (x) LNodeInfo_UI4At(NumberIterations, x)++;
	printf("Examining node [%p]\n", x);///////////////////////////////////////////DEBUG
	if (x == NULL) return NO_VAL;
	Delete(x);
	if (k_old < LNodeInfo_UI4At(H, x))
	{
		LGraph_ForAllOut(x, edge)
		{
			c_xy = LEdgeInfo_UI4At(Cost, edge);
			y = (LGraph_GetSource(edge) == x) ? LGraph_GetTarget(edge):
												LGraph_GetSource(edge);
			h_y = LNodeInfo_UI4At(H,y);
			h_x = LNodeInfo_UI4At(H,x);
			if ((h_y <= k_old) && (h_x > h_y + c_xy))
			{
				NODE LNodeInfo_ItemAt(BackPtr, x) = y;
				printf("B(%p) = %p \n", x, y);///////////////////////////////////////////DEBUG
				LNodeInfo_UI4At(H, x) = h_y + c_xy;
			}
		}
	}
	if (k_old == LNodeInfo_UI4At(H, x))
	{
		LGraph_ForAllOut(x, edge)
		{
			c_xy = LEdgeInfo_UI4At(Cost, edge);
			y = (LGraph_GetSource(edge) == x) ? LGraph_GetTarget(edge):
												LGraph_GetSource(edge);
			t_y = LNodeInfo_UI1At(Tag, y);
			b_y = NODE LNodeInfo_ItemAt(BackPtr, y);
			h_y = LNodeInfo_UI4At(H,y);
			h_x = LNodeInfo_UI4At(H,x);
			if ((t_y == NEW)						 || 
			    ( (b_y == x) && (h_y != h_x + c_xy) )||
			    ( (b_y != x) && (h_y  > h_x + c_xy) ))
			{
				NODE LNodeInfo_ItemAt(BackPtr, y) = x;
				printf("B(%p) = %p \n", y, x);///////////////////////////////////////////DEBUG
				Insert(y, h_x + c_xy);
			}
		}
	}
	else
	{
		LGraph_ForAllOut(x, edge)
		{
			c_xy = LEdgeInfo_UI4At(Cost, edge);
			y = (LGraph_GetSource(edge) == x) ? LGraph_GetTarget(edge):
													LGraph_GetSource(edge);
			t_y = LNodeInfo_UI1At(Tag, y);
			b_y = NODE LNodeInfo_ItemAt(BackPtr, y);
			h_y = LNodeInfo_UI4At(H,y);
			h_x = LNodeInfo_UI4At(H,x);
			if ((t_y == NEW)						  ||
				( (b_y == x) && (h_y != h_x + c_xy) )) 
			{
				NODE LNodeInfo_ItemAt(BackPtr, y) = x;
				printf("B(%p) = %p \n", y, x);///////////////////////////////////////////DEBUG
				Insert(y, h_x + c_xy);
			}
			else
				if ( (b_y != x) && (h_y > h_x + c_xy) )
					Insert(x, h_x);
				else
					if ( (b_y != x) && (h_x > h_y + c_xy) &&
						 (t_y == CLOSED) && (h_y > k_old) )
						Insert(y, h_y);
			
		}
	}
	if (current != start)
	{
		ProcessedNodes[nodes_processed_since_last].x = x->mIndex / base;
		ProcessedNodes[nodes_processed_since_last].y = x->mIndex % base;
		nodes_processed_since_last++;
	}
	
	total_processed_nodes++;
	return GetKMin();
}

point DStar::GetStart()
{
	point p;

	p.x = start->mIndex / base;
	p.y = start->mIndex % base;
	return p;
}

point DStar::GetGoal()
{
	point p;

	p.x = goal->mIndex / base;
	p.y = goal->mIndex % base;
	return p;
}

