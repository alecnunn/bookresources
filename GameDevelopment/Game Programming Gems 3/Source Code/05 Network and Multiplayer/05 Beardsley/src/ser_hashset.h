// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_hashset.h

#ifndef INCLUDED_SER_HASHSET_H
#define INCLUDED_SER_HASHSET_H

#include "serdefs.h"
#include "ser_stl.h"

// sheesh, who was it that decided header files without ".h"
// was a good idea anyway?

#if defined(COMPILER_MSVC)
# include <hash_set>
#else
# include <hash_set.h>
#endif

/**
 * SerializeHelper functions for hash_set<> and hash_multiset<>
 **/

namespace SerializeHelper
{
  template <typename T, typename HASH, typename EQUAL>
  inline SerializeTag ComputeTag(const std::hash_set<T,HASH,EQUAL>&)
  {
    return eTagHashSet;
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool PutInto(Serializer& ser, const std::hash_set<T,HASH,EQUAL>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool GetFrom(Serializer& ser, std::hash_set<T,HASH,EQUAL>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (T*)0);
  }

  template <typename T, typename HASH, typename EQUAL>
  inline SerializeTag ComputeTag(const std::hash_multiset<T,HASH,EQUAL>&)
  {
    return eTagHashMultiSet;
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool PutInto(Serializer& ser, const std::hash_multiset<T,HASH,EQUAL>& obj)
  {
    return PutRange(ser, obj.begin(), obj.end(), obj.size());
  }

  template <typename T, typename HASH, typename EQUAL>
  inline bool GetFrom(Serializer& ser, std::hash_multiset<T,HASH,EQUAL>& obj)
  {
    obj.clear();
    return GetRange(ser, std::inserter(obj, obj.end()), (T*)0);
  }
}

#endif // !INCLUDED_SER_HASHSET_H
