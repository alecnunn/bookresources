// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test4.cpp

#include "test.h"

//
// test arrays
//

template <typename T>
void TestArray(const T* ary, uint32 numItems)
{
  printf("TestArray: numItems=%u\n", numItems);

  Serializer s;
  Buffer buf;

  // write
  assert( s.BeginWriting(&buf, useTagging) );
  assert( s.PutArray(ary, numItems) );
  assert( s.EndWriting() );

  DumpBuffer(buf);

  // read
  T* ary_read = new T[numItems];
  uint32 numItems_read = numItems; // in/out
  assert( s.BeginReading(&buf, useTagging) );
  assert( s.GetArray(ary_read, numItems_read) );
  assert( s.EndReading() );

  // nothing left to read
  assert( buf.GetReadLength() == 0 );

  // we read what we wrote
  assert( numItems == numItems_read );
  for (uint32 i = 0; i < numItems; ++i)
    assert( ary[i] == ary_read[i] );

  // cleanup
  delete [] ary_read;
}

void test4()
{
  printf("\n\n*** test4() ***\n\n");

  uint32 a1[4] = { 1, 2, 3, 4 };
  TestArray(a1, 4);

  float32 a2[5] = { 0.5f, -1.5f, 33.44f, 120.3f, 100.1f };
  TestArray(a2, 5);

  float64 a3[5] = { 0.5, -1.5, 33.44, 120.3, 100.1 };
  TestArray(a3, 5);

  string a4[3];
  a4[0] = "zero";
  a4[1] = "one";
  a4[2] = "two";
  TestArray(a4, 3);
}
