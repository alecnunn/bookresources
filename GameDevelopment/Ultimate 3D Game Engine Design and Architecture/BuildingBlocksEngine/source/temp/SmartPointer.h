/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_SMART_POINTERS_H_
#define _BB_SMART_POINTERS_H_

#include"BBAssert.h"


template <class T>
class SmartPointer
{
   public:
      SmartPointer() : obj(0) { }
      SmartPointer(T *object) : obj(0) { *this = object; }
      SmartPointer(const SmartPointer<T> &ptr) : obj(0) { *this = ptr; }
      ~SmartPointer() { if(obj) obj->ReleaseRef(); }

      inline void operator =(T *object)
         {
            if(obj) obj->ReleaseRef();
            obj = object;
            if(obj) obj->AddRef();
         }

      inline void operator =(const SmartPointer<T> &ptr)
         {
            if(obj) obj->ReleaseRef();
            obj = ptr.obj;
            if(obj) obj->AddRef();
         }

      inline T& operator *() const
         {
            Assert(obj != 0, "Attempt an * on NULL smart pointer.");
            return *obj;
         }

      inline T* operator ->() const
         {
            Assert(obj != 0, "Attempt an -> on NULL smart pointer.");
            return obj;
         }

      inline operator T*() const { return obj; }
      inline bool isValid() const { return (obj != 0); }
      inline bool operator !() { return !(obj); }

      inline bool operator ==(const SmartPointer<T> &ptr) const { return (obj == ptr.obj); }
      inline bool operator ==(const T *object) const { return (obj == object); }

   protected:
      T *obj;
};

#endif