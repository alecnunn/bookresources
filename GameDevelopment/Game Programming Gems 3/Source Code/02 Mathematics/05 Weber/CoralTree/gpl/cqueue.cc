
#include <gpl/gpl.h>

GPL_Core_CircularQueue::GPL_Core_CircularQueue(long block_size)
{
	Array = NULL;
	SizeAllocated = 0;
	SizeBlock = block_size;
	Size = 0;
	Head = 0;
	Tail = 0;
}

GPL_Core_CircularQueue::~GPL_Core_CircularQueue()
{
	if(Array)
	{
		delete Array;
	}
}

long GPL_Core_CircularQueue::AddTail(void *entry)
{
	void **a_array;
	long i;

	if(Size == SizeAllocated)
	{
		a_array = (void **)osdCalloc(
			(unsigned int)(SizeAllocated + SizeBlock),
			sizeof(void *));
		if(a_array == NULL)
		{
			gplLog("GPL ERROR", "out of memory");
			return FALSE;
		}

		for(i=0;i < Size;i++)
		{
			a_array[i] = Array[Head];
			GPL_INDEX_INCR(Head);
		}
		Tail = Size - 1;
		Head = 0;
		osdFree(Array);
		Array = a_array;
		SizeAllocated += SizeBlock;
	}
	else if(Size > SizeAllocated)
	{
		gplLog("GPL ERROR", "circular queue larger than space allocated");
		return FALSE;
	}

	GPL_INDEX_INCR(Tail);
	Array[Tail] = entry;
	Size++;

	return TRUE;
}

long GPL_Core_CircularQueue::AddHead(void *entry)
{
	void **a_array;
	long i;

	if(Size == SizeAllocated)
	{
		a_array = (void **)osdCalloc(
			(unsigned int)(SizeAllocated + SizeBlock),
			sizeof(void *));
		if(a_array == NULL)
		{
			gplLog("GPL ERROR", "out of memory");
			return FALSE;
		}

		for(i=1;i <= Size;i++)
		{
			a_array[i] = Array[Head];
			GPL_INDEX_INCR(Head);
		}
		Tail = Size;
		Head = 1;
		osdFree(Array);
		Array = a_array;
		SizeAllocated += SizeBlock;
	}
	GPL_INDEX_DECR(Head);
	Array[Head] = entry;
	Size++;
	return TRUE;
}

void *GPL_Core_CircularQueue::RemoveHead()
{
	void *entry;
	void **a_array;
	long i;

	/* IS ARRAY EMPTY */
	if(Size == 0)
	{
		return NULL;
	}

	entry = Array[Head];
	Array[Head] = NULL;

	/* IS THIS THE LAST ENTRY */
	if(Size == 1)
	{
		Head = 0;
		Tail = 0;
		osdFree(Array);
		Array = NULL;
		SizeAllocated = 0;
		Size = 0;
	}
	else /* NOT THE LAST ENTRY */
	{
		GPL_INDEX_INCR(Head);
		Size--;
		if(SizeAllocated == Size + SizeBlock)
		{
			
			a_array = (void **)osdCalloc(
				(unsigned int)(SizeAllocated - SizeBlock),
				sizeof(void *));
			if(a_array == NULL)
			{
				gplLog("GPL ERROR", "out of memory");
			}

			for(i=0;i < Size;i++)
			{
				a_array[i] = Array[Head];
				GPL_INDEX_INCR(Head);
			}
			Tail = Size - 1;
			Head = 0;
			osdFree(Array);
			Array = a_array;
			SizeAllocated -= SizeBlock;
		}
		else if(SizeAllocated > Size + SizeBlock)
		{
			gplLog("GPL ERROR", "this much extra space should never be allocated");
		}
	}
	
	return entry;
}

void *GPL_Core_CircularQueue::RemoveTail()
{
	void *entry;
	void **a_array;
	long i;

	/* IS ARRAY EMPTY */
	if(Size == 0)
	{
		return NULL;
	}

	entry = Array[Tail];
	Array[Tail] = NULL;

	/* IS THIS THE LAST ENTRY */
	if(Size == 1)
	{
		Head = 0;
		Tail = 0;
		osdFree(Array);
		Array = NULL;
		SizeAllocated = 0;
		Size = 0;
	}
	else /* NOT THE LAST ENTRY */
	{
		GPL_INDEX_DECR(Tail);
		Size--;
		if(SizeAllocated == Size + SizeBlock)
		{
			
			a_array = (void **)osdCalloc(
				(unsigned int)(SizeAllocated - SizeBlock),
				sizeof(void *));
			if(a_array == NULL)
			{
				gplLog("GPL ERROR", "out of memory");
			}

			for(i=0;i < Size;i++)
			{
				a_array[i] = Array[Head];
				GPL_INDEX_INCR(Head);
			}
			Tail = Size - 1;
			Head = 0;
			osdFree(Array);
			Array = a_array;
			SizeAllocated -= SizeBlock;
		}
		else if(SizeAllocated > Size + SizeBlock)
		{
			gplLog("GPL ERROR", "this much extra space should never be allocated");
		}
	}
	
	return entry;
}


