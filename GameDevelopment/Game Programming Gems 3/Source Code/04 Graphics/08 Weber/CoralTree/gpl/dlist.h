
/******************************************************************************

Coral Tree gpl/dlist.h
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
There are actually 2 types of linked lists here, an intrusive list and a 
non intrusive list.  The intrusive list requires that the node type be derived
from GPL_DoubleNode.  This means that a node can only be in a single intrusive
list at a time.  However, intrusive lists are more efficient.

******************************************************************************/


class GPL_DoubleNode;

/******************************************************************************
class	GPL_Core_DoubleList

keywords: blackbox, template, c++, data, linked_list
******************************************************************************/
class GPL_Core_DoubleList
{
	public:

static	long			count;

	private:

		GPL_DoubleNode	*HeadNode;
		GPL_DoubleNode	*TailNode;
		GPL_DoubleNode	*CurrentNode;	// NULL= pre-beginning or post-end
		long			nodes;
		long			end;			// FALSE=beginning NULL or on list, TRUE=ending NULL

	public:

						GPL_Core_DoubleList(void);
virtual					~GPL_Core_DoubleList(void);

		void			Reset(void);

		void			ValidCheck();
		long			RemoveNode(GPL_DoubleNode *node);
		GPL_DoubleNode	*PostIncrement(void)
							{ return PostIncrement(&CurrentNode,&end); }
		GPL_DoubleNode	*PostIncrement(GPL_DoubleNode **current_node,
															long *the_end);
		GPL_DoubleNode	*PostDecrement(void);

		void			Clear(void);

		long			Insert(GPL_DoubleNode *node,long after);
		long			Insert(GPL_Core_DoubleList *doublelist,long after);

		long			InsertBefore(GPL_DoubleNode *node)
							{
							return Insert(node,FALSE);
							};
		long			InsertBefore(GPL_Core_DoubleList *doublelist)
							{
							return Insert(doublelist,FALSE);
							};

		long			InsertAfter(GPL_DoubleNode *node)
							{
							return Insert(node,TRUE);
							};
		long			InsertAfter(GPL_Core_DoubleList *doublelist)
							{
							return Insert(doublelist,TRUE);
							};

		long			SetCurrent(GPL_DoubleNode *node)
							{
							CurrentNode=node;
							return TRUE;
							};

		GPL_DoubleNode	*GetCurrent(void)
							{
							return CurrentNode;
							};

		long			GetNumberNodes(void)	{ return nodes; };

		GPL_DoubleNode	*ToHead(void){return ToHead(&CurrentNode,&end);}
		GPL_DoubleNode	*ToHead(GPL_DoubleNode **current_node, long *the_end)
							{
							*current_node=HeadNode;
							*the_end=FALSE;
							return *current_node;
							};

		GPL_DoubleNode	*ToTail(void)
							{
							CurrentNode=TailNode;
							end=FALSE;
							return GetCurrent();
							};

		////////// Convenience //////////

		GPL_DoubleNode	*ToIndex(long index);

		GPL_DoubleNode	*PreIncrement(void)
							{
							PostIncrement();
							return GetCurrent();
							};

		GPL_DoubleNode	*PreDecrement(void)
							{
							PostDecrement();
							return GetCurrent();
							};

		long AppendNode(GPL_DoubleNode *node)
							{
							ToTail();
							return InsertAfter(node);
							};
		long AppendList(GPL_Core_DoubleList *doublelist)
							{
							ToTail();
							return InsertAfter(doublelist);
							};


		////////// Historical //////////
		GPL_DoubleNode	*Head(void)
							{
							return HeadNode;
							};

		GPL_DoubleNode	*Next(void)
							{
							return PostIncrement();
							};

		GPL_DoubleNode	*Current(void)
							{
							return GetCurrent();
							};

		void			Rewind(void)
							{
							ToHead();
							};
};



/******************************************************************************
class GPL_DoubleNode

keywords: blackbox, template, c++, data, linked_list
******************************************************************************/
class GPL_DoubleNode
{
	public:
static	long				count;

	private:

		GPL_Core_DoubleList	*ParentListPtr;
		GPL_DoubleNode		*NextNode;
		GPL_DoubleNode		*PrevNode;

	public:

							GPL_DoubleNode();
virtual						~GPL_DoubleNode();

		void				SetNext(GPL_DoubleNode *node);
		void				SetPrev(GPL_DoubleNode *node);
		void				SetParentList(GPL_Core_DoubleList *list);

		GPL_DoubleNode		*Next();
		GPL_DoubleNode		*Prev();
		GPL_Core_DoubleList	*ParentList();
};



/******************************************************************************
template<class L>
class GPL_IDoubleList : private GPL_Core_DoubleList

	templated intrusive doubly-linked list of any class L

keywords: blackbox, template, c++, data, linked_list
******************************************************************************/
template<class L>
class GPL_IDoubleList : private GPL_Core_DoubleList
{
	private:
		long	HasNode(L *node)
					{
					L *anode;

					GPL_Core_DoubleList::ToHead();
					while((anode = (L *)GPL_Core_DoubleList::PostIncrement()))
						{
						if(anode == node)
							return TRUE;
						}

					return FALSE;
					};
	public:
				GPL_IDoubleList(){}
		virtual	~GPL_IDoubleList(){}

		void	Reset(void)
					{ GPL_Core_DoubleList::Reset(); }
		void	Clear(void)
					{ GPL_Core_DoubleList::Clear(); }
		long	InsertBefore(L *node)
					{ return GPL_Core_DoubleList::InsertBefore(node); };
		long	InsertAfter(L *node)
					{ return GPL_Core_DoubleList::InsertAfter(node); };
		long	AppendNode(L *node)
					{ return GPL_Core_DoubleList::AppendNode(node); };
		long	RemoveNode(L *node)
					{ return GPL_Core_DoubleList::RemoveNode(node); };
		long	SetCurrent(L *node)
					{
					if(HasNode(node))
						return GPL_Core_DoubleList::SetCurrent(node);
					else
						return FALSE;
					};
		L		*GetCurrent(void)
					{ return (L *)GPL_Core_DoubleList::GetCurrent(); };
		long	GetNumberNodes(void)
					{ return GPL_Core_DoubleList::GetNumberNodes(); };
		L		*ToHead(void)
					{ return (L *)GPL_Core_DoubleList::ToHead(); };
		L		*ToTail(void)
					{ return (L *)GPL_Core_DoubleList::ToTail(); };
		L		*ToIndex(long index)
					{ return (L *)GPL_Core_DoubleList::ToIndex(index); };
		L		*PreDecrement(void)
					{ return (L *)GPL_Core_DoubleList::PreDecrement(); };
		L		*PostDecrement(void)
					{ return (L *)GPL_Core_DoubleList::PostDecrement(); };
		L		*PreIncrement(void)
					{ return (L *)GPL_Core_DoubleList::PreIncrement(); };
		L		*PostIncrement(void)
					{ return (L *)GPL_Core_DoubleList::PostIncrement(); };

		////////// Historical //////////
		L		*Head(void)
					{ return (L *)GPL_Core_DoubleList::Head(); };
		L		*Next(void)
					{ return (L *)GPL_Core_DoubleList::Next(); };
		L		*Current(void)
					{ return (L *)GPL_Core_DoubleList::Current(); };
		void	Rewind(void)
					{ GPL_Core_DoubleList::Rewind(); }
};


/******************************************************************************
		template<class D>
class	GPL_NotIntrDoubleNode : public GPL_DoubleNode

keywords: blackbox, template, c++, data, linked_list
******************************************************************************/
template<class D>
class GPL_NotIntrDoubleNode : public GPL_DoubleNode
{
	public:
		D *data;
		GPL_NotIntrDoubleNode(D *new_data)
		{
			data = new_data;
		}
		virtual ~GPL_NotIntrDoubleNode(){}
};


/******************************************************************************
template<class D>
class GPL_DoubleList : public GPL_Core_DoubleList

keywords: blackbox, template, c++, data, linked_list
******************************************************************************/
template<class D>
class GPL_DoubleList : public GPL_Core_DoubleList
{
	private:
		D		*Data(GPL_NotIntrDoubleNode<D> *node)
					{
					if(node)
						return (D *)node->data;
					else
						return NULL;
					};

		long	Insert(D *node,long after)
					{
					GPL_NotIntrDoubleNode<D> *dnode;

					dnode = new GPL_NotIntrDoubleNode<D>(node);

					return GPL_Core_DoubleList::Insert(dnode,after);
					}

		long	Insert(GPL_DoubleList *doublelist,long after)
					{
					return GPL_Core_DoubleList::Insert(doublelist,after);
					}

		GPL_NotIntrDoubleNode<D> *FindDNode(D *node)
					{
					GPL_NotIntrDoubleNode<D> *dnode;

					GPL_Core_DoubleList::Rewind();
					while((dnode = (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::Next()))
						{
						if(dnode->data == node)
							return dnode;
						}

					return NULL;
					}

	public:
				GPL_DoubleList(){}
virtual			~GPL_DoubleList(){}

		void	Reset(void)		{ GPL_Core_DoubleList::Reset(); };
		void	Clear(void)		{ GPL_Core_DoubleList::Clear(); };

		long	RemoveNode(D *node)
					{
					/* WE HAVE TO MANUALLY FIND NODE */
					GPL_NotIntrDoubleNode<D> *dnode=FindDNode(node);

					if(dnode)
						{
						long return_val;
						return_val =  GPL_Core_DoubleList::RemoveNode(dnode);
						delete dnode;
						return return_val;
						}
					else
						{
						return FALSE;
						}
					}

		D		*PostIncrement(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::PostIncrement() );
					};

		D		*PostDecrement(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::PostDecrement() );
					};

		long	InsertBefore(D *node) 
					{ 
					return Insert(node,FALSE);
					};
		long	InsertBefore(GPL_DoubleList *doublelist) 
					{ 
					return Insert(doublelist,FALSE);
					};

		long	InsertAfter(D *node) 
					{ 
					return Insert(node,TRUE);
					};
		long	InsertAfter(GPL_DoubleList *doublelist) 
					{ 
					return Insert(doublelist,TRUE);
					};

		long	SetCurrent(D *node)
					{ 
					/* WE HAVE TO MANUALLY FIND NODE */
					GPL_NotIntrDoubleNode<D> *dnode=FindDNode(node);

					if(dnode)
						return GPL_Core_DoubleList::SetCurrent(dnode);
					else
						return FALSE;
					}

		D		*GetCurrent(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::GetCurrent() );
					}

		long	GetNumberNodes(void)
					{ 
					return GPL_Core_DoubleList::GetNumberNodes();
					}

		D		*ToHead(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::ToHead() );
					}

		D		*ToTail(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::ToTail() );
					}

		///////// Convenience //////////

		D		*ToIndex(long index)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::ToIndex(index) );
					}

		D		*PreIncrement(void)
					{
					PostIncrement();
					return Current();
					};

		D		*PreDecrement(void)
					{
					PostDecrement();
					return Current();
					};

		long	AppendNode(D *node)
					{
					ToTail();
					return InsertAfter(node);
					};
		long	AppendList(GPL_DoubleList *doublelist)
					{
					ToTail();
					return InsertAfter(doublelist);
					};

		////////// Historical //////////

		D		*Head(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleList::Head() );
					};

		D		*Current(void)
					{ 
					return GetCurrent();
					}

		D		*Next(void)		{ return PostIncrement(); };
		void	Rewind(void)	{ GPL_Core_DoubleList::Rewind(); };
};



/******************************************************************************
class	GPL_Core_DoubleListIterator

******************************************************************************/
class GPL_Core_DoubleListIterator
{
	public:
		static long 			count;
	protected:
		GPL_Core_DoubleList		*dlist;
		GPL_DoubleNode 			*CurrentNode;
		long					end;
	public:
		GPL_Core_DoubleListIterator()
		{
			count++;
		}
		GPL_Core_DoubleListIterator(GPL_Core_DoubleList	*a_dlist)
		{
			count++;
			Reset(a_dlist);
		}
		virtual ~GPL_Core_DoubleListIterator(){count--;}
		void Reset(GPL_Core_DoubleList	*a_dlist)
		{
			dlist = a_dlist;
			CurrentNode = NULL;
			end=FALSE;
		}
		void Reset(GPL_Core_DoubleListIterator *other, GPL_Boolean same_setting = FALSE)
		{
			dlist = other->dlist;
			if(same_setting)
			{
				CurrentNode = other->CurrentNode;
				end = other->end;
			}
			else
			{
				CurrentNode = NULL;
				end=FALSE;
			}
		}
		long SetCurrent(GPL_DoubleNode *node)
		{
			CurrentNode = node;
			return TRUE;
		}
		GPL_DoubleNode *PostIncrement(void)
		{
			return dlist->PostIncrement(&CurrentNode, &end);
		}
		GPL_DoubleNode *ToHead(void)
		{
			return dlist->ToHead(&CurrentNode, &end);
		}
};



/******************************************************************************
template<class D>
class GPL_DoubleListIterator : public GPL_Core_DoubleListIterator

******************************************************************************/
template<class D>
class GPL_DoubleListIterator : public GPL_Core_DoubleListIterator
{
	private:
		D		*Data(GPL_NotIntrDoubleNode<D> *node)
					{
					if(node)
						return (D *)node->data;
					else
						return NULL;
					};
	public:
		GPL_DoubleListIterator(){}
		GPL_DoubleListIterator(GPL_DoubleList<D> *a_dlist) :
			GPL_Core_DoubleListIterator(a_dlist){}
		virtual ~GPL_DoubleListIterator(){}
		void	Reset(GPL_DoubleList<D> *a_dlist)
					{
					GPL_Core_DoubleListIterator::Reset(a_dlist);
					}
		void	Reset(GPL_DoubleListIterator<D> *other, GPL_Boolean same_setting = FALSE)
					{
					GPL_Core_DoubleListIterator::Reset(other, same_setting);
					}
		GPL_NotIntrDoubleNode<D> *FindDNode(D *node)
					{
					GPL_NotIntrDoubleNode<D> *dnode;

					dlist->Rewind();
					while((dnode = (GPL_NotIntrDoubleNode<D> *)dlist->Next()))
						{
						if(dnode->data == node)
							return dnode;
						}

					return NULL;
					}
		long	SetCurrent(D *node)
					{
					/* WE HAVE TO MANUALLY FIND NODE */
					GPL_NotIntrDoubleNode<D> *dnode=FindDNode(node);
					if(dnode)
						{
							return GPL_Core_DoubleListIterator::SetCurrent(dnode);
						}
					else
						{
							return FALSE;
						}
					}
		D		*PostIncrement(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleListIterator::PostIncrement() );
					};
		D		*ToHead(void)
					{ 
					return Data( (GPL_NotIntrDoubleNode<D> *)GPL_Core_DoubleListIterator::ToHead() );
					}
};


