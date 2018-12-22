
/******************************************************************************

Coral Tree gpl/hash2.h
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
keywords: template, c++, blackbox_system, data

HashNode must be a class derived from GPL_DoubleNode if an intrusive hashtable
is used.
The virtual methods HashFunction and Compare must be defined.

note the #include "hash.h" trick below where this file includes itself once
so the hash code is reused for both the intrusive and non-intrusive hash
tables whose only real difference is the type of linked lists used.

******************************************************************************/
class GPL_CORE_HASH_CLASSNAME
{
	public:
		static long 		count;
	private:

		long Count;
		long Size;
		GPL_HASH_LISTTYPE<GPL_HASH_NODETYPE> **Array;

	public:

							GPL_CORE_HASH_CLASSNAME(long a_size);
virtual						~GPL_CORE_HASH_CLASSNAME(void);

		long				GetSize(void);
		GPL_HASH_NODETYPE	*LookUp(GPL_HASH_COMPTYPE *element);
		GPL_HASH_NODETYPE	*Remove(GPL_HASH_COMPTYPE *element);
		void				Insert(GPL_HASH_NODETYPE *node, GPL_HASH_COMPTYPE *element);
		virtual long		HashFunction(GPL_HASH_COMPTYPE *element) 
							{
								element = element;
								return 0;
							}
		virtual long		Compare(GPL_HASH_NODETYPE *node, GPL_HASH_COMPTYPE *element) 
							{
								node = node;
								element = element;
								return 0;
							}
		long				GetCount(void);
		GPL_HASH_LISTTYPE<GPL_HASH_NODETYPE> **GetArray(void);
		GPL_DoubleList<void> *MemberList(void);
};

template<class HashNode, class CompType>
class GPL_HASH_CLASSNAME : public GPL_CORE_HASH_CLASSNAME
{
	public:
		GPL_HASH_CLASSNAME(long a_size) : GPL_CORE_HASH_CLASSNAME(a_size){};
virtual ~GPL_HASH_CLASSNAME(){}

		long				GetSize(void)
								{
								return GPL_CORE_HASH_CLASSNAME::GetSize();
								}
		long				GetCount(void)
								{
								return GPL_CORE_HASH_CLASSNAME::GetCount();
								};
		GPL_HASH_LISTTYPE<GPL_HASH_NODETYPE> **GetArray(void)
								{
								return GPL_CORE_HASH_CLASSNAME::GetArray();
								}
		HashNode			*LookUp(CompType *element)
								{
								return (HashNode *)GPL_CORE_HASH_CLASSNAME::LookUp((GPL_HASH_COMPTYPE *)element);
								};
		HashNode			*Remove(CompType *element)
								{
								return (HashNode *)GPL_CORE_HASH_CLASSNAME::Remove((GPL_HASH_COMPTYPE *)element);
								};
		void				Insert(HashNode *node, CompType *element)
								{
								GPL_CORE_HASH_CLASSNAME::Insert((GPL_HASH_NODETYPE *)node,(GPL_HASH_COMPTYPE *)element);
								};
		GPL_DoubleList<void> *MemberList(void)
								{
								return GPL_CORE_HASH_CLASSNAME::MemberList();
								}
};

/******************************************************************************
WARNING: This is a convenience object to be used as an alternative to
eemberList().  It's state is not valid after other operations on the hash 
table.  A Reset() should be done to make it valid again.
******************************************************************************/

class GPL_HASH_ITERATOR
{
	private:
		long index;
		GPL_CORE_HASH_CLASSNAME *hashtable;
	public:
		GPL_HASH_ITERATOR(){hashtable = NULL;}
virtual ~GPL_HASH_ITERATOR(){}
		long Reset(GPL_CORE_HASH_CLASSNAME *a_hashtable);
		GPL_HASH_NODETYPE *Next();
};




