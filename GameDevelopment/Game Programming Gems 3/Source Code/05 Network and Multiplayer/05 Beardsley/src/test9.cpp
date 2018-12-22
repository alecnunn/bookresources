// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test9.cpp

#include "test.h"

#include "ser_pair.h"
#include "ser_list.h"
#include "ser_vector.h"
#include "ser_deque.h"
#include "ser_map.h"
#include "ser_set.h"
#include "ser_hashset.h"
#include "ser_hashmap.h"

//
// test a user-defined class
//

class Foo; // fwd
SERIALIZE_DEFINE_TAG(Foo, eTagUserClass);
SERIALIZE_DECLARE_PUTGET(Foo);

class Foo
{

public:

  Foo() { }
  ~Foo() { }

  void Init(uint32 x, const string& s, list<float32> lf)
  {
    m_x = x;
    m_s = s;
    m_lf = lf;
  }

  bool Equals(const Foo& other) const
  {
    return (m_x == other.m_x) && (m_s == other.m_s) && (m_lf == other.m_lf);
  }

private:

  uint32 m_x;
  string m_s;
  list<float32> m_lf;

  SERIALIZE_DECLARE_FRIEND(Foo);

};

bool operator==(const Foo& lhs, const Foo& rhs)
{
  return lhs.Equals(rhs);
}

bool operator!=(const Foo& lhs, const Foo& rhs)
{
  return !lhs.Equals(rhs);
}

SERIALIZE_DEFINE_PUT(Foo, obj, serializer)
{
  return
    serializer.Put(obj.m_x) &&
    serializer.Put(obj.m_s) &&
    serializer.Put(obj.m_lf);
}

SERIALIZE_DEFINE_GET(Foo, obj, serializer)
{
  return
    serializer.Get(obj.m_x) &&
    serializer.Get(obj.m_s) &&
    serializer.Get(obj.m_lf);
}

void test9()
{
  printf("\n\n*** test9() ***\n\n");

  Foo f;
  list<float32> l;
  l.push_back(4.5f);
  l.push_back(56.65f);
  f.Init(1, "hi", l);
  TestSerializeSingleObject(f);

  vector<Foo> vf;
  vf.push_back(f);
  TestSerializeSingleObject(vf);
  l.push_back(890.123f);
  f.Init(2, "there", l);
  vf.push_back(f);
  TestSerializeSingleObject(vf);
  l.clear();
  f.Init(3, "asdf", l);
  vf.push_back(f);
  TestSerializeSingleObject(vf);
}
