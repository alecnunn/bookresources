// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_vector.h

#ifndef INCLUDED_SER_VECTOR_H
#define INCLUDED_SER_VECTOR_H

#include "serdefs.h"
#include "ser_stl.h"

#include <vector>

/**
 * SerializeHelper functions for vector<>
 **/

namespace SerializeHelper
{
  template <typename T>
  inline SerializeTag ComputeTag(const std::vector<T>&)
  {
    return eTagVector;
  }

  template <typename T>
  inline bool PutInto(Serializer& ser, const std::vector<T>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T>
  inline bool GetFrom(Serializer& ser, std::vector<T>& obj)
  {
    obj.clear();
    return GetRange(ser, std::back_inserter(obj), (T*)0);
  }
}

#endif // !INCLUDED_SER_VECTOR_H
