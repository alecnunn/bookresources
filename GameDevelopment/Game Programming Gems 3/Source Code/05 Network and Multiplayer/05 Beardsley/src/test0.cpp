// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test0.cpp

#include "test.h"

//
// test Buffer
//

template <typename T>
void TestBufferSingle(const T& obj)
{
  Buffer buf;

  printf("TestBufferSingle: size=%u\n", sizeof(T));

  // test writing and reading a single thing
  assert( buf.Write(&obj, sizeof(T)) );
  assert( buf.GetLength() == sizeof(T) );
  assert( buf.GetReadLength() == buf.GetLength() );
  T obj2;
  assert( buf.Read(&obj2, sizeof(T)) );
  assert( buf.GetLength() == sizeof(T) );
  assert( buf.GetReadLength() == 0 );
  assert( obj == obj2 );
}

void test0()
{
  printf("\n\n*** test0() ***\n\n");

  { uint8 tmp = 100; TestBufferSingle(tmp); }
  { uint16 tmp = 1000; TestBufferSingle(tmp); }
  { uint32 tmp = 100000; TestBufferSingle(tmp); }
  { uint64 tmp = MAKE_UINT64(1000000000000); TestBufferSingle(tmp); }
}
