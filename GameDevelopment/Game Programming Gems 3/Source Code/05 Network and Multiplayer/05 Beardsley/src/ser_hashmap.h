// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_hashmap.h

#ifndef INCLUDED_SER_HASHMAP_H
#define INCLUDED_SER_HASHMAP_H

#include "serdefs.h"
#include "ser_pair.h"
#include "ser_stl.h"

#if defined(COMPILER_MSVC)
# include <hash_map>
#else
# include <hash_map.h>
#endif

/**
 * SerializeHelper functions for hash_map<> and hash_multimap<>
 **/

namespace SerializeHelper
{
  template <typename KEY, typename VALUE, typename HASH, typename EQUAL>
  inline SerializeTag ComputeTag(const std::hash_map<KEY,VALUE,HASH,EQUAL>&)
  {
    return eTagHashMap;
  }

  template <typename KEY, typename VALUE, typename HASH, typename EQUAL>
  inline bool PutInto(Serializer& ser, const std::hash_map<KEY,VALUE,HASH,EQUAL>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename KEY, typename VALUE, typename HASH, typename EQUAL>
  inline bool GetFrom(Serializer& ser, std::hash_map<KEY,VALUE,HASH,EQUAL>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (std::pair<KEY,VALUE>*)0);
  }

  template <typename KEY, typename VALUE, typename HASH, typename EQUAL>
  inline SerializeTag ComputeTag(const std::hash_multimap<KEY,VALUE,HASH,EQUAL>&)
  {
    return eTagHashMultiMap;
  }

  template <typename KEY, typename VALUE, typename HASH, typename EQUAL>
  inline bool PutInto(Serializer& ser, const std::hash_multimap<KEY,VALUE,HASH,EQUAL>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename KEY, typename VALUE, typename HASH, typename EQUAL>
  inline bool GetFrom(Serializer& ser, std::hash_multimap<KEY,VALUE,HASH,EQUAL>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (std::pair<KEY,VALUE>*)0);
  }
}

#endif // !INCLUDED_SER_HASHMAP_H
