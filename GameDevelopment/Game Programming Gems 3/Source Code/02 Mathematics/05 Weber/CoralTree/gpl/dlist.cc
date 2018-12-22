
/******************************************************************************

Coral Tree gpl/dlist.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include "gpl.h"

long GPL_Core_DoubleList::count = 0;
long GPL_DoubleNode::count = 0;
long GPL_Core_DoubleListIterator::count = 0;



/******************************************************************************
GPL_Core_DoubleList::GPL_Core_DoubleList()

keywords: c++, data, linked_list
******************************************************************************/
GPL_Core_DoubleList::GPL_Core_DoubleList()
	{
	Reset();
	}



/******************************************************************************
void	GPL_Core_DoubleList::Reset(void)

	a forced constructor
	do not use on existing valid lists

******************************************************************************/
void GPL_Core_DoubleList::Reset(void)
	{
	count++;
	HeadNode = NULL;
	TailNode = NULL;
	CurrentNode = NULL;
	nodes=0;
	end=FALSE;
	}



/******************************************************************************
GPL_Core_DoubleList::~GPL_Core_DoubleList()

keywords:
******************************************************************************/
GPL_Core_DoubleList::~GPL_Core_DoubleList()
{
	count--;
	Clear();
}



/******************************************************************************
void	GPL_Core_DoubleList::Clear()

keywords:
******************************************************************************/
void GPL_Core_DoubleList::Clear()
	{
	GPL_DoubleNode *node;

	// disassemble list
	while( (node=HeadNode) != NULL )
		{
		if( !RemoveNode(node) )
			{
			gplLog("GPL ERROR", "GPL_Core_DoubleList::Clear() couldn't remove node");
			}
			
		delete node;
		}
	}



/******************************************************************************
GPL_DoubleNode	*GPL_Core_DoubleList::ToIndex(long index)

******************************************************************************/
GPL_DoubleNode *GPL_Core_DoubleList::ToIndex(long index)
	{
	GPL_DoubleNode *node;

	ToHead();

	long m=0;
	while( m++<index && (node=PostIncrement()) != NULL );

	node=node;

	return GetCurrent();
	}



/******************************************************************************
long GPL_Core_DoubleList::Insert(GPL_DoubleNode *node,long after)

keywords:
******************************************************************************/
long GPL_Core_DoubleList::Insert(GPL_DoubleNode *node,long after)
	{
	// check to make sure node is not already on a list
	if( node->ParentList() )
		{
		gplLog("GPL ERROR", "node to add already part of a list");
		return FALSE;
		}

	GPL_DoubleNode *left=NULL,*right=NULL;
	
	if(!CurrentNode)
		{
		/* note: after the ending NULL or before the beginning NULL doesn't really make much sense
		 so, we just attach to the closest node, the head or tail*/

		if(end)
			{
			left=TailNode;
			end = FALSE;
			}
		else
			{
			right=HeadNode;
			}
		}
	else if(after)
		{
		// insert after current node

		left=CurrentNode;
		right=CurrentNode->Next();
		}
	else
		{
		// insert before current node

		right=CurrentNode;
		left=CurrentNode->Prev();
		}

	// if there is a node to the left
	if(left)
		{
		left->SetNext(node);
		node->SetPrev(left);
		}
	else
		HeadNode=node;

	// if there is a node to the right
	if(right)
		{
		right->SetPrev(node);
		node->SetNext(right);
		}
	else
		TailNode=node;

	node->SetParentList(this);
	CurrentNode = node;
	nodes++;

	return TRUE;
	}


/******************************************************************************
long GPL_Core_DoubleList::Insert(GPL_Core_DoubleList *doublelist,long after)

keywords:
******************************************************************************/
long GPL_Core_DoubleList::Insert(GPL_Core_DoubleList *doublelist,long after)
	{
	GPL_DoubleNode *left=NULL,*right=NULL;
	
	if(!CurrentNode)
		{
		/* note: after the ending NULL or before the beginning NULL doesn't really make much sense
		so, we just attach to the closest node, the head or tail */

		if(end)
			{
			left=TailNode;
			end = FALSE;
			}
		else
			{
			right=HeadNode;
			}
		}
	else if(after)
		{
		// insert after current node

		left=CurrentNode;
		right=CurrentNode->Next();
		}
	else
		{
		// insert before current node

		right=CurrentNode;
		left=CurrentNode->Prev();
		}

	// if there is a node to the left
	if(left)
		{
		left->SetNext(doublelist->HeadNode);
		doublelist->HeadNode->SetPrev(left);
		}
	else
		HeadNode=doublelist->HeadNode;

	// if there is a node to the right
	if(right)
		{
		right->SetPrev(doublelist->TailNode);
		doublelist->TailNode->SetNext(right);
		}
	else
		TailNode=doublelist->TailNode;

	/* reusing variable 'left' */
	doublelist->ToHead();
	while((left = doublelist->PostIncrement()))
	{
		left->SetParentList(this);
	}

	nodes += doublelist->nodes;
	
	doublelist->HeadNode = NULL;
	doublelist->TailNode = NULL;
	doublelist->CurrentNode = NULL;
	doublelist->nodes = 0;
	doublelist->end = FALSE;

	CurrentNode = doublelist->HeadNode;

	return TRUE;
	}



/******************************************************************************
long GPL_Core_DoubleList::RemoveNode(GPL_DoubleNode *node)

NOTE: removing a node from a list does NOT destroy the node itself

keywords:
******************************************************************************/
long GPL_Core_DoubleList::RemoveNode(GPL_DoubleNode *node)
	{
	// check to make sure the node is part of list
	if( this != node->ParentList() )
		{
		gplLog("GPL WARNING", "Attempt to remove node not in double list");
		return FALSE;
		}

	GPL_DoubleNode *left=node->Prev(),*right=node->Next();

	// if there is a node to the left
	if(left)
		left->SetNext(right);
	else
		HeadNode=right;

	// if there is a node to the right
	if(right)
		right->SetPrev(left);
	else
		TailNode=left;

	// if this was the current node, gotta choose another
	if(CurrentNode==node)
		{
		CurrentNode=right;
		if(!CurrentNode)
			end=TRUE;
		}

	node->SetPrev(NULL);
	node->SetNext(NULL);
	node->SetParentList(NULL);

	nodes--;

	return TRUE;
	}



/******************************************************************************
GPL_DoubleNode *GPL_Core_DoubleList::PostDecrement()

	move to head-more position on list, but return what was the current position

keywords:
******************************************************************************/
GPL_DoubleNode *GPL_Core_DoubleList::PostDecrement()
	{
	if(CurrentNode==NULL)
		{
		if(end)
			CurrentNode=TailNode;

		return NULL;
		}

	GPL_DoubleNode *was_current=CurrentNode;

	CurrentNode=CurrentNode->Prev();
	end=FALSE;

	return was_current;
	}


/******************************************************************************
GPL_DoubleNode *GPL_Core_DoubleList::PostIncrement(GPL_DoubleNode **current_node, long *the_end)

	move to tail-more position on list, but return what was the current position

keywords:
******************************************************************************/
GPL_DoubleNode *GPL_Core_DoubleList::PostIncrement(GPL_DoubleNode **current_node, long *the_end)
	{
	if(*current_node==NULL)
		{
		if(!*the_end)
			*current_node=HeadNode;

		return NULL;
		}

	GPL_DoubleNode *was_current=*current_node;

	*current_node=(*current_node)->Next();
	if(*current_node==NULL)
		*the_end=TRUE;

	return was_current;
	}



/******************************************************************************
	GPL_DoubleNode::GPL_DoubleNode()

keywords:
******************************************************************************/
GPL_DoubleNode::GPL_DoubleNode()
{
	count++;
	ParentListPtr = NULL;
	NextNode = NULL;
	PrevNode = NULL;
}



/******************************************************************************
GPL_DoubleNode::~GPL_DoubleNode()
******************************************************************************/
GPL_DoubleNode::~GPL_DoubleNode()
{
	count--;
	// if part of a list, remove self from list
	if(ParentListPtr)
	{
		ParentListPtr->RemoveNode(this);
	}
}

/******************************************************************************
void GPL_DoubleNode::SetNext(GPL_DoubleNode *node)
******************************************************************************/
void GPL_DoubleNode::SetNext(GPL_DoubleNode *node)
{
	NextNode = node;
}

/******************************************************************************
void GPL_DoubleNode::SetPrev(GPL_DoubleNode *node)
******************************************************************************/
void GPL_DoubleNode::SetPrev(GPL_DoubleNode *node)
{
	PrevNode = node;
}

/******************************************************************************
void GPL_DoubleNode::SetParentList(GPL_Core_DoubleList *list)
******************************************************************************/
void GPL_DoubleNode::SetParentList(GPL_Core_DoubleList *list)
{
	ParentListPtr = list;
}

/******************************************************************************
GPL_DoubleNode *GPL_DoubleNode::Next()
******************************************************************************/
GPL_DoubleNode *GPL_DoubleNode::Next()
{
	return NextNode;
}

/******************************************************************************
GPL_DoubleNode *GPL_DoubleNode::Prev()
******************************************************************************/
GPL_DoubleNode *GPL_DoubleNode::Prev()
{
	return PrevNode;
}

/******************************************************************************
GPL_Core_DoubleList *GPL_DoubleNode::ParentList()
******************************************************************************/
GPL_Core_DoubleList *GPL_DoubleNode::ParentList()
{
	return ParentListPtr;
}


/******************************************************************************
void	GPL_Core_DoubleList::ValidCheck()

******************************************************************************/
void GPL_Core_DoubleList::ValidCheck()
{
	if(HeadNode && !TailNode) 
		gplLog("GPL ERROR", "head no tail");
	if(!HeadNode && TailNode) 	
	{
		gplLog("GPL ERROR", "tail no head========================");
		((GPL_Core_DoubleList *)0)->ValidCheck();
		exit(-1);
	}
	if(CurrentNode && !HeadNode && !TailNode) 
		gplLog("GPL ERROR", "current no head no tail");
	if(!nodes && (CurrentNode || HeadNode || TailNode))
		gplLog("GPL ERROR", "handles without nodes");
	if(nodes && (!CurrentNode && !HeadNode && !TailNode))
		gplLog("GPL ERROR", "nodes without handles");
}


