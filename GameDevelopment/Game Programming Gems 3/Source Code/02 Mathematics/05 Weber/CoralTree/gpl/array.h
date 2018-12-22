#ifndef GPL_ARRAY_H
#define GPL_ARRAY_H

/******************************************************************************

Coral Tree gpl/array.h
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

#include <osd/osd.h>

#include <gpl/typedefs.h>
#include <gpl/ref.h>

/***********************************************************************
class GPL_ArrayRep

Array representation
***********************************************************************/
template <class T>
class GPL_ArrayRep : public GPL_ReferenceRep
{
	public:
		T *data;
		unsigned long size;
		unsigned long last;

		GPL_ArrayRep(void) : data((T *) 0), size(0), last(0) {}
		GPL_ArrayRep(const unsigned long n) : data((T *) 0), size(n), last(0)
		{
			if (0 != size)
			{
				data = new T[size];
			}
		}
		GPL_ArrayRep(const GPL_ArrayRep<T> & rep)
		{
			data = (0 == rep.size) ? (T *) 0 : new T[rep.size];
			if(0 == rep.size)
			{
				data = (T *) 0;
			}
			else
			{
				data = new T[rep.size];
			}
			for (register unsigned long i = 0; rep.size > i; ++i)
			{
				data[i] = rep.data[i];
			}
			size = rep.size;
			last = rep.last;
		}
		virtual ~GPL_ArrayRep(void)
		{
			if ((T *) 0 != data)
			{
				delete [] data;
			}
			size = 0;
			last = 0;
		}
};

#ifndef GPL_MAXIMUM
#define GPL_MAXIMUM(a,b)	(((a) < (b)) ? (b) : (a))
#endif
#ifndef GPL_MINIMUM
#define GPL_MINIMUM(a,b)	(((a) < (b)) ? (a) : (b))
#endif

/***********************************************************************
class GPL_Array

Class T must have a default constructor, an assignment operator, and
the equality operator.
***********************************************************************/
template <class T>
class GPL_Array : public GPL_Reference
{
	public:
		GPL_Array(void){}
		GPL_Array(const unsigned long size)
		{
			Resize(size);
		}
		GPL_Array(const unsigned long size, const T & initial)
		{
			Resize(size, initial);
		}
		GPL_Array(const GPL_Array<T> & array) : GPL_Reference(array) {}
		virtual ~GPL_Array(void) {}

		GPL_Array<T> & operator=(const GPL_Array<T> & array)
		{
			Detach();
			Attach(array);
			return *this;
		}
		T & operator[](const unsigned long i)
		{
			Privatize();
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			if (i < r->size && i >= r->last)
			{
				r->last = i + 1;
			}
			return r->data[i];
		}
		const T & operator[](const unsigned long i) const
		{
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			return r->data[i];
		}
		operator T *(void) { return GetData(); }
		operator T *(void) const { return GetData(); }
		operator const T *(void) const { return GetData(); }

		GPL_Boolean operator==(const GPL_Array<T> & array) const
		{
			return 0 == Compare(array.GetData(), array.GetCount());
		}
		GPL_Boolean operator!=(const GPL_Array<T> & array) const
		{
			return 0 != Compare(array.GetData(), array.GetCount());
		}

		unsigned long GetSize(void) const
		{
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			return IsValid() ? r->size : 0;
		}
		unsigned long GetCount(void) const
		{
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			return IsValid() ? r->last : 0;
		}
		T * GetData(void) const
		{
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			return IsValid() ? r->data : (T *) 0;
		}
		const T & GetElement(const unsigned long i) const
		{
			return operator[](i);
		}
		GPL_Array<T> GetRange(const unsigned long from,
								const unsigned long to) const
		{
			GPL_Array<T> temp;
			if (IsValid())
			{
				register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
				register const unsigned long end =
					(r->last - 1 < to) ? r->last - 1 : to;
				if (from <= end)
				{
					temp.Resize(1 + end - from);
					register unsigned long j = 0;
					for (register unsigned long i = from; end >= i; ++i)
					{
						// We know that temp.rep is not null
						// since we called Resize().
						((GPL_ArrayRep<T> *) temp.rep)->data[j++] = r->data[i];
					}
					((GPL_ArrayRep<T> *) temp.rep)->last = j;
				}
			}
			return temp;
		}
		void CopyToBuffer(T *buffer) const
		{
			if ((T *) 0 != buffer)
			{
				register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
				register const unsigned long last = GetCount();
				for (register unsigned long i = 0; last > i; ++i)
				{
					buffer[i] = r->data[i];
				}
			}
		}
		long Compare(const GPL_Array<T> & array) const
		{
			return Compare(array.GetData(), array.GetCount());
		}
		long Compare(T *data, const unsigned long count,
							const unsigned long from = 0) const

		{
			const unsigned long arrayCount = GetCount();
			const GPL_Boolean arrayIsValid =
							IsValid() && 0 < arrayCount;
			const GPL_Boolean dataIsValid =
							(T *) 0 != data && 0 < count;
			if (!arrayIsValid && !dataIsValid)
			{
				return 0;
			}
			if (!arrayIsValid)
			{
				return -1;
			}
			if (!dataIsValid)
			{
				return 1;
			}
			if (arrayCount < from || count > arrayCount - from)
			{
				return ((long) from) - ((long) arrayCount);
			}
			register T *arrayData = GetData();
			register unsigned long i = 0;
			register unsigned long j = from;
			while (count > i)
			{
				if (data[i] == arrayData[j])
				{
					++i;
					++j;
				}
				else
				{
					return -1;
				}
			}
			return 0;
		}
		void SetData(T *data, const unsigned long count)
		{
			Privatize();
			Resize(count);
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			r->last = count;
			if ((T *) 0 != data)
			{
				for (register unsigned long i = 0; count > i; ++i)
				{
					r->data[i] = data[i];
				}
			}
		}
		void SetElement(const T & element, const unsigned long i)
		{
			operator[](i) = element;
		}
		void SetRange(const GPL_Array<T> & array, const unsigned long at = 0)
		{
			SetRange(array.GetData(),array.GetCount(),at);
		}
		void SetRange(const T *data, const unsigned long count,
						const unsigned long at = 0)
		{
			Privatize();
			if ((T *) 0 != data && 0 < count)
			{
				if (count > GetSize() - at)
				{
					Resize(at + count);
				}
				register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
				register T *arrayData = r->data;
				register unsigned long i = 0;
				register unsigned long j = at;
				while (count > i)
				{
					arrayData[j++] = data[i++];
				}
				if (at + count > r->last)
				{
					r->last = at + count;
				}
			}
		}
		void Clear(void)
		{
			Detach();
		}
		void Resize(const unsigned long size)
		{
			T initial;
			Resize(size, initial);
		}
		void Resize(const unsigned long size, const T & initial)
		{
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			register GPL_ArrayRep<T> *newRep = new GPL_ArrayRep<T>(size);
			if (IsValid()/* && (size != r->size)*/)
			{
				const unsigned long n = (size < r->last) ? size : r->last;
				for (register unsigned long i = 0; n > i; ++i)
				{
					newRep->data[i] = r->data[i];
				}
				newRep->last = n;
			}
			for (register unsigned long i = newRep->last; size > i; ++i)
			{
				newRep->data[i] = initial;
			}
			Detach();
			rep = newRep;
		}
		GPL_Boolean InsertElement(const T & element,
									const unsigned long at = 0)
		{
			return InsertRange(&element, 1, at);
		}
		GPL_Boolean InsertRange(const GPL_Array<T> & array,
								const unsigned long at = 0)
		{
			return InsertRange(array.GetData(),
								array.GetCount(), at);
		}
		GPL_Boolean InsertRange(const T *data, const unsigned long count,
								const unsigned long at = 0)
		{
/* COMPILER BUG HACK , throw in a bunch of goo to get non-debug to work */
#if OSD_OS==OSD_SUNOS && OSD_OSVER==5 && OSD_COMPILER==OSD_ANSI
long a;
a = 10;
a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1;
a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1;
a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1; a += 1;
#endif
			Privatize();
			// If the insertion occurs before existing elements...
			register const unsigned long c = GetCount();
			if (at < c)
			{
#define GPL_USE_NON_TWEAKED_MOVERANGE_RETURN
#ifdef GPL_USE_NON_TWEAKED_MOVERANGE_RETURN
				if (!MoveRange(c - at, at, at + count))
				{
					return FALSE;
				}
#else
				GPL_Boolean mr_ret;
				mr_ret = MoveRange(c - at, at, at + count);
fprintf(stderr,"MoveRange Return %d %d %d\n", mr_ret, TRUE, FALSE);
				//if(FALSE == mr_ret)
				if(0 == 1)
				{
fprintf(stderr,"MoveRange Return Failed\n");
					return FALSE;
				}
#endif
			}
			// If the storage is too small...
			else if (count + c > GetSize())
			{
				Resize(count + c);
			}
		
			// Set the elements in the insertion.
			SetRange(data, count, at);
			return TRUE;
		}
		GPL_Boolean AppendElement(const T & element)
		{
			return InsertRange(&element, 1, GetCount());
		}
		GPL_Boolean AppendRange(const GPL_Array<T> & array)
		{
			return InsertRange(array, GetCount());
		}
		GPL_Boolean RemoveElement(const unsigned long i)
		{
			return RemoveRange(i, i);
		}
		GPL_Boolean RemoveRange(const unsigned long from,
								const unsigned long to)
		{
			register const unsigned long count = GetCount();
			if (from <= to && to < count)
			{
				return MoveRange(count - (1 + to), to + 1, from);
			}
			return FALSE;
		}
		GPL_Boolean MoveRange(const unsigned long count,
								const unsigned long from,
								const unsigned long to)

		{
			Privatize();
			// If the move would accomplish nothing...
			if (from == to)
			{
				return TRUE;
			}
		
			// If there is nothing to move or these indices make no sense...
			const unsigned long c = GetCount();
			if (!IsValid() || from + count > c)
			{
				return FALSE;
			}
		
			// Create the new representation.
			register GPL_ArrayRep<T> *r = (GPL_ArrayRep<T> *) rep;
			register GPL_ArrayRep<T> *newRep =
				new GPL_ArrayRep<T>((GPL_MAXIMUM(from,to) + count < c) ? c : to + count);
			newRep->last = newRep->size;
		
			// Copy the front part.
			const unsigned long n = GPL_MINIMUM(from,to);
			register unsigned long i = 0;
			while (n > i)
			{
				newRep->data[i] = r->data[i];
				++i;
			}
		
			// Copy the middle part.
			i = 0;
			while (count > i)
			{
				newRep->data[to + i] = r->data[from + i];
				++i;
			}
		
			// Copy the back part.
			i = GPL_MAXIMUM(from,to) + count;
			while (c > i)
			{
				newRep->data[i] = r->data[i];
				++i;
			}
		
			// Use the new representation.
			Detach();
			rep = newRep;
			return TRUE;
		}
		virtual	void DataChanged(void) {}

	protected:
		virtual void Privatize(void)
		{
			// If this reference is sharing a representation...
			GPL_ArrayRep<T> *newRep = (GPL_ArrayRep<T> *) GPL_BAD_ADDRESS;
			const GPL_Boolean isShared = IsShared();
			if (isShared)
			{
				newRep = new GPL_ArrayRep<T>(*((GPL_ArrayRep<T> *) rep));
			}
			if (isShared)
			{
				--rep->count;
			}
			if (isShared)
			{
				rep = newRep;
			}
		}
};


#define GPL_MAXPRINTFSTRINGSIZE 	256
/******************************************************************************
class GPL_CharPtr

Array derived type for chars.
******************************************************************************/
class GPL_CharPtr : public GPL_Array<char>
{
	public:
		GPL_CharPtr(void) {}
		GPL_CharPtr(char *buffer, unsigned long buflen)
		{
			SetData((char *)buffer, buflen);
		}
		GPL_CharPtr(const char *string)
		{
			SetData((char *)string,
				(((char *) 0 == (string)) ? 0 : strlen(string))+1);
		}
		GPL_CharPtr(char *string)
		{
			SetData((char *)string,
				(((char *) 0 == (string)) ? 0 : strlen(string))+1);
		}
		GPL_CharPtr(const GPL_Array<char> &array) : GPL_Array<char>(array) {}
		GPL_CharPtr(const GPL_CharPtr &c) : GPL_Array<char>(c) {}
		virtual	~GPL_CharPtr(void) {}
		GPL_CharPtr& operator=(const char *string)
		{
			SetData((char *) string,
				(((char *) 0 == (string)) ? 0 : strlen(string))+1);
			return *this;
		}
		GPL_CharPtr& operator=(const GPL_CharPtr &c)
		{
			Detach();
			Attach(c);
			return *this;
		}

		GPL_CharPtr GetRange(const unsigned long from, const unsigned long to) const
		{
			GPL_Array<char> t = GPL_Array<char>::GetRange(from, to);
			t.Resize(1 + t.GetCount());
			t[t.GetCount()] = (char) 0;
			return t;
		}
		void Printf(char *format, ...)
		{
			va_list args;
			va_start(args, format);
			char *s = osdVSPrintf(format, args);
			operator=(s);
			osdFree(s);
			va_end(args);
		}
		void Cat(const GPL_CharPtr other)
		{
			// remove NULL termination if there is one
			if(GetCount() > 0 && (GetElement(GetCount() - 1) == '\0'))
				RemoveElement(GetCount() - 1);
			InsertRange(other.GetData(), other.GetCount(), GetCount());
		}
};

#endif /* GPL_ARRAY_H */
