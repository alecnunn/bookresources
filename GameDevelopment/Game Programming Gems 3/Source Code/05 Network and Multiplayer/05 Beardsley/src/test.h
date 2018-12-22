// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test.h

#ifndef INCLUDED_TEST_H
#define INCLUDED_TEST_H

#include "basetypes.h"
#include "buffer.h"
#include "serializer.h"

#include <cassert>
#include <cstdio>

using namespace std;

extern bool useTagging;

extern void DumpBuffer(const Buffer& buf);

template <typename T>
void TestSerializeSingleObject(const T& val)
{
  Serializer s;
  Buffer buf;

  // write
  assert( s.BeginWriting(&buf, useTagging) );
  assert( s.Put(val) );
  assert( s.EndWriting() );

  DumpBuffer(buf);

  // read
  T val_read;
  assert( s.BeginReading(&buf, useTagging) );
  assert( s.Get(val_read) );
  assert( s.EndReading() );

  // nothing left to read
  assert( buf.GetReadLength() == 0 );

  // we read what we wrote
  assert( val == val_read );
}

extern void test0();
extern void test1();
extern void test2();
extern void test3();
extern void test4();
extern void test5();
extern void test6();
extern void test7();
extern void test8();
extern void test9();

#endif // !INCLUDED_TEST_H
