// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test8.cpp

#include "test.h"
#include "ser_list.h"
#include "ser_set.h"
#include "keyedserializer.h"

//
// test KeyedSerializer
//

void test8()
{
  printf("\n\n*** test8() ***\n\n");

  {
    // put some items in
    KeyedSerializer<uint16> ks;
    ks.SetTagging(useTagging);
    assert( ks.Put(3, string("testing")) );
    assert( ks.Put(4, (uint32) 342) );
    list<float32> l;
    l.push_back(1.0);
    l.push_back(2.0);
    assert( ks.Put(5, l) );

    // check that it serializes
    TestSerializeSingleObject(ks);

    // now get stuff out
    uint32 ui32;
    assert( ks.Get(4, ui32) );
    assert( ui32 == 342 );
    list<float32> lf;
    assert( ks.Get(5, lf) );
    assert( l == lf );
    string str;
    assert( ks.Get(3, str) );
    assert( str == "testing" );

    // get something again
    ui32 = 1;
    assert( ks.Get(4, ui32) );
    assert( ui32 == 342 );
  }

  {
    // put some items in
    KeyedSerializer<string> ks;
    ks.SetTagging(useTagging);
    assert( ks.Put("foo", string("bar")) );
    assert( ks.Put("baz", string("quux")) );
    assert( ks.Put("x", (uint32) 1) );
    set<float32> s;
    s.insert(1.0f);
    s.insert(3.4f);
    assert( ks.Put("zz", s) );

    // check that it serializes
    TestSerializeSingleObject(ks);

    // now get stuff out
    uint32 ui32;
    assert( ks.Get("x", ui32) );
    assert( ui32 == 1 );
    string str;
    assert( ks.Get("baz", str) );
    assert( str == "quux" );
    assert( ks.Get("foo", str) );
    assert( str == "bar" );
    set<float32> sf;
    assert( ks.Get("zz", sf) );
    assert( sf == s );

    // get something again
    str = "x";
    assert( ks.Get("foo", str) );
    assert( str == "bar" );
  }
}
