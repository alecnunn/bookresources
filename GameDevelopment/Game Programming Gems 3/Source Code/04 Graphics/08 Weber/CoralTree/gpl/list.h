#ifndef GPL_LIST_H
#define GPL_LIST_H

/******************************************************************************

Coral Tree gpl/list.h
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

#include <gpl/typedefs.h>
#include <gpl/ref.h>

/***********************************************************************
class GPL_List

Class T must have a default constructor, an assignment operator, and
the equality operator.
***********************************************************************/

/***********************************************************************
class GPL_ListNode

***********************************************************************/
template <class T>
class GPL_ListNode
{
	public:
		T data;
		GPL_ListNode<T> *next;

		GPL_ListNode(const T & d, GPL_ListNode<T> *n = (GPL_ListNode<T> *) 0)
			: data(d), next(n) {}
		~GPL_ListNode(void) {}
};

/***********************************************************************
class GPL_ListRep

***********************************************************************/
template <class T>
class GPL_ListRep : public GPL_ReferenceRep
{
	public:
		unsigned long size;
		GPL_ListNode<T> *first;
		GPL_ListNode<T> **current;
		unsigned long index;

		GPL_ListRep(void) : size(0), first((GPL_ListNode<T> *) 0), index(0)
		{
			current = &first;
		}
		GPL_ListRep (const GPL_ListRep<T> & rep)
			: size(0), first((GPL_ListNode<T> *) 0), index(0)
		{
			Copy(rep.first);
			current = &first;
		}
		virtual ~GPL_ListRep(void)
		{
			Delete();
		}

	protected:
		void Copy(GPL_ListNode<T> *l)
		{
			size = 0;
			register GPL_ListNode<T> **node = &first;
			register GPL_ListNode<T> *t = l;
			while ((GPL_ListNode<T> *) 0 != t)
			{
				*node = new GPL_ListNode<T>(t->data);
				++size;
				node = &((*node)->next);
				t = t->next;
			}
		}
		void Delete(void)
		{
			register GPL_ListNode<T> *t = first;
			register GPL_ListNode<T> *u;
			while ((GPL_ListNode<T> *) 0 != t)
			{
				u = t;
				t = t->next;
				delete u;
				--size;
			}
		}
};


/***********************************************************************
class GPL_List

Generic list of values.
***********************************************************************/
template <class T>
class GPL_List : public GPL_Reference
{
	public:
		GPL_List(void) {}
		GPL_List(const GPL_List<T> & list) { Attach(list); }
		virtual ~GPL_List(void) {}

		GPL_List<T>	& operator=(const GPL_List<T> & list)
		{
			Detach();
			Attach(list);
			return *this;
		}

		GPL_Boolean operator==(const GPL_List<T> & list) const
		{
			return 0 == Compare(list);
		}
		GPL_Boolean operator!=(const GPL_List<T> & list) const
		{
			return 0 != Compare(list);
		}
		T & operator[](const unsigned long i)
		{
			Privatize();
			SetCurrent(i);
			register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
			return (*(r->current))->data;
		}

		const T & operator[](const unsigned long i) const
		{
			SetCurrent(i);
			register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
			return (*(r->current))->data;
		}

		unsigned long GetSize(void) const
		{
			register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
			return IsValid() ? r->size : 0;
		}
		unsigned long GetCount(void) const
		{
			register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
			return IsValid() ? r->size : 0;
		}
		const T & GetElement(const unsigned long i) const
		{
			return operator[](i);
		}	

		virtual long Compare(const GPL_List<T> & list) const
		{
			const long sourceIsValid = IsValid();
			const long destinationIsValid = list.IsValid();
			if (!sourceIsValid && !destinationIsValid)
			{
				return 0;
			}
			if (!sourceIsValid || !destinationIsValid)
			{
				return -1;
			}
			register GPL_ListNode<T> *source =
				((GPL_ListRep<T> *) rep)->first;
			register GPL_ListNode<T> *destination =
				((GPL_ListRep<T> *) list.rep)->first;
			while ((GPL_ListNode<T> *) 0 != source &&
					(GPL_ListNode<T> *) 0 != destination)
			{
				if (source->data == destination->data)
				{
					source = source->next;
					destination = destination->next;
				}
				else
				{
					return -1;
				}
			}
			return 0;
		}
		GPL_Boolean Contains(const T & data) const
		{
			return -1 != Find(data);
		}
		long Find(const T & data) const
		{
			if (IsValid())
			{
				register long i = 0;
				register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
				register GPL_ListNode<T> *t = r->first;
				while ((GPL_ListNode<T> *) 0 != t)
				{
					if (data == t->data)
					{
						return i;
					}
					t = t->next;
					++i;
				}
			}
			return -1;
		}

		void Clear(void) { Detach(); }
		void SetElement(const T & data, const unsigned long i)
		{
			operator[](i) = data;
		}
		void Concatenate(const GPL_List<T> & l)
		{
			if (l.IsValid() && 0 != l.GetCount())
			{
				if (!IsValid())
				{
					rep = new GPL_ListRep<T>;
				}
				else
				{
					Privatize();
				}

				register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
				SetCurrent(r->size - 1);

				GPL_ListRep<T> *s = (GPL_ListRep<T> *) l.rep;
				register GPL_ListNode<T> *p = s->first;
				for (register unsigned long i = 0; s->size > i; ++i)
				{
					r->current = &((*(r->current))->next);
					*(r->current) = new GPL_ListNode<T>(p->data, *(r->current));
					r->size++;
					p = p->next;
				}
			}
		}
		void Insert(const T & data, const unsigned long at = 0)
		{
			if (!IsValid())
			{
				rep = new GPL_ListRep<T>;
			}
			else
			{
				Privatize();
			}
			SetCurrent(at);
			register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
			*(r->current) = new GPL_ListNode<T>(data, *(r->current));
			r->size++;
		}
		void Append(const T & data) { Insert(data, GetCount()); }
		void Remove(const unsigned long i)
		{
			if (IsValid())
			{
				Privatize();
				SetCurrent(i);
				register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
				register GPL_ListNode<T> *t = *(r->current);
				*(r->current) = (*(r->current))->next;
				delete t;
				r->size--;
			}
		}

		virtual void DataChanged(void) {}

	protected:
		unsigned long GetCurrent(void) const
		{
			register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
			return IsValid() ? r->index : 0;
		}
		void SetCurrent(const unsigned long i) const
		{
			if (IsValid())
			{
				// If the current index is past the desired index...
				register GPL_ListRep<T> *r = (GPL_ListRep<T> *) rep;
				if (i < r->index)
				{
					// Go back to the beginning.
					r->current = &(r->first);
					r->index = 0;
				}
		
				// While the current index is before the desired index...
				while (i > r->index && (GPL_ListNode<T> *) 0 != *(r->current))
				{
					// Advance it.
					r->current = &((*(r->current))->next);
					r->index++;
				}
			}
		}
		virtual	void Privatize(void)
		{
			// If this reference is sharing a representation...
			if (IsShared())
			{
				register GPL_ListRep<T> * newRep =
					new GPL_ListRep<T>(*((GPL_ListRep<T> *) rep));
				--rep->count;
				rep = newRep;
			}
		}
};

#endif /* GPL_LIST_H */
