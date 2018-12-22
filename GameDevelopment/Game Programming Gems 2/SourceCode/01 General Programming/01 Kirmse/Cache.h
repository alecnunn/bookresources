/* Copyright (C) Andrew Kirmse, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2001"
 */
#ifndef _CACHE_H
#define _CACHE_H

#include <vector>
#include <assert.h>

/***********************************************************************
 **                                                                   **
 ** TCache                                                            **
 **                                                                   **
 ** Objects can be released to the cache in arbitrary order.          **
 **                                                                   **
 ***********************************************************************/

template <class T>
class TCache
{
public:

   TCache(unsigned int MaxCacheSize)
      : mMaxCacheSize(MaxCacheSize)
   {
      mCache.reserve(MaxCacheSize);
   }

   ~TCache()
   {
      Clear();
   }


   // Returns NULL if no objects available in cache
   T* Retrieve()
   {
      T* pItem = NULL;

      if (false == mCache.empty())
      {
         pItem = mCache.back();
         mCache.pop_back();
      }

      return pItem;
   }

   // Creates a new object on the help if no objects
   // available in cache
   T* RetrieveOrCreate()
   {
      T* pItem = NULL;

      if (false == mCache.empty())
      {
         pItem = mCache.back();
         mCache.pop_back();
      }
      else
         pItem = new T;

      return pItem;
   }

   void Release(T* pItem)
   {
      // If cache isn't full, return object to cache
      if (mMaxCacheSize > mCache.size() &&
          NULL != pItem)
      {
         mCache.push_back(pItem);
      }
      else
         delete pItem;
   }

   void Clear()
   {
      while (false == mCache.empty())
      {
         // Pop him off
         T* pItem = mCache.back();
         mCache.pop_back();

         // Release
         delete pItem;
      }
   }

   unsigned int GetMaxCacheSize() const
   {
      return mMaxCacheSize;
   }

   unsigned int GetCurrentCacheSize() const
   {
      return mCache.size();
   }

private:

   // Private data
   unsigned int     mMaxCacheSize;
   std::vector<T*>  mCache;
};


/***********************************************************************
 **                                                                   **
 ** TSimpleCache                                                      **
 **                                                                   **
 ** Acts as a stack: you can only release the most recently retrieved **
 ** item, or clear the entire collection.  Holds on to objects        **
 ** (not pointers) and does no allocation after the initial block.    **
 **                                                                   **
 ***********************************************************************/

template<class T>
class TSimpleCache
{
public:
   
   TSimpleCache(unsigned int size)
   {
      mSize    = size;
      mNumUsed = 0;
      mpCache  = new T[size];
   }

   ~TSimpleCache()
   {
      delete mpCache;
   }

   T *Retrieve()
   {
      if (mNumUsed >= mSize)
         return NULL;

      return &mpCache[mNumUsed++];
   }

   void Release(T *pItem)
   {
      // You can only release the last one (we're a stack)
      assert(mNumUsed > 0 && pItem == &mpCache[mNumUsed - 1]);

      mNumUsed--;
   }
   
   void Clear()
   {
      mNumUsed = 0;
   }

private:

   T*           mpCache;
   unsigned int mNumUsed;
   unsigned int mSize;
};


#endif
