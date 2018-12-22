#include "gpl.h"

GPL_Core_DynamicArray::GPL_Core_DynamicArray(long block_size)
{
	Array = NULL;
	Size = 0;
	SizeAllocated = 0;
	SizeBlock = block_size;
}

GPL_Core_DynamicArray::~GPL_Core_DynamicArray()
{
	if(Array)
	{
		osdFree(Array);
	}
}

long GPL_Core_DynamicArray::GetSize()
{
	return Size;
}

void *GPL_Core_DynamicArray::Swap(long index, void *entry)
{
	void *return_entry;

	if(index >= Size)
	{
		gplLog("GPL ERROR", "dynamic array index out of range");
	}

	return_entry = Array[index];
	Array[index] = entry;

	return return_entry;
}

long GPL_Core_DynamicArray::Add(void *entry)
{
	long return_long;
	if(!Add(&return_long,entry))
	{
		return -1;
	}
	else
	{
		return return_long;
	}
}

void *GPL_Core_DynamicArray::Add(long *index, void *entry)
{
	*index = FirstFreeIndex();

	if(*index == Size)
	{
		if(Size == SizeAllocated)
		{
			if(Size == 0)
			{
				Array = (void **)osdCalloc((unsigned int)(SizeAllocated + SizeBlock), sizeof(void *));
				if(Array == NULL)
				{
					gplLog("GPL ERROR", "out of memory");
				}
			}
			else
			{
				Array = (void **)osdRealloc((void *)Array, (unsigned int)(SizeAllocated + SizeBlock) * sizeof(void *));
				if(Array == NULL)
				{
					gplLog("GPL ERROR", "out of memory");
				}
				osdMemset((void *)&Array[SizeAllocated], 0, (unsigned int)SizeBlock * sizeof(void *));
			}
			SizeAllocated = *index + SizeBlock;
		}
		else if(Size > SizeAllocated)
		{
			gplLog("GPL ERROR", "dynamic array size larger than allocated space");
		}
		Size = *index + 1;
	}
	else if(*index > Size)
	{
		gplLog("GPL ERROR", "invalid index");
	}
	Array[*index] = entry;
	return Array[*index];
}

void *GPL_Core_DynamicArray::Set(long index, void *entry)
{
	void *old_entry;
	if(index >= Size)
	{
		while(index >= SizeAllocated)
		{
			if(Size == 0)
			{
				Array = (void **)osdCalloc((unsigned int)(SizeAllocated + SizeBlock), sizeof(void *));
				if(Array == NULL)
				{
					gplLog("GPL ERROR", "out of memory");
				}
			}
			else
			{
				Array = (void **)osdRealloc((void *)Array, (unsigned int)(SizeAllocated + SizeBlock) * sizeof(void *));
				if(Array == NULL)
				{
					gplLog("GPL ERROR", "out of memory");
				}
				osdMemset((void *)&Array[SizeAllocated], 0, (unsigned int)SizeBlock * sizeof(void *));
			}
			SizeAllocated += SizeBlock;
		}
		Size = index + 1;
	}
	old_entry = Array[index];
	Array[index] = entry;
	return old_entry;
}

void *GPL_Core_DynamicArray::Remove(long index)
{
	void *entry;

	if(index >= Size)
	{
		gplLog("GPL ERROR", "dynamic array index out of range");
	}

	/* FIRST, REMOVE ENTRY FROM ARRAY */
	entry = Array[index];
	Array[index] = NULL;

	/* CONDENSE SIZE */
	while((Size > 0) && (Array[Size - 1] == NULL))
	{
		Size--;
	}

	/* REMOVE BLOCK(S) */
	if(Size <= 0)
	{
		Size = 0;
		if(SizeAllocated > 0) 
		{
			osdFree(Array);
			Array = NULL;
		}
		SizeAllocated = 0;
	}
	else 
	{
		while(Size <= SizeAllocated - SizeBlock)
		{
			Array = (void **)osdRealloc((void *)Array, (int)(SizeAllocated - SizeBlock) * sizeof(void *));
			if(Array == NULL)
			{
				gplLog("GPL ERROR", "memory deallocation fault");
			}
			SizeAllocated -= SizeBlock;
		}
	}
	return entry;
}

void *GPL_Core_DynamicArray::Get(long index)
{
	void *return_val;
	if(index >= Size)
	{
		return_val =  NULL;
	}
	else
	{
		return_val =  Array[index];
	}

	return return_val;
}

long GPL_Core_DynamicArray::FirstFreeIndex()
{
	long i;

	i=0;
	while(i < Size)
	{
		if(Array[i] == NULL) break;
		i++;
	}
	
	return i;
}

