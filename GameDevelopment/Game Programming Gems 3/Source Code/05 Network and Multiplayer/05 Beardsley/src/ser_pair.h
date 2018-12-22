// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_pair.h

#ifndef INCLUDED_SER_PAIR_H
#define INCLUDED_SER_PAIR_H

#include "serdefs.h"
#include "serializer.h"

#include <utility>

/**
 * SerializeHelper functions for pair<T1,T2>
 **/

namespace SerializeHelper
{
  template <typename T1, typename T2>
  inline SerializeTag ComputeTag(const std::pair<T1,T2>&)
  {
    return eTagPair;
  }

  template <typename T1, typename T2>
  inline bool PutInto(Serializer& ser, const std::pair<T1,T2>& obj)
  {
    return ser.Put(obj.first) && ser.Put(obj.second);
  }

  template <typename T1, typename T2>
  inline bool GetFrom(Serializer& ser, std::pair<T1,T2>& obj)
  {
    return ser.Get(obj.first) && ser.Get(obj.second);
  }
};

#endif // !INCLUDED_SER_PAIR_H
