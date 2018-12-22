// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// keyedserializer.h

#ifndef INCLUDED_KEYEDSERIALIZER_H
#define INCLUDED_KEYEDSERIALIZER_H

#include "buffer.h"
#include "serdefs.h"
#include "serializer.h"
#include "ser_map.h"

#include <map>
#include <vector>

/**
 * Utility class that resembles a Python dictionary.
 *
 * KEY must be able to be a key type in an STL map.
 **/

template <typename KEY>
class KeyedSerializer
{

public:

  KeyedSerializer();
  ~KeyedSerializer();

  // default copy/assignment functions are fine

  /**
   * Put the given object into the map, using the given key.  Any
   * existing object with this key is overwritten.
   **/

  template <typename T>
  bool Put(const KEY& key, const T& obj)
  {
    // serialize object into a buffer
    Buffer buf;
    Serializer ser;
    if (!ser.BeginWriting(&buf, m_useTagging) ||
        !ser.Put(obj) ||
        !ser.EndWriting())
      return false;
    // add to the map (avoid copying by inserting an empty Buffer
    // first, and using Swap())
    m_objects[key] = Buffer();
    m_objects[key].Swap(buf);
    return true;
  }

  /**
   * Get the given object from the map, using the key.  The same
   * object may be read multiple times - its data is not "consumed" by
   * this function.
   **/

  template <typename T>
  bool Get(const KEY& key, T& obj) const
  {
    // get buffer
    BufferMap::const_iterator it = m_objects.find(key);
    if (it == m_objects.end())
      return false;
    // have to make a copy, because it will be const
    Buffer buf(it->second);
    // unserialize
    Serializer ser;
    return ser.BeginReading(&buf, m_useTagging) && ser.Get(obj) && ser.EndReading();
  }

  bool HasKey(const KEY& key) const;
  void GetKeys(std::vector<KEY>& outKeys) const;
  void Erase(const KEY& key);
  void Clear();

  // should internal serialization use tags?
  void SetTagging(bool useTagging);

  // serialization helpers - since friendship and template classes
  // don't really work well together, we can't make the
  // SerializeHelper functions friends directly.
  bool PutInto(Serializer& ser) const;
  bool GetFrom(Serializer& ser);

  bool Equals(const KeyedSerializer& other) const;

private:

  typedef std::map<KEY, Buffer> BufferMap;

  bool m_useTagging;
  BufferMap m_objects;

}; // end KeyedSerializer class

// template methods

/**
 * Constructor
 **/

template <typename KEY>
inline KeyedSerializer<KEY>::KeyedSerializer()
  : m_useTagging(false)
{
  // empty
}

/**
 * Destructor
 **/

template <typename KEY>
inline KeyedSerializer<KEY>::~KeyedSerializer()
{
  // empty
}

/**
 * Return true if the given key is in the map
 **/

template <typename KEY>
bool KeyedSerializer<KEY>::HasKey(const KEY& key) const
{
  return m_objects.find(key) != m_objects.end();
}

/**
 * Retrieve a vector of all keys
 **/

template <typename KEY>
void KeyedSerializer<KEY>::GetKeys(std::vector<KEY>& outKeys) const
{
  BufferMap::const_iterator it;
  for (it = m_objects.begin(); it != m_objects.end(); ++it)
    outKeys.push_back(it->first);
}

/**
 * Remove a key from the map
 **/

template <typename KEY>
void KeyedSerializer<KEY>::Erase(const KEY& key)
{
  m_objects.erase(key);
}

/**
 * Erase object contents
 **/

template <typename KEY>
void KeyedSerializer<KEY>::Clear()
{
  m_objects.clear();
}

/**
 * Turn internal tagging on/off
 **/

template <typename KEY>
void KeyedSerializer<KEY>::SetTagging(bool useTagging)
{
  m_useTagging = useTagging;
}

/**
 * Return true if objects are equal
 **/

template <typename KEY>
bool KeyedSerializer<KEY>::Equals(const KeyedSerializer<KEY>& other) const
{
  return (m_useTagging == other.m_useTagging) && (m_objects == other.m_objects);
}

/**
 * Store object in serializer
 **/

template <typename KEY>
bool KeyedSerializer<KEY>::PutInto(Serializer& ser) const
{
  return ser.Put(m_useTagging) && ser.Put(m_objects);
}

/**
 * Load object from serializer
 **/

template <typename KEY>
bool KeyedSerializer<KEY>::GetFrom(Serializer& ser)
{
  m_objects.clear();
  return ser.Get(m_useTagging) && ser.Get(m_objects);
}

/**
 * Equality operators
 **/

template <typename KEY>
bool operator==(const KeyedSerializer<KEY>& lhs,
                const KeyedSerializer<KEY>& rhs)
{
  return lhs.Equals(rhs);
}

template <typename KEY>
bool operator!=(const KeyedSerializer<KEY>& lhs,
                const KeyedSerializer<KEY>& rhs)
{
  return !lhs.Equals(rhs);
}

/**
 * SerializeHelper functions for KeyedSerializer<>
 **/

namespace SerializeHelper
{
  template <typename KEY>
  inline SerializeTag ComputeTag(const KeyedSerializer<KEY>&)
  {
    return eTagKeyedSerializer;
  }

  template <typename KEY>
  inline bool PutInto(Serializer& ser, const KeyedSerializer<KEY>& obj)
  {
    return obj.PutInto(ser);
  }

  template <typename KEY>
  inline bool GetFrom(Serializer& ser, KeyedSerializer<KEY>& obj)
  {
    return obj.GetFrom(ser);
  }
}

#endif // !INCLUDED_NAMEDSERIALIZER_H
