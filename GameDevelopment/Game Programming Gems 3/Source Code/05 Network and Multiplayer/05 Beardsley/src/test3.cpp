// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test3.cpp

#include "test.h"

//
// test Put/Get strings
//

void TestString(const string& str)
{
  printf("TestString: '%s'\n", str.c_str());
  TestSerializeSingleObject(str);
}

void TestCharPtr(const char* str)
{
  printf("TestCharPtr: '%s'\n", str);

  Serializer s;
  Buffer buf;

  // write
  assert( s.BeginWriting(&buf, useTagging) );
  assert( s.Put(str) );
  assert( s.EndWriting() );

  DumpBuffer(buf);

  // read (into C++ string)
  string str_read;
  assert( s.BeginReading(&buf, useTagging) );
  assert( s.Get(str_read) );
  assert( s.EndReading() );

  // nothing left to read
  assert( buf.GetReadLength() == 0 );

  // we read what we wrote
  assert( string(str) == str_read );
}

void test3()
{
  printf("\n\n*** test3() ***\n\n");

  TestString(string("testing"));
  TestString(string("testing again"));
  TestCharPtr("testing char*");
  TestCharPtr("testing char* again");
}
