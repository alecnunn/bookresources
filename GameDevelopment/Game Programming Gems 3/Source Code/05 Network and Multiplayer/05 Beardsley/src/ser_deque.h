// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_deque.h

#ifndef INCLUDED_SER_DEQUE_H
#define INCLUDED_SER_DEQUE_H

#include "serdefs.h"
#include "ser_stl.h"

#include <deque>

/**
 * SerializeHelper functions for deque<>
 **/

namespace SerializeHelper
{
  template <typename T>
  inline SerializeTag ComputeTag(const std::deque<T>&)
  {
    return eTagDeque;
  }

  template <typename T>
  inline bool PutInto(Serializer& ser, const std::deque<T>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T>
  inline bool GetFrom(Serializer& ser, std::deque<T>& obj)
  {
    obj.clear();
    return GetRange(ser, std::back_inserter(obj), (T*)0);
  }
}

#endif // !INCLUDED_SER_DEQUE_H
