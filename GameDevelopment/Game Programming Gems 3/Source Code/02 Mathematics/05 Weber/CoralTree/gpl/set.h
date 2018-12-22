#ifndef GPL_SET_H
#define GPL_SET_H

/******************************************************************************

Coral Tree gpl/set.h
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

#include <gpl/list.h>

#define GPL_COMPARE(a,b,c)	((a) ? (a)((b),(c)) : ((b)==(c)))

/***********************************************************************
class GPL_Set

Class T must have a default constructor, an assignment operator, and
an equivalence operator.
***********************************************************************/
template <class T>
class GPL_Set
{
	public:
		GPL_Set(GPL_Boolean (*c) (const T & b, const T & c) = NULL)
			: compareFunction(c) {}
		virtual ~GPL_Set(void) {}

		GPL_Set<T> operator|(const GPL_Set<T> & set) { return Join(set); }
		GPL_Set<T> operator&(const GPL_Set<T> & set) { return Intersect(set); }
		GPL_Set<T> operator-(const GPL_Set<T> & set) { return Subtract(set); }

		inline GPL_Boolean operator==(const GPL_Set<T> & set) const
		{
			return elements == set.elements;
		}
		inline GPL_Boolean operator!=(const GPL_Set<T> & set) const
		{
			return elements != set.elements;
		}
		inline T & operator[](const unsigned long i)
		{
			return elements[i];
		}
		inline const T & operator[](const unsigned long i) const
		{
			return elements[i];
		}

		inline unsigned long GetCount(void) const
		{
			return elements.GetCount();
		}
		inline unsigned long GetSize(void) const
		{
			return elements.GetSize();
		}
		inline const T & GetElement(const unsigned long i) const
		{
			return elements[i];
		}	

		GPL_Boolean Contains(const T & data) const
		{
			if (-1 == Find(data))
			{
				return FALSE;
			}
			return TRUE;
		}
		GPL_Boolean Contains(const GPL_Set<T> & set) const
		{
			register const unsigned long n = set.GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (!Contains(set.elements[i]))
				{
					return FALSE;
				}
			}
			return TRUE;
		}
		long Find(const T & data) const
		{
			register const unsigned long n = GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (GPL_COMPARE(compareFunction, data, elements[i]))
				{
					return (long) i;
				}
			}
			return -1;
		}
		virtual long Compare(const GPL_Set<T> & set) const
		{
			if (Contains(set))
			{
				if (set.Contains(*this))
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			else
			{
				return -1;
			}
		}

		inline void Clear(void) { elements.Clear(); }
		void Insert(const T & data)
		{
			if (!Contains(data))
			{
				elements.Insert(data);
			}
		}
		void Remove(const T & data)
		{
			register const long i = Find(data);
			if (-1 != i)
			{
				elements.Remove(i);
			}
		}

		GPL_Set<T> Join(const GPL_Set<T> & set) const
		{
			GPL_Set<T> s(set);
			register const unsigned long n = GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (!s.Contains(elements[i]))
				{
					s.elements.Append(elements[i]);
				}
			}
			return s;
		}
		GPL_Set<T> Intersect(const GPL_Set<T> & set) const
		{
			GPL_Set<T> s;
			register const unsigned long n = GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (set.Contains(elements[i]))
				{
					s.elements.Append(elements[i]);
				}
			}
			return s;
		}
		GPL_Set<T> Subtract(const GPL_Set<T> & set) const
		{
			GPL_Set<T> s;
			register unsigned long n = GetCount();
			register unsigned long i = 0;
			for (i = 0; n > i; ++i)
			{
				if (!set.Contains(elements[i]))
				{
					s.elements.Append(elements[i]);
				}
			}
			n = set.GetCount();
			for (i = 0; n > i; ++i)
			{
				if (!Contains(set.elements[i]))
				{
					s.elements.Append(set.elements[i]);
				}
			}
			return s;
		}

	protected:
		GPL_Boolean (*compareFunction) (const T & b, const T & c);
		GPL_List<T> elements;
};

#endif /* GPL_SET_H */
