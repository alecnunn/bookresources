// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// serializer.cpp

#include "serializer.h"

#include "buffer.h"
#include "byteorder.h"

#include <cassert>

/**
 * Constructor
 **/

Serializer::Serializer()
  : m_state(eStateNone),
    m_tagsEnabled(false),
    m_buffer(0)
{
  // empty
}

/**
 * Destructor
 **/

Serializer::~Serializer()
{
  m_state = eStateNone;
  m_buffer = 0;
}

/**
 * Initialize for writing
 **/

bool Serializer::BeginWriting(Buffer* outBuf, bool tagging)
{
  assert(outBuf != 0);

  m_state = eStateWriting;
  m_tagsEnabled = tagging;
  m_buffer = outBuf;
  return true;
}

/**
 * Finalize writing
 **/

bool Serializer::EndWriting()
{
  m_state = eStateNone;
  m_buffer = 0;
  return true;
}

/**
 * Initialize for reading
 **/

bool Serializer::BeginReading(Buffer* inBuf, bool tagging)
{
  assert(inBuf != 0);

  m_state = eStateReading;
  m_tagsEnabled = tagging;
  m_buffer = inBuf;
  return true;
}

/**
 * Finalize reading
 **/

bool Serializer::EndReading()
{
  m_state = eStateNone;
  m_buffer = 0;
  return true;
}


/**
 * Builtin Put/Get functions
 **/

bool Serializer::Put(int8 x)
{
  return WriteNumber(x, eTagInt8);
}

bool Serializer::Get(int8& x)
{
  return ReadNumber(x, eTagInt8);
}

bool Serializer::Put(uint8 x)
{
  return WriteNumber(x, eTagUInt8);
}

bool Serializer::Get(uint8& x)
{
  return ReadNumber(x, eTagUInt8);
}

bool Serializer::Put(int16 x)
{
  x = HTON_I16(x);
  return WriteNumber(x, eTagInt16);
}

bool Serializer::Get(int16& x)
{
  bool ret = ReadNumber(x, eTagInt16);
  x = NTOH_I16(x);
  return ret;
}

bool Serializer::Put(uint16 x)
{
  x = HTON_I16(x);
  return WriteNumber(x, eTagUInt16);
}

bool Serializer::Get(uint16& x)
{
  bool ret = ReadNumber(x, eTagUInt16);
  x = NTOH_I16(x);
  return ret;
}

bool Serializer::Put(int32 x)
{
  x = HTON_I32(x);
  return WriteNumber(x, eTagInt32);
}

bool Serializer::Get(int32& x)
{
  bool ret = ReadNumber(x, eTagInt32);
  x = NTOH_I32(x);
  return ret;
}

bool Serializer::Put(uint32 x)
{
  x = HTON_I32(x);
  return WriteNumber(x, eTagUInt32);
}

bool Serializer::Get(uint32& x)
{
  bool ret = ReadNumber(x, eTagUInt32);
  x = NTOH_I32(x);
  return ret;
}

bool Serializer::Put(int64 x)
{
  x = HTON_I64(x);
  return WriteNumber(x, eTagInt64);
}

bool Serializer::Get(int64& x)
{
  bool ret = ReadNumber(x, eTagInt64);
  x = NTOH_I64(x);
  return ret;
}

bool Serializer::Put(uint64 x)
{
  x = HTON_I64(x);
  return WriteNumber(x, eTagUInt64);
}

bool Serializer::Get(uint64& x)
{
  bool ret = ReadNumber(x, eTagUInt64);
  x = NTOH_I64(x);
  return ret;
}

bool Serializer::Put(float32 x)
{
  x = HTON_F32(x);
  return WriteNumber(x, eTagFloat32);
}

bool Serializer::Get(float32& x)
{
  bool ret = ReadNumber(x, eTagFloat32);
  x = NTOH_F32(x);
  return ret;
}

bool Serializer::Put(float64 x)
{
  x = HTON_F64(x);
  return WriteNumber(x, eTagFloat64);
}

bool Serializer::Get(float64& x)
{
  bool ret = ReadNumber(x, eTagFloat64);
  x = NTOH_F64(x);
  return ret;
}

bool Serializer::Put(bool x)
{
  uint8 val = x ? 1 : 0;
  return WriteNumber(val, eTagBool);
}

bool Serializer::Get(bool& x)
{
  uint8 val;
  bool ret = ReadNumber(val, eTagBool);
  x = (val == 1);
  return ret;
}

bool Serializer::Put(const std::string& x)
{
  // tag
  if (!WriteTag(eTagString))
    return false;
  // size
  uint32 size = x.size();
  if (!PutCount(size))
    return false;
  // data (only if there is some)
  return (size == 0) || WriteBytes(x.data(), size);
}

bool Serializer::Put(const char* x)
{
  // could just return Put(string(x)), but that makes a copy..
  assert(x != 0);
  // tag
  if (!WriteTag(eTagString))
    return false;
  // size
  uint32 size = strlen(x);
  if (!PutCount(size))
    return false;
  // data (only if there is some)
  return (size == 0) || WriteBytes(x, size);
}

bool Serializer::Get(std::string& x)
{
  // erase contents
  x.erase();
  // tag
  if (!ReadAndCheckTag(eTagString))
    return false;
  // size
  uint32 size;
  if (!GetCount(size))
    return false;
  // data (only if there is some)
  if (size == 0)
    return true;
  // since we can't get a pointer to string's internal data, have to
  // use a temp buffer and copy it...
  char* tempString = new char[size];
  bool ok = ReadBytes(tempString, size);
  if (ok)
    x.assign(tempString, size);
  delete [] tempString;
  return ok;
}


/**
 * Raw byte arrays
 **/

bool Serializer::PutRaw(const void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);

  // note: no length field
  return WriteTag(eTagRawBytes) && WriteBytes(buf, len);
}

bool Serializer::GetRaw(void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);

  // note: no length field
  return ReadAndCheckTag(eTagRawBytes) && ReadBytes(buf, len);
}


/**
 * "Packed" count
 *
 * - 1-5 bytes, least significant bits first
 * - low 7 bits are data
 * - high bit is a stop bit
 **/

bool Serializer::PutCount(uint32 count)
{
  if (!WriteTag(eTagCount))
    return false;

  // encode loop
  do
  {
    // get data
    uint8 tmp = (count & 0x7F);
    // shift it off
    count = count >> 7;
    // set stop bit
    if (count == 0)
      tmp |= 0x80;
    // write
    if (!WriteBytes(&tmp, 1))
      return false;
  } while (count != 0);

  return true;
}

bool Serializer::GetCount(uint32& count)
{
  if (!ReadAndCheckTag(eTagCount))
    return false;

  // decode loop
  count = 0;
  bool stopBit = false;
  for (uint32 i = 0; (i < 5) && !stopBit; ++i)
  {
    // read
    uint8 tmp;
    if (!ReadBytes(&tmp, 1))
      return false;
    // stop bit?
    stopBit = ((tmp & 0x80) != 0);
    // mask data, shift up, or into count
    count |= ((uint32) (tmp & 0x7F) << (i * 7));
  }

  // no stop bit == bad
  return stopBit;
}


/**
 * Write a tag
 *
 * If not in 'tagging' mode, then automatically succeeds.
 **/

bool Serializer::WriteTag(const SerializeTag tag)
{
  assert((tag >= 0) && (tag <= 255)); // fit inside uint8

  uint8 tmp = tag;
  return !m_tagsEnabled || WriteBytes(&tmp, 1);
}

/**
 * Read a tag, and check it against an expected value
 *
 * If not in 'tagging' mode, then automatically succeeds.
 **/

bool Serializer::ReadAndCheckTag(SerializeTag expectedTag)
{
  uint8 tmp;
  return !m_tagsEnabled || (ReadBytes(&tmp, 1) && (tmp == expectedTag));
}


/**
 * Write to the output Buffer
 **/

bool Serializer::WriteBytes(const void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);
  assert(m_state == eStateWriting);

  return m_buffer->Write(buf, len);
}

/**
 * Read from the input Buffer
 **/

bool Serializer::ReadBytes(void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);
  assert(m_state == eStateReading);

  return m_buffer->Read(buf, len);
}
