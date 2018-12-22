// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// buffer.cpp

#include "buffer.h"
#include "serializer.h"

#include <cassert>
#include <cstring>

/**
 * Equality operator
 **/

bool operator==(const Buffer& lhs, const Buffer& rhs)
{
  return (lhs.GetLength() == rhs.GetLength()) &&
    (memcmp(lhs.GetData(), rhs.GetData(), lhs.GetLength()) == 0);
}

/**
 * Append data to the end of the buffer
 **/

bool Buffer::Write(const void* data, uint32 len)
{
  assert(data != 0);
  assert(len > 0);

  // resize to new length
  uint32 oldSize = m_buf.size();
  m_buf.resize(oldSize + len);
  // copy directly into the vector's memory
  memcpy(&m_buf[oldSize], data, len);
  return true;
}

/**
 * Read bytes from the current read position
 **/

bool Buffer::Read(void* dataOut, uint32 len)
{
  assert(dataOut != 0);
  assert(len > 0);

  // not enough to read?
  if (len > (m_buf.size() - m_readOffset))
    return false;
  // copy, advance read head
  memcpy(dataOut, &m_buf[m_readOffset], len);
  m_readOffset += len;
  return true;
}

/**
 * Serialization
 **/

SERIALIZE_DEFINE_PUT(Buffer, obj, serializer)
{
  // store size and bytes, but not read offset
  uint32 len = obj.GetLength();
  return serializer.PutCount(len) &&
         ((len == 0) || serializer.PutRaw(obj.GetData(), len));
}

SERIALIZE_DEFINE_GET(Buffer, obj, serializer)
{
  // clear existing contents
  obj.Clear();
  // read size
  uint32 len;
  if (!serializer.GetCount(len))
    return false;
  if (len == 0)
    return true;
  // resize and read data
  obj.m_buf.resize(len);
  return serializer.GetRaw(&obj.m_buf[0], len);
}
