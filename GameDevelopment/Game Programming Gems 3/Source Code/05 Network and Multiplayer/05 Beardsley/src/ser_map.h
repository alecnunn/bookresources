// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_map.h

#ifndef INCLUDED_SER_MAP_H
#define INCLUDED_SER_MAP_H

#include "serdefs.h"
#include "ser_pair.h"
#include "ser_stl.h"

#include <map>

/**
 * SerializeHelper functions for map<> and multimap<>
 **/

namespace SerializeHelper
{
  template <typename KEY, typename VALUE, typename COMPARE>
  inline SerializeTag ComputeTag(const std::map<KEY,VALUE,COMPARE>&)
  {
    return eTagMap;
  }

  template <typename KEY, typename VALUE, typename COMPARE>
  inline bool PutInto(Serializer& ser, const std::map<KEY,VALUE,COMPARE>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename KEY, typename VALUE, typename COMPARE>
  inline bool GetFrom(Serializer& ser, std::map<KEY,VALUE,COMPARE>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (std::pair<KEY,VALUE>*)0);
  }

  template <typename KEY, typename VALUE, typename COMPARE>
  inline SerializeTag ComputeTag(const std::multimap<KEY,VALUE,COMPARE>&)
  {
    return eTagMultiMap;
  }

  template <typename KEY, typename VALUE, typename COMPARE>
  inline bool PutInto(Serializer& ser, const std::multimap<KEY,VALUE,COMPARE>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename KEY, typename VALUE, typename COMPARE>
  inline bool GetFrom(Serializer& ser, std::multimap<KEY,VALUE,COMPARE>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (std::pair<KEY,VALUE>*)0);
  }
}

#endif // !INCLUDED_SER_MAP_H
