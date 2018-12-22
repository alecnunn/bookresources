#ifndef GPL_STACK_H
#define GPL_STACK_H

/******************************************************************************

Coral Tree gpl/stack.h
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

#include <gpl/array.h>

/***********************************************************************
class GPL_Stack

Class T must have a default constructor, an assignment operator, and
the equality operator.
***********************************************************************/
template <class T>
class GPL_Stack
{
	public:
		GPL_Stack(const GPL_Boolean a = TRUE) : autoResize(a), index(0) {}
		GPL_Stack(const unsigned long size, const GPL_Boolean a = TRUE)
			: autoResize(a), index(0)
		{
			Resize(size);
		}
		virtual ~GPL_Stack(void) {}

		GPL_Boolean operator==(const GPL_Stack<T> & stack) const
		{
			return 0 == Compare(stack);
		}
		GPL_Boolean operator!=(const GPL_Stack<T> & stack) const
		{
			return 0 != Compare(stack);
		}
		T & operator [](const unsigned long i)
		{
			return elements[i];
		}
		const T & operator [](const unsigned long i) const
		{
			return elements[i];
		}

		unsigned long GetSize(void) const
		{
			return elements.GetSize();
		}
		unsigned long GetCount(void) const
		{
			return index;
		}
		long Compare(const GPL_Stack<T> & stack) const
		{
			if (index == stack.index)
			{
				return elements.Compare(stack.elements);
			}
			else
			{
				return stack.index - index;
			}
		}
		void Clear(void)
		{
			index = 0;
			elements.Clear();
		}
		void Resize(const unsigned long size)
		{
			elements.Resize(size);
		}

		GPL_Boolean Peek(T & element, const unsigned long depth = 0) const
		{
			if (depth < index && index <= elements.GetCount())
			{
				element = elements[index - (1 + depth)];
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		GPL_Boolean Push(const T & element)
		{
			if (index < elements.GetCount())
			{
				elements[index++] = element;
				return TRUE;
			}
			else if (autoResize)
			{
				Resize(index + 1);
				elements[index++] = element;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		GPL_Boolean Pop(void)
		{
			T temp;
			return Pop(temp);
		}
		GPL_Boolean Pop(T & element)
		{
			if (0 < index && index <= elements.GetCount())
			{
				element = elements[index - 1];
				--index;
				if (autoResize)
				{
					Resize(index);
				}
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		T & Top(void) { return elements[index - 1]; }

	protected:
		GPL_Boolean autoResize;
		unsigned long index;
		GPL_Array<T> elements;
};

/******************************************************************************
WARNING: This is a convenience object.
It's state is not valid after other operations on the stack.
A Reset() should be done to make it valid again.
******************************************************************************/

/***********************************************************************
class GPL_StackIterator
***********************************************************************/
template <class T>
class GPL_StackIterator
{
	public:
		GPL_StackIterator(void) : stack((GPL_Stack<T> *) 0), index(-1) {}
		virtual ~GPL_StackIterator(void) {}
		void Reset(GPL_Stack<T> *s)
		{
			stack = s;
			index = stack->GetCount();
		}
		GPL_Boolean Next(void)
		{
			return (0 <= --index);
		}

		T & Value(void)
		{
			return (*stack)[index];
		}

	protected:
		GPL_Stack<T> *stack;
		long index;
};

#endif /* GPL_STACK_H */
