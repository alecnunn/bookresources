// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test7.cpp

#include "test.h"

//
// test Buffer serialization
//

void test7()
{
  printf("\n\n*** test7() ***\n\n");

  // serialize some stuff into a Buffer
  Buffer buf;
  Serializer s;
  assert( s.BeginWriting(&buf, useTagging) );
  assert( s.Put("hello, world") );
  assert( s.Put((uint32) 42) );
  assert( s.PutCount(1565) );
  assert( s.EndWriting() );

  // serialize that buffer
  TestSerializeSingleObject(buf);
}
