// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// serializer.h

#ifndef INCLUDED_SERIALIZER_H
#define INCLUDED_SERIALIZER_H

#include "basetypes.h"
#include "serdefs.h"

#include <string>

class Buffer; // fwd

/**
 * Serializer class
 **/

class Serializer
{

public:

  Serializer();
  ~Serializer();

  bool BeginWriting(Buffer* outBuf, bool tagging = false);
  bool EndWriting();

  bool BeginReading(Buffer* inBuf, bool tagging = false);
  bool EndReading();

  /**
   * Store an object.
   **/

  template <typename T>
  bool Put(const T& obj)
  {
    SerializeTag tag = SerializeHelper::ComputeTag(obj);
    return WriteTag(tag) && SerializeHelper::PutInto(*this, obj);
  }

  /**
   * Read an object.
   **/

  template <typename T>
  bool Get(T& obj)
  {
    SerializeTag tag = SerializeHelper::ComputeTag(obj);
    return ReadAndCheckTag(tag) && SerializeHelper::GetFrom(*this, obj);
  }

  // Support for builtin types; these could be handled through the
  // above template functions, but it's undoubtedly more efficient to
  // put them here (less template parsing for the compiler).

  bool Put(int8 x);
  bool Get(int8& x);
  bool Put(uint8 x);
  bool Get(uint8& x);
  bool Put(int16 x);
  bool Get(int16& x);
  bool Put(uint16 x);
  bool Get(uint16& x);
  bool Put(int32 x);
  bool Get(int32& x);
  bool Put(uint32 x);
  bool Get(uint32& x);
  bool Put(int64 x);
  bool Get(int64& x);
  bool Put(uint64 x);
  bool Get(uint64& x);

  bool Put(float32 x);
  bool Get(float32& x);
  bool Put(float64 x);
  bool Get(float64& x);

  bool Put(bool x);
  bool Get(bool& x);

  bool Put(const std::string& x);
  bool Put(const char* x);
  bool Get(std::string& x);

  //
  // "non-standard" Put/Get methods
  //

  // raw byte array of known size
  bool PutRaw(const void* buf, uint32 len);
  bool GetRaw(void* buf, uint32 len);

  // packed count (for variable-length things)
  bool PutCount(uint32 count);
  bool GetCount(uint32& count);

  /**
   * Store an array of objects
   **/

  template <typename T>
  bool PutArray(const T* objs, uint32 numItems)
  {
    // write tag
    if (!WriteTag(eTagArray))
      return false;
    // store size
    if (!PutCount(numItems))
      return false;
    // store each element
    for (uint32 i = 0; i < numItems; ++i)
      if (!Put(objs[i]))
        return false;
    return true;
  }

  /**
   * Read an array of objects
   **/

  template <typename T>
  bool GetArray(T* objs, uint32& numInOut)
  {
    // check tag
    if (!ReadAndCheckTag(eTagArray))
      return false;
    // retrieve the size
    uint32 numItems;
    if (!GetCount(numItems))
      return false;
    // too many to read?
    if (numItems > numInOut)
      return false;
    // read each element
    for (uint32 i = 0; i < numItems; ++i)
      if (!Get(objs[i]))
        return false;
    numInOut = numItems;
    return true;
  }

private:

  // Helper functions for Put/Get on numeric types; assume that
  // byteorder swapping happens elsewhere

  template <typename T>
  inline bool WriteNumber(T x, SerializeTag tag)
  {
    return WriteTag(tag) && WriteBytes(&x, sizeof(T));
  }

  template <typename T>
  inline bool ReadNumber(T& x, SerializeTag tag)
  {
    return ReadAndCheckTag(tag) && ReadBytes(&x, sizeof(T));
  }

  // tags
  bool WriteTag(SerializeTag tag);
  bool ReadAndCheckTag(SerializeTag expectedTag);

  // low-level Buffer access
  bool WriteBytes(const void* buf, uint32 len);
  bool ReadBytes(void* buf, uint32 len);

  enum State
  {
    eStateNone,
    eStateReading,
    eStateWriting
  };

  State m_state;
  bool m_tagsEnabled;
  Buffer* m_buffer;

  // disabled - no copying
  Serializer(const Serializer&);
  void operator=(const Serializer&);

}; // end Serializer class

// inline methods

#endif // !INCLUDED_SERIALIZER_H
