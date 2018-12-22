/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */

#ifndef _POOL_H
#define _POOL_H

// A Pool keeps a list of previously allocated objects, so that frequently
// reused objects don't have to be continually freed and reallocated.
// The class that the Pool tracks must have a no-argument constructor; it
// works best for simple structures.
//
// Objects are added to the Pool's free list when they are released; to avoid
// memory leaks, all outstanding allocated objects should be released, so
// that will ultimately be freed when the Pool is deleted.  FreeAll can
// be called anytime to reset the pool of released objects.
//
// The code for the template is in the .h file because Microsoft Visual C++
// can only instantiate template classes in the file where they're defined.
// Thus, if the code were in a .cpp file, all uses of the Pool would have to
// appear here.

#include <list>
using namespace std;

template<class T>
class Pool 
{
public:

   Pool() { allocated_count = 0; }
   ~Pool()  {  FreeAll(); }

   // Get an object from the free store if possible; allocate it if not.
   virtual T* Get()
   {
      if (store.begin() != store.end())
      {
	 T *t = *store.begin();
	 store.erase(store.begin());
	 
	 return t;
      }
      
      /* no available structures */
      allocated_count++;
      return new T;
   }

   virtual void Release(T *t) { store.push_front(t); }
   void FreeAll()
   {
      store_type::iterator it;
      
      it = store.begin();
      while (it != store.end())
      {
	 allocated_count--;
	 
	 delete *it;
	 
	 it = store.erase(it);
      }
   }

   int32 GetAvailableCount()
   {
      store_type::iterator it;
      int32 count = 0;
      
      for (it = store.begin();it != store.end();it++)
	 count++;
      
      return count;
   }

   int32 GetAllocatedCount() { return allocated_count; }
   int32 GetTotalCount() { return GetAvailableCount() + GetAllocatedCount(); }

protected:
   
   typedef list<T *> store_type;
   store_type store;
   int32 allocated_count;
};

#endif
