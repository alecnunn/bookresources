
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

#ifndef __GPGList_h__
#define __GPGList_h__


class GPGVariant;


#if GPGLIST_USEALLOCATOR==1
class GPGUnitAllocator;
#endif


/******************************************************************************
class GPGCoreList

	contains type-nonspecific functionality of GPGList in order to reduce
		template size

	allows access to generic list without knowing the element type
	(parser uses this)

	WARNING:	this class must contain all data used by GPGList;
				it must be interchangable with a GPGList through a void*

******************************************************************************/
class GPGCoreList
	{
	public:
					GPGCoreList(void);
virtual				~GPGCoreList(void);

#ifdef GPG_LISTALLOC
virtual	void		AllocElement(GPGVariant *get)							=0;
virtual	void		AccessElement(long index,GPGVariant *get)				=0;
#endif

					//* remove all elements from list (does not delete them)
		void		Clear(void);

					// for GPGListNode only
#if GPGLIST_USEALLOCATOR==3
		void		Deallocate(void *ptr);
#else
static	void		Deallocate(void *ptr);
#endif

		void		SetAutoDestruct(BOOL set)		{ m_autodestruct=set; }
		BOOL		GetAutoDestruct(void)			{ return m_autodestruct; }

		long		GetNumberElements(void) const	{ return m_length; }

	protected:
		BOOL		CoreRemove(void *entry);
		BOOL		CoreRemove(void *entry, GPGListContext &context);
		void		*CoreSearchForElement(GPGListContext &context,
														void *entry) const;
		void		*CoreGetElement(long index) const;
		void		**CoreInsert(BOOL before,GPGListContext &context,
										void *entry,GPGListNode *existingNode);
		BOOL		CoreRemoveNode(GPGListNode *node);
		BOOL		CoreMoveNode(BOOL before,GPGListContext &from,
														GPGListContext &to);

		GPGListNode			*m_head;
		GPGListNode			*m_tail;
		long				m_length;
		BOOL				m_autodestruct;

#if GPGLIST_USEALLOCATOR==1
static	GPGUnitAllocator	*m_pAllocator;
#elif GPGLIST_USEALLOCATOR==2
static	GPGChunkPool		*m_pChunkPool;
#elif GPGLIST_USEALLOCATOR==3
		GPGUnitAllocator	m_allocator;
#endif

static	U32					m_listCount;

	private:
		void		*InternalToHead(GPGListContext &context) const;
		void		*InternalGetCurrent(GPGListContext &context) const;
		void		*InternalPostIncrement(GPGListContext &context) const;
		void		InternalDetachNode(GPGListNode *node);
	};


/**************************************************************************//**
template <class T> class GPGList: public GPGCoreList

	bidirectional linked list

	there is a NULL entry at both beginning and end

	To allow generic allocation, a list must define it's own AllocElement()
	like the following.  The default AllocElement() will assert.

		virtual	void	AllocElement(GPGVariant *get)
			{
			T *entry=new T();

			Append(entry);
			*get=entry;
			};

	If SearchForContent() is to be used and type T has any data complexity
	the operator == should probably be defined for type T.  Some examples of
	data complexity are:
		- pointer data members
		- data members that do not contribute to 'equality'
		- data whose equality can be more efficiently determined with something
		  other than the default bytewise comparison.
		- fuzzy or soft data

	The element stored is actually T*, not T, so templating
	on a pointer is probably adding an additional level of indirection.
	Note that this is not true for GPGArray which stores actual instances.

	Example:

		GPGList<MyClass> list;
		GPGListContext context;
		MyClass *one=new MyClass();
	
		// add element
		list.Append(one);

		// read element
		MyClass *two=list.GetCurrent(context);

		// do something to all elements
		MyClass *node;
		list.ToHead(context);
		while( (node=list.PostIncrement(context)) != NULL)
			{
			node->DoSomething();
			}

	No contexts should lose place if elements are removed,
	even if removed with a different context.
	Reference counting prevents invalid pointers and heirs
	attempt to find valid node.

	By using SetAutoDestruct(true), all elements remaining
	on the list are deleted when the list is destructed.
	WARNING: if these elements were not created with new(),
	failures will probably occur.  Note that SetAutoDestruct(true)
	is precarious when elements may be in more than one list.
	In this case it is very possible for elements to be deleted
	that are still members of other lists.

	Note that much of the functionality of this class is implemented in the 
	non-template class GPGCoreList.  This is done to reduce template
	instantiation size.

	Uses GPGUnitAllocator to manage GPGListNode's.

*//***************************************************************************/
template <class T> class GPGList: public GPGCoreList
	{
	public:
				GPGList(void)	{ }
virtual			~GPGList(void)
				{
					if(m_autodestruct)
						DeleteAll();
					else
						RemoveAll();
				}

#ifdef GPG_LISTALLOC
					// for GPGCoreList
virtual	void	AllocElement(GPGVariant *get)
				{
#if TRUE
					/*** a list permitting this function must define their
						own type-specific default version */
					GPGASSERT(FALSE);
					*get=GPGVariant(0);
#else
					T *entry=new T();

					Append(entry);
					*get=entry;
#endif
				}
virtual	void	AccessElement(long index,GPGVariant *get)
				{	*get=GetElement(index); }
#endif

				/** The four insertion functions returns the location of
					the new element.  They can return NULL if the internal
					node allocation failed. */
		T		**InsertBefore(GPGListContext &context,T *entry)
				{	return (T **)CoreInsert(true,context,(void *)entry,NULL); }
		T		**InsertAfter(GPGListContext &context,T *entry)
				{	return (T **)CoreInsert(false,context,(void *)entry,NULL); }
		T		**Prepend(T *entry)
				{
					GPGListContext context;
					context.SetCurrent(m_head);
#if GPGLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return (T **)CoreInsert(false,context,(void *)entry,NULL);
				}
		T		**Append(T *entry)
				{
					GPGListContext context;
					context.SetCurrent(m_tail);
#if GPGLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return (T **)CoreInsert(false,context,(void *)entry,NULL);
				}

				/// Append a copy of an entire list of identical type
		void	Append(GPGList<T> &list)
				{
					T *pT;
					GPGListContext context;
					list.ToHead(context);
					while((pT=list.PostIncrement(context))!=NULL)
						Append(pT);
				}

				/**	This is the straightforward remove implementation.  This
					Remove() will look for the entry in the list by simply
					checking each member entry in order.  This can be very slow
					in some cases.  See Remove(T *entry, GPGListContext &hint)
					for a faster alternative */
		BOOL	Remove(T *entry)
				{	return CoreRemove((void *)entry); }

				/**	This Remove() method uses a hint GPGListContext to check
					current, next, and prev nodes (in that order) before
					reverting to a head to tail scan.  In most cases this 
					significantly reduces execution time. */
		BOOL	Remove(T *entry, GPGListContext &hint)
				{	return CoreRemove((void *)entry, hint); }

				/// Remove and delete nodes, but don't delete contents
		void	RemoveAll(void)
				{
					BOOL result;
					while( m_head != NULL)
					{
						result=CoreRemoveNode(m_head);
						GPGASSERT(result);
					}
				}

		void	DeleteAll(void)
				{
					BOOL result;
					while( m_head != NULL)
					{
//						result=Delete((T *)node->GetPointer());

						T *ptr=(T *)m_head->GetPointer();
						result=CoreRemoveNode(m_head);
						GPGASSERT(result);
						delete ptr;
					}
				}

		BOOL	DeleteCurrent(GPGListContext &context)
				{
					GPGListNode *node=GetCurrent();
					if(node)
					{
						T *ptr=(T *)node->GetPointer();
						result=CoreRemoveNode(node);
						GPGASSERT(result);
						delete ptr;
						return result;
					}

					return false;
				};

		BOOL	Delete(T *entry)
				{
					BOOL result=Remove(entry);
					delete entry;

					return result;
				};

		BOOL	MoveNodeBefore(GPGListContext &from,GPGListContext &to)
				{	return CoreMoveNode(true,from,to); }
		BOOL	MoveNodeAfter(GPGListContext &from,GPGListContext &to)
				{	return CoreMoveNode(false,from,to); }

		BOOL	IsAtHead(GPGListContext &context) const
				{	return !context.GetCurrent() && !context.GetAtTail(); };
		BOOL	IsAtTail(GPGListContext &context) const
				{	return !context.GetCurrent() && context.GetAtTail(); };

		T		*ToHead(GPGListContext &context) const
				{
					context.SetCurrent(m_head);
#if GPGLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return GetCurrent(context);
				}

		T		*ToTail(GPGListContext &context) const
				{
					context.SetCurrent(m_tail);
#if GPGLIST_CHECKPOINTER
					context.SetCorePointer(this);
#endif
					return GetCurrent(context);
				}

				/** return the first element matching the given pointer.
					Only pointer are compared.
					Matches are not made by content they point to. */
		T		*SearchForElement(GPGListContext &context,T *entry) const
				{
					return (T *)
						(CoreSearchForElement(context,(void *)entry));
				}

				/** return the first element with the same value.
					The elements are compared to the contents of *value.
					The value argument is passed by pointer instead of
					value or reference to prevent a requirement the
					T be an instantiable class. */
		T		*SearchForContent(GPGListContext &context,const T *value) const
				{
					if(!value)
						return NULL;

					ToHead(context);

					T *entry2;
					while( (entry2=GetCurrent(context)) != NULL)
					{
						if(*entry2==*value)
							break;
						else
							PostIncrement(context);
					}

					if(!entry2)
						return NULL;

					return GetCurrent(context);
				}

		T		*PostIncrement(GPGListContext &context) const
				{
/*
					GPGListNode *node=context.GetCurrent();

					if(node)
						context.SetCurrent(node->GetNext());
					else if(!IsAtEnd(context))
						ToHead(context);

					if(m_length && !context.GetCurrent())
						context.SetAtEnd(true);

					return node? (T *)(node->GetPointer()): (T *)NULL;
*/
					GPGListNode *node,*next;

					if((node=context.GetCurrent())!=NULL)
					{
						context.SetCurrent(next=node->GetNext());
						if(!next)
							context.SetAtTail(true);
						return (T *)(node->GetPointer());
					}
					else if(!IsAtTail(context))
						context.SetCurrent(m_head);

					return (T *)NULL;
				}

		T		*PostDecrement(GPGListContext &context) const
				{
/*
					GPGListNode *node=context.GetCurrent();
					if(node)
						context.SetCurrent(node->GetPrevious());
					else if(IsAtEnd(context))
						ToTail(context);
					context.SetAtEnd(false);
					return node? (T *)(node->GetPointer()): (T *)NULL;
*/
					GPGListNode *node;

					if(node=context.GetCurrent())
					{
						context.SetCurrent(node->GetPrevious());
						context.SetAtTail(false);
						return (T *)(node->GetPointer());
					}
					else if(IsAtTail(context))
						context.SetCurrent(m_tail);

					context.SetAtTail(false);
					return (T *)NULL;
				}

		T		*GetHead(void) const
				{	return m_head? (T *)(m_head->GetPointer()): (T *)NULL; }

		T		*GetTail(void) const
				{	return m_tail? (T *)(m_tail->GetPointer()): (T *)NULL; }

		T		*GetCurrent(GPGListContext &context) const
				{
#if GPGLIST_CHECKPOINTER
					context.CheckCorePointer(this);
#endif
					GPGListNode *node=context.GetCurrent();
					return node? (T *)(node->GetPointer()): (T *)NULL;
				}

				//* convenience
		T		*PreIncrement(GPGListContext &context) const
				{
					PostIncrement(context);
					return GetCurrent(context);
				}

		T		*PreDecrement(GPGListContext &context) const
				{
					PostDecrement(context);
					return GetCurrent(context);
				}

		T		*GetElement(long index) const
				{	return (T *)CoreGetElement(index); }
	};


#endif /* __GPGList_h__ */
