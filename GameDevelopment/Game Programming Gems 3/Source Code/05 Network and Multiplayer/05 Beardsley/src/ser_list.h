// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_list.h

#ifndef INCLUDED_SER_LIST_H
#define INCLUDED_SER_LIST_H

#include "serdefs.h"
#include "ser_stl.h"

#include <list>

/**
 * SerializeHelper functions for list<>
 **/

namespace SerializeHelper
{
  template <typename T>
  inline SerializeTag ComputeTag(const std::list<T>&)
  {
    return eTagList;
  }

  template <typename T>
  inline bool PutInto(Serializer& ser, const std::list<T>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T>
  inline bool GetFrom(Serializer& ser, std::list<T>& obj)
  {
    obj.clear();
    return GetRange(ser, std::back_inserter(obj), (T*)0);
  }
}

#endif // !INCLUDED_SER_LIST_H
