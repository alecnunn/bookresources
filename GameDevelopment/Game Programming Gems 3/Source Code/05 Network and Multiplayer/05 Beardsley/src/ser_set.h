// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_set.h

#ifndef INCLUDED_SER_SET_H
#define INCLUDED_SER_SET_H

#include "serdefs.h"
#include "ser_stl.h"

#include <set>

/**
 * SerializeHelper functions for set<> and multiset<>
 **/

namespace SerializeHelper
{
  template <typename T, typename COMPARE>
  inline SerializeTag ComputeTag(const std::set<T,COMPARE>&)
  {
    return eTagSet;
  }

  template <typename T, typename COMPARE>
  inline bool PutInto(Serializer& ser, const std::set<T,COMPARE>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T, typename COMPARE>
  inline bool GetFrom(Serializer& ser, std::set<T,COMPARE>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (T*)0);
  }

  template <typename T, typename COMPARE>
  inline SerializeTag ComputeTag(const std::multiset<T,COMPARE>&)
  {
    return eTagMultiSet;
  }

  template <typename T, typename COMPARE>
  inline bool PutInto(Serializer& ser, const std::multiset<T,COMPARE>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T, typename COMPARE>
  inline bool GetFrom(Serializer& ser, std::multiset<T,COMPARE>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (T*)0);
  }
}

#endif // !INCLUDED_SER_SET_H
