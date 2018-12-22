
/******************************************************************************

Coral Tree gpl/hash2.cc
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

long GPL_CORE_HASH_CLASSNAME::count = 0;

/******************************************************************************
GPL_CORE_HASH_CLASSNAME::GPL_CORE_HASH_CLASSNAME(long a_size)
******************************************************************************/
GPL_CORE_HASH_CLASSNAME::GPL_CORE_HASH_CLASSNAME(long a_size)
{
	count++;
	Size = a_size;

	Array = (GPL_HASH_LISTTYPE<GPL_HASH_NODETYPE> **)osdCalloc((int)Size,  sizeof(GPL_HASH_LISTTYPE<GPL_HASH_NODETYPE> *));
	if(!Array)
	{
		gplLog("GPL ERROR", "out of memory");
	}

	for(int i = 0; i < Size; i++)
	{
		Array[i] = new GPL_HASH_LISTTYPE<GPL_HASH_NODETYPE>();
	}

	Count = 0;
}


/******************************************************************************
GPL_CORE_HASH_CLASSNAME::~GPL_CORE_HASH_CLASSNAME()
******************************************************************************/
GPL_CORE_HASH_CLASSNAME::~GPL_CORE_HASH_CLASSNAME()
{
	count--;
	for(int i = 0; i < Size; i++)
	{
		delete Array[i];
	}

	if(Array) 
		osdFree(Array);
}

/******************************************************************************
long GPL_CORE_HASH_CLASSNAME::GetSize()
******************************************************************************/
long GPL_CORE_HASH_CLASSNAME::GetSize()
{
	return Size;
}

/******************************************************************************
GPL_HASH_NODETYPE *GPL_CORE_HASH_CLASSNAME::LookUp(GPL_HASH_COMPTYPE *element)
******************************************************************************/
GPL_HASH_NODETYPE *GPL_CORE_HASH_CLASSNAME::LookUp(GPL_HASH_COMPTYPE *element)
{
	long index;
	GPL_HASH_NODETYPE *node;

	index = HashFunction(element);
	Array[index]->Rewind();
	while((node = Array[index]->Next()))
	{
		if(!Compare(node,element))
			break;
	}
	
	return node;
}

/******************************************************************************
GPL_HASH_NODETYPE *GPL_CORE_HASH_CLASSNAME::Remove(GPL_HASH_COMPTYPE *element)
******************************************************************************/
GPL_HASH_NODETYPE *GPL_CORE_HASH_CLASSNAME::Remove(GPL_HASH_COMPTYPE *element)
{
	long index;
	GPL_HASH_NODETYPE *node;

	index = HashFunction(element);
	node = LookUp(element);
	if(node)
	{
		Array[index]->RemoveNode(node);
		Count--;
	}
	else
	{
		gplLog("GPL ERROR", "attempt to remove non-existant node in hash table");
	}
	return node;
}

/******************************************************************************
void GPL_CORE_HASH_CLASSNAME::Insert(GPL_HASH_NODETYPE *node, GPL_HASH_COMPTYPE *element)
******************************************************************************/
void GPL_CORE_HASH_CLASSNAME::Insert(GPL_HASH_NODETYPE *node, GPL_HASH_COMPTYPE *element)
{
	long index;

	index = HashFunction(element);
	Array[index]->AppendNode(node);
	Count++;
}

/******************************************************************************
long GPL_CORE_HASH_CLASSNAME::GetCount()
******************************************************************************/
long GPL_CORE_HASH_CLASSNAME::GetCount()
{
	return Count;
}

/******************************************************************************
******************************************************************************/
GPL_HASH_LISTTYPE<GPL_HASH_NODETYPE> **GPL_CORE_HASH_CLASSNAME::GetArray(void)
{
	return Array;
}

GPL_DoubleList<void> *GPL_CORE_HASH_CLASSNAME::MemberList()
{
	long i;
	GPL_DoubleList<void> *member_list;
	void *ptr;

	if(!Count)
	{
		return NULL;
	}

	if(!Array)
	{
		gplLog("GPL ERROR", "Array not allocated");
		return NULL;
	}

	member_list = new GPL_DoubleList<void>();

	for(i=0;i<Size;i++)
	{
		if(!Array[i])
		{
			gplLog("GPL ERROR", "List not allocated");
			delete member_list;
			return NULL;
		}
		Array[i]->ToHead();
		while((ptr = Array[i]->PostIncrement()))
		{
			member_list->AppendNode(ptr);
		}
	}

	return member_list;
}

long GPL_HASH_ITERATOR::Reset(GPL_CORE_HASH_CLASSNAME *a_hashtable)
{
	hashtable = a_hashtable;
	for(index=0;index<hashtable->GetSize();index++)
	{
		hashtable->GetArray()[index]->ToHead();
	}
	index = 0;
	return TRUE;
}

GPL_HASH_NODETYPE *GPL_HASH_ITERATOR::Next()
{
	GPL_HASH_NODETYPE *return_val;

	if(!hashtable) return NULL;

	if(index >= hashtable->GetSize())
	{
		return NULL;
	}

	while(!(return_val = hashtable->GetArray()[index]->Next()))
	{
		index++;
		if(index >= hashtable->GetSize())
		{
			return NULL;
		}
	}
	return return_val;
}



