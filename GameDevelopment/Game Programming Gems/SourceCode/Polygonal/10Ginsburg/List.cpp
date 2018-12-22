/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
//////////////////////////////////////////////////////////////////////////////////////////
//
//	List.cpp
//	
//	Author: Dan Ginsburg
//	
//	Description:
//
//		General list class for arbitrary data
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "List.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Constructor/Destructor
//	
//	

///
//	List()
//
List::List()
{
	First= NULL;
	Last=  NULL;
	Iter=  NULL;
	ListCount= 0;
}

///
//	~List()
//
List::~List()
{
	LIST_NODE *Node= First;

	while(Node != NULL)
	{
		LIST_NODE *Del= Node;

		Node= Node->Next;
		free(Del);
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Protected Methods
//	
//	


//////////////////////////////////////////////////////////////////////////////////////////
//
//	Public Methods
//	
//	

///
//	Append()
//
int List::Append(void *Data, int Size)
{
	LIST_NODE *NewNode;

	NewNode= (LIST_NODE*)malloc(sizeof(LIST_NODE));
	if(NewNode== NULL)
		return(FALSE);

	NewNode->Data= malloc(Size);
	if(NewNode->Data== NULL)
		return(FALSE);

	memcpy(NewNode->Data, Data, Size);
	NewNode->Next= NULL;
	NewNode->Prev= NULL;
	NewNode->Id=   ListCount;
    NewNode->DataSize= Size;
	
	if(Last== NULL)
	{
		First= NewNode;
		Last=  First;
	}
	else
	{
		Last->Next=    NewNode;
		NewNode->Prev= Last;
		Last=		   NewNode;
	}

	
	ListCount++;
	return(TRUE);
}

///
//	GetCount()
//
int List::GetCount()
{
	return(ListCount);
}

///
//	GetLast()
//
void* List::GetLast()
{
	if(Last)
		return(Last->Data);

	return(NULL);
}

///
//	BuildTable()
//
//		Build a table of pointers to the node data
//
void** List::BuildTable(int *Count)
{
	LIST_NODE *Node= First;
	void	  **Table;
	
	*Count= ListCount;
	if(Node != NULL)
	{
		int i= 0;

		Table= (void**)malloc(sizeof(void*) * ListCount);
		if(Table== NULL)
			return(NULL);

		while(Node != NULL)
		{
			Table[i]= Node->Data;
			Node= Node->Next;
			i++;
		}

		return(Table);
	}
	
	return(NULL);
}

///
//  BuildLinearTable()
//
//      Create one big array with all the node data
//
void* List::BuildLinearTable(int *Count)
{
	LIST_NODE *Node= First;
	char	  *Table;
	
	*Count= ListCount;
	if(Node != NULL)
	{
		int i= 0;

        ///
        //  Assuming all data is the same size
        //
		Table= (char*)malloc(Node->DataSize * ListCount);
		if(Table== NULL)
			return(NULL);

		while(Node != NULL)
		{
			memcpy(&Table[i * Node->DataSize], Node->Data, Node->DataSize);
			Node= Node->Next;
			i++;
		}

		return(Table);
	}
	
	return(NULL);
}


///
//	PositionAt()
//
void List::PositionAt(int Id)
{
	LIST_NODE *Node= Last;
	
	while(Node != NULL && Node->Id != Id)
		Node= Node->Prev;
	
	Iter= Node;
}

///
//	GetCurrent()
//
void* List::GetCurrent()
{
	if(Iter)
		return(Iter->Data);
	
	return(NULL);
}

///
//	Advance()
//
void List::Advance()
{
	Iter= Iter->Next;
}
