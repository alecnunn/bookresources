#ifndef GPL_PTR_H
#define GPL_PTR_H

/******************************************************************************

Coral Tree gpl/ptr.h
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
class GPL_Pointer

***********************************************************************/
template <class T>
class GPL_PointerRep : public GPL_ReferenceRep
{
	public:
		T *data;

		GPL_PointerRep(T * d = (T *) 0) : data(d) {}
		virtual ~GPL_PointerRep(void)
		{
			if ((T *) 0 != data)
			{
				delete data;
			}
		}
};

/***********************************************************************
class GPL_Pointer

Use for pointers to primitive data types probably will *NOT* work; it
is a crack in the language definitions that some compilers (e.g., Sun's)
handle extremely ungracefully.
***********************************************************************/
template <class T>
class GPL_Pointer : public GPL_Reference
{
	public:
		GPL_Pointer(void) {}
		GPL_Pointer(T *p)
		{
			rep = new GPL_PointerRep<T>(p);
		}
		GPL_Pointer(const GPL_Pointer<T> & p)
			: GPL_Reference(p)
		{
		}
		virtual ~GPL_Pointer(void) {}

		GPL_Pointer<T> & operator=(const GPL_Pointer<T> & p)
		{
			Detach();
			Attach(p);
			return *this;
		}
		GPL_Pointer<T> & operator=(T *p)
		{
			Detach();
			rep = new GPL_PointerRep<T>(p);
			return *this;
		}
		GPL_Boolean operator==(const GPL_Pointer<T> & p) const
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			register GPL_PointerRep<T> * s = (GPL_PointerRep<T> *) p.rep;
			if ((GPL_PointerRep<T> *) 0 == r || (T *) 0 == r->data)
			{
				return (GPL_PointerRep<T> *) 0 == s || (T *) 0 == s->data;
			}
			return (GPL_PointerRep<T> *) 0 != s && r->data == s->data;
		}
		GPL_Boolean operator!=(const GPL_Pointer<T> & p) const
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			register GPL_PointerRep<T> * s = (GPL_PointerRep<T> *) p.rep;
			if ((GPL_PointerRep<T> *) 0 == r || (T *) 0 == r->data)
			{
				return (GPL_PointerRep<T> *) 0 != s && (T *) 0 != s->data;
			}
			return (GPL_PointerRep<T> *) 0 == s || r->data != s->data;
		}

		operator T *(void)
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			return IsValid() ? r->data : (T *) 0;
		}
		operator const T *(void) const
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			return IsValid() ? (const T *) r->data : (const T *) 0;
		}
		T * operator->(void)
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			return IsValid() ? r->data : (T *) 0;
		}
		const T * operator->(void) const
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			return IsValid() ? (const T *) r->data : (const T *) 0;
		}

		GPL_Boolean IsNull(void) const { return !IsValid() || (T *) 0 == ((GPL_PointerRep<T> *) rep)->data; }
		T * GetAddress (void)
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			return IsValid() ? r->data : (T *) 0;
		}
		const T * GetAddress (void) const
		{
			register GPL_PointerRep<T> * r = (GPL_PointerRep<T> *) rep;
			return IsValid() ? (const T *) r->data : (const T *) 0;
		}

		void ConvertUnsafely(GPL_Reference *r)
		{
			if ((GPL_Reference *) 0 != r)
			{
				GPL_Reference::operator=(*r);
			}
		}

		void Clear(void) { Detach(); }

	protected:
		virtual void Privatize(void) {}
};


#endif /* GPL_PTR_H */
