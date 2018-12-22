/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_SINGLETON_H_
#define _BB_SINGLETON_H_

#include<cassert>


template<typename T>
class Singleton
{
   protected:
      Singleton()
         {
            assert(!mSingleton);
            int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
            mSingleton = (T*)((int)this + offset);
         }


      virtual ~Singleton()
         {
            assert(mSingleton);
            mSingleton = 0;
         }

   public:
      static T &GetSingleton()
         {
            static T sys;
            assert(mSingleton);
            return (*mSingleton);
         }


      static T *GetSingletonPtr() { return mSingleton; }


   protected:
      static T *mSingleton;
};

template <typename T> T* Singleton<T>::mSingleton = 0;

#endif