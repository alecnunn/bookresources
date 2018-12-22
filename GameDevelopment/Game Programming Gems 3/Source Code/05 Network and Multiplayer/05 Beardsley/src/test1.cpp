// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test1.cpp

#include "test.h"

//
// test Put/Get count
//

void TestCount(uint32 c)
{
  Serializer s;
  Buffer buf;

  printf("TestCount: c=%u\n", c);

  // write
  assert( s.BeginWriting(&buf, useTagging) );
  assert( s.PutCount(c) );
  assert( s.EndWriting() );

  DumpBuffer(buf);

  // read
  uint32 c_read;
  assert( s.BeginReading(&buf, useTagging) );
  assert( s.GetCount(c_read) );
  assert( s.EndReading() );

  // nothing left to read
  assert( buf.GetReadLength() == 0 );

  // we read what we wrote
  assert( c == c_read );
}

void test1()
{
  printf("\n\n*** test1() ***\n\n");

  TestCount( 0u );
  TestCount( 1u );
  TestCount( 127u ); // 2^7-1: 1 byte
  TestCount( 128u ); // 2^7: 2 bytes
  TestCount( 129u );
  TestCount( 255u );
  TestCount( 16383u ); // 2^14-1: 2 bytes
  TestCount( 16384u ); // 2^14: 3 bytes
  TestCount( 16385u );
  TestCount( 32767u );
  TestCount( 32768u );
  TestCount( 2097151u ); // 2^21-1: 3 bytes
  TestCount( 2097152u ); // 2^21: 4 bytes
  TestCount( 2097153u );
  TestCount( 268435455u ); // 2^28-1: 4 bytes
  TestCount( 268435456u ); // 2^28: 5 bytes
  TestCount( 268435457u );
  TestCount( 4294967294u );
  TestCount( 4294967295u ); // 2^32-1: max
}
