
/******************************************************************************

Coral Tree gpl/darray.h
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

/******************************************************************************
keywords: c++, data, template, blackbox_system

******************************************************************************/
class GPL_Core_DynamicArray
{
#ifdef GPL_PROBE_DYNAMIC_ARRAY
	public:
#else
	private:
#endif

		void	**Array;
		long	Size;
		long 	SizeAllocated;
		long	SizeBlock;

	public:

				GPL_Core_DynamicArray(long block_size);
virtual			~GPL_Core_DynamicArray();

		long	GetSize();
		void	*Swap(long index, void *entry);
		void	*Set(long index, void *entry);
		void	*Add(long *index, void *entry);
		long	Add(void *entry);
		void	*Remove(long index);
		void	*Get(long index);
		long	FirstFreeIndex();
};

template<class Entry>
class GPL_DynamicArray : public GPL_Core_DynamicArray
{
	public:

				GPL_DynamicArray(long block_size) : GPL_Core_DynamicArray(block_size) {}

		long	GetSize() { return GPL_Core_DynamicArray::GetSize(); }
		Entry	*Swap(long index, Entry *entry){ return (Entry *)GPL_Core_DynamicArray::Swap(index,(void *)entry); }
		Entry	*Set(long index, Entry *entry){ return (Entry *)GPL_Core_DynamicArray::Set(index,(void *)entry); }
		Entry	*Add(long *index, Entry *entry){ return (Entry *)GPL_Core_DynamicArray::Add(index,(void *)entry); }
		long	Add(Entry *entry){ return GPL_Core_DynamicArray::Add((void *)entry); }
		Entry	*Remove(long index){return (Entry *)GPL_Core_DynamicArray::Remove(index);}
		Entry	*Get(long index){return (Entry *)GPL_Core_DynamicArray::Get(index);}
};


