// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test5.cpp

#include "test.h"

//
// test "raw" byte array
//

void TestRaw(const void* data, uint32 len)
{
  printf("TestRaw: len=%u\n", len);

  Serializer s;
  Buffer buf;

  // write
  assert( s.BeginWriting(&buf, useTagging) );
  assert( s.PutRaw(data, len) );
  assert( s.EndWriting() );

  DumpBuffer(buf);

  // read
  uint8* data_read = new uint8[len];
  assert( s.BeginReading(&buf, useTagging) );
  assert( s.GetRaw(data_read, len) );
  assert( s.EndReading() );

  // nothing left to read
  assert( buf.GetReadLength() == 0 );

  // we read what we wrote
  const uint8* data_tmp = (const uint8*) data;
  for (uint32 i = 0; i < len; ++i)
    assert( data_tmp[i] == data_read[i] );

  // cleanup
  delete [] data_read;
}

void test5()
{
  printf("\n\n*** test5() ***\n\n");

  uint8 a1[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  TestRaw(a1, 8);

  // not that you'd want to do this, but you could
  float64 foo = 1.234;
  TestRaw(&foo, sizeof(foo));
}
