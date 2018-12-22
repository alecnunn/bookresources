// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// buffer.h

#ifndef INCLUDED_BUFFER_H
#define INCLUDED_BUFFER_H

#include "basetypes.h"
#include "serdefs.h"

#include <vector>

// serialization support
class Buffer;
SERIALIZE_DEFINE_TAG(Buffer, eTagBuffer);
SERIALIZE_DECLARE_PUTGET(Buffer);

/**
 * An automatically resizable byte array, using vector<uint8> for
 * underlying storage.
 *
 * Writing to the buffer always appends to the end.
 *
 * Reading is non-destructive, and uses a "read head" which can be
 * reset back to the front.
 **/

class Buffer
{

public:

  Buffer();
  Buffer(const Buffer& other);

  ~Buffer();

  const Buffer& operator=(const Buffer& other);

  bool Write(const void* data, uint32 len);
  bool Read(void* dataOut, uint32 len);
  const void* GetData() const;
  uint32 GetLength() const;
  void Clear();
  void Reset();
  uint32 GetReadLength() const;

  void Swap(Buffer& other);

private:

  std::vector<uint8> m_buf; // buffer
  uint32 m_readOffset; // "read head" offset from m_buf[0]

  // serialization needs friend access
  SERIALIZE_DECLARE_FRIEND(Buffer);

};

// equality operator

bool operator==(const Buffer& lhs, const Buffer& rhs);

// inline methods

/**
 * Constructor
 **/

inline Buffer::Buffer()
  : m_readOffset(0)
{
}

/**
 * Copy constructor
 **/

inline Buffer::Buffer(const Buffer& other)
  : m_buf(other.m_buf),
    m_readOffset(other.m_readOffset)
{
}

/**
 * Destructor
 **/

inline Buffer::~Buffer()
{
}

/**
 * Assignment operator
 **/

inline const Buffer& Buffer::operator=(const Buffer& other)
{
  // check self-assignment
  if (this != &other)
  {
    m_buf = other.m_buf;
    m_readOffset = other.m_readOffset;
  }
  return *this;
}

/**
 * Swap contents with another buffer
 **/

inline void Buffer::Swap(Buffer& other)
{
  m_buf.swap(other.m_buf);
  uint32 tmp = m_readOffset;
  m_readOffset = other.m_readOffset;
  other.m_readOffset = tmp;
}

/**
 * Return a pointer to the beginning of the buffer, or NULL if the
 * buffer is empty.
 **/

inline const void* Buffer::GetData() const
{
  return m_buf.empty() ? 0 : &m_buf[0];
}

/**
 * Return the buffer size
 **/

inline uint32 Buffer::GetLength() const
{
  return m_buf.size();
}

/**
 * Erase buffer contents
 **/

inline void Buffer::Clear()
{
  m_buf.clear();
  m_readOffset = 0;
}

/**
 * Reset read head
 **/

inline void Buffer::Reset()
{
  m_readOffset = 0;
}

/**
 * Return bytes available for reading
 **/

inline uint32 Buffer::GetReadLength() const
{
  return m_buf.size() - m_readOffset;
}

#endif // INCLUDED_BUFFER_H
