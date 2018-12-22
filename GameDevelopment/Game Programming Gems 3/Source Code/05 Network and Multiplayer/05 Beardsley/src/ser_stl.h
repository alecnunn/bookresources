// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// ser_stl.h

#ifndef INCLUDED_SER_STL_H
#define INCLUDED_SER_STL_H

#include "serializer.h"

/**
 * SerializeHelper functions for STL containers
 **/

namespace SerializeHelper
{
  /**
   * Helper for STL PutInto functions
   **/

  template <typename Iterator>
  bool PutRange(Serializer& ser, const Iterator& begin, const Iterator& end, uint32 numItems)
  {
    // store the size
    if (!ser.PutCount(numItems))
      return false;
    // store each element
    uint32 count = 0;
    for (Iterator it = begin; it != end; ++it, ++count)
      if (!ser.Put(*it))
        return false;
    // just in case distance(begin,end) != numItems
    return (count == numItems);
  }

  /**
   * Helper for STL GetFrom functions
   **/

  template <typename T, typename Iterator>
  bool GetRange(Serializer& ser, Iterator it, const T*)
  {
    // read size
    uint32 size;
    if (!ser.GetCount(size))
      return false;
    // read each item
    for (uint32 i = 0; i < size; ++i)
    {
      T obj;
      if (!ser.Get(obj))
        return false;
      *it = obj; // insertion
    }
    return true;
  }

}

#endif // !INCLUDED_SER_STL_H
