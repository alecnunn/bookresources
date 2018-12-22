#ifndef GPL_MAP_H
#define GPL_MAP_H

/******************************************************************************

Coral Tree gpl/map.h
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
#include <gpl/array.h>
#include <gpl/list.h>
#include <gpl/pair.h>

#define GPL_HASH(a,b)	((a) ? (a)((b)) : 0)

/******************************************************************************
class GPL_MapRep

******************************************************************************/
template <class K, class V>
class GPL_MapRep : public GPL_ReferenceRep
{
	public:
		unsigned long (*hashFunction) (const K & k);
		GPL_Array< GPL_List< GPL_Pair<K, V> > > buckets;

		GPL_MapRep(void) : hashFunction(0) {}
		GPL_MapRep(unsigned long (*h) (const K & k)) : hashFunction(h) {}
		GPL_MapRep(const GPL_MapRep<K, V> & rep)
			: hashFunction(rep.hashFunction), buckets(rep.buckets) {}
		virtual ~GPL_MapRep(void) {}
};

/******************************************************************************
class GPL_Map

Classes K and V must have a default constructor, an assignment operator,
and the equality operator.
******************************************************************************/
template <class K, class V>
class GPL_Map : public GPL_Reference
{
	public:
		// This guarantees that rep is never NULL.  IsValid() is
		// not used, so rep must never be NULL.
		GPL_Map(unsigned long (*h) (const K & k) = NULL,
				const unsigned long initialSize = 4)
		{
			rep = (GPL_ReferenceRep *) new GPL_MapRep<K, V>(h);
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long size = (h) ? initialSize : 1;
			r->buckets.Resize(size);
			for (register unsigned long i = 0; size > i; ++i)
			{
				r->buckets[i].Clear();
			}
		}
		GPL_Map(const GPL_Map<K, V> & map) { Attach(map); }
		virtual ~GPL_Map(void) {}

		GPL_Map<K, V> & operator= (const GPL_Map<K, V> & map)
		{
			Detach();
			Attach(map);
			return *this;
		}
		V & operator[] (const K & key) { return Find(key); }
		GPL_Boolean operator==(const GPL_Map<K, V> & map) const
		{
			return 0 == Compare(map);
		}
		GPL_Boolean operator!=(const GPL_Map<K, V> & map) const
		{
			return 0 != Compare(map);
		}

		unsigned long GetCount(void) const
		{
			register unsigned long count = 0;
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long n = r->buckets.GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				count += r->buckets[i].GetCount();
			}
			return count;
		}
		unsigned long GetSize(void) const
		{
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			return r->buckets.GetSize();
		}
		GPL_Array<K> GetKeys(void) const
		{
			const unsigned long size = GetCount();
			GPL_Array<K> keys(size);
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			unsigned long i = 0;
			const unsigned long m = r->buckets.GetCount();
			register unsigned long j = 0;
			register unsigned long k = 0;
			register unsigned long n = 0;
			for (i = 0; m > i; ++i)
			{
				n = r->buckets[i].GetCount();
				for (j = 0; n > j; ++j)
				{
					keys[k++] = r->buckets[i][j].first;
				}
			}
			return keys;
		}

		long Compare(const GPL_Map<K, V> & map) const
		{
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register GPL_MapRep<K, V> *s = (GPL_MapRep<K, V> *) map.rep;
			register const unsigned long n = r->buckets.GetCount();
			if (n != s->buckets.GetCount())
			{
				return ((long) n) - ((long) s->buckets.GetCount());
			}
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (r->buckets[i] != s->buckets[i])
				{
					// Should this be some calculated value?
					return -1;
				}
			}
			return 0;
		}
		GPL_Boolean Contains(const K & key) const
		{
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long hash =
				GPL_HASH(r->hashFunction,key) % r->buckets.GetCount();
			register const unsigned long n = r->buckets[hash].GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (key == r->buckets[hash][i].first)
				{
					return TRUE;
				}
			}
			return FALSE;
		}
		GPL_Boolean LookUp(const K & key, V & value) const
		{
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long hash =
				GPL_HASH(r->hashFunction,key) % r->buckets.GetCount();
			register const unsigned long n = r->buckets[hash].GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (key == r->buckets[hash][i].first)
				{
					value = r->buckets[hash][i].second;
					return TRUE;
				}
			}
			return FALSE;
		}
		V & Find(const K & key)
		{
			Privatize();
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long hash =
				GPL_HASH(r->hashFunction,key) % r->buckets.GetCount();
			register const unsigned long n = r->buckets[hash].GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (key == r->buckets[hash][i].first)
				{
					return r->buckets[hash][i].second;
				}
			}
			GPL_Pair<K, V> pair(key, V());
			r->buckets[hash].Insert(pair, n);
			return r->buckets[hash][n].second;
		}
		GPL_Boolean Remove(const K & key)
		{
			Privatize();
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long hash =
				GPL_HASH(r->hashFunction,key) % r->buckets.GetCount();
			register const unsigned long n = r->buckets[hash].GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (key == r->buckets[hash][i].first)
				{
					r->buckets[hash].Remove(i);
					return TRUE;
				}
			}
			return FALSE;
		}
		GPL_Boolean Replace(const K & key, const V & value)
		{
			Privatize();
			GPL_Boolean success = FALSE;
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long hash =
				GPL_HASH(r->hashFunction,key) % r->buckets.GetCount();
			register const unsigned long n = r->buckets[hash].GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (key == r->buckets[hash][i].first)
				{
					success = TRUE;
					r->buckets[hash].Remove(i);
					break;
				}
			}
			GPL_Pair<K, V> pair(key, value);
			r->buckets[hash].Insert(pair);
			return success;
		}
		GPL_Boolean Insert(const K & key, const V & value)
		{
			Privatize();
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long hash =
				GPL_HASH(r->hashFunction,key) % r->buckets.GetCount();
			register const unsigned long n = r->buckets[hash].GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				if (key == r->buckets[hash][i].first)
				{
					return FALSE;
				}
			}
			GPL_Pair<K, V> pair(key, value);
			r->buckets[hash].Insert(pair);
			return TRUE;
		}
		void Clear(void)
		{
			Privatize();
			register GPL_MapRep<K, V> *r = (GPL_MapRep<K, V> *) rep;
			register const unsigned long n = r->buckets.GetCount();
			for (register unsigned long i = 0; n > i; ++i)
			{
				r->buckets[i].Clear();
			}
		}

	protected:
		virtual void Privatize(void)
		{
			if (IsShared())
			{
				register GPL_MapRep<K, V> *newRep =
					new GPL_MapRep<K, V>(*((GPL_MapRep<K, V> *) rep));
				--rep->count;
				rep = newRep;
			}
		}
};

/******************************************************************************
WARNING: This is a convenience object.
It's state is not valid after other operations on the map.
A Reset() should be done to make it valid again.
******************************************************************************/

template <class K, class V>
class GPL_MapIterator
{
	public:
		GPL_MapIterator(void) : map((GPL_Map<K, V> *) 0), index(-1) {
		}
		virtual ~GPL_MapIterator(void)
		{
		}
		void Reset(GPL_Map<K, V> *m)
		{
			map = m;
			keys = map->GetKeys();
			index = -1;
		}
		GPL_Boolean Next(void)
		{
			return keys.GetCount() > (unsigned long) (++index);
		}

		const K & Key(void) const
		{
			return keys.GetElement(index);
		}
		V & Value(void)
		{
			return map->Find(keys.GetElement(index));
		}

	protected:
		GPL_Map<K, V> *map;
		GPL_Array<K> keys;
		long index;
};

#endif /* GPL_MAP_H */

