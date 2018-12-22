// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test6.cpp

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
// test STL
//

void test6()
{
  printf("\n\n*** test6() ***\n\n");

  // pair
  {
    printf("pair:\n");
    pair<uint32, string> obj;
    obj.first = 100;
    obj.second = "foobar";
    TestSerializeSingleObject(obj);
  }

  // list
  {
    printf("list:\n");
    list<int16> obj;
    TestSerializeSingleObject(obj);
    obj.push_back(1);
    TestSerializeSingleObject(obj);
    obj.push_back(34);
    TestSerializeSingleObject(obj);
    obj.push_back(42);
    TestSerializeSingleObject(obj);
  }

  // vector
  {
    printf("vector:\n");
    vector<float32> obj;
    TestSerializeSingleObject(obj);
    obj.push_back(1.0f);
    TestSerializeSingleObject(obj);
    obj.push_back(3.14159f);
    TestSerializeSingleObject(obj);
    obj.push_back(66.88f);
    TestSerializeSingleObject(obj);
  }

  // deque
  {
    printf("deque:\n");
    deque<string> obj;
    TestSerializeSingleObject(obj);
    obj.push_back("one");
    TestSerializeSingleObject(obj);
    obj.push_back("two");
    TestSerializeSingleObject(obj);
  }

  // set
  {
    printf("set:\n");
    set<int64> obj;
    TestSerializeSingleObject(obj);
    obj.insert(10023);
    TestSerializeSingleObject(obj);
    obj.insert(348238424);
    TestSerializeSingleObject(obj);
    obj.insert(MAKE_INT64(3482384823482823));
    TestSerializeSingleObject(obj);
  }

  // multiset
  {
    printf("multiset:\n");
    multiset<int64> obj;
    TestSerializeSingleObject(obj);
    obj.insert(10023);
    TestSerializeSingleObject(obj);
    obj.insert(348238424);
    TestSerializeSingleObject(obj);
    obj.insert(10023);
    TestSerializeSingleObject(obj);
  }

  // map
  {
    printf("map:\n");
    map<int8,float64> obj;
    TestSerializeSingleObject(obj);
    obj[1] = 10034.32;
    TestSerializeSingleObject(obj);
    obj[6] = 384234.28;
    TestSerializeSingleObject(obj);
  }

  // multimap
  {
    printf("multimap:\n");
    multimap<uint8,float32> obj;
    typedef multimap<uint8,float32>::value_type valtype;
    TestSerializeSingleObject(obj);
    obj.insert( valtype(1, 10034.24f) );
    TestSerializeSingleObject(obj);
    obj.insert( valtype(2, 348.4234f) );
    TestSerializeSingleObject(obj);
    obj.insert( valtype(1, 98234.3471f) );
    TestSerializeSingleObject(obj);
    obj.insert( valtype(2, 10034.24f) );
    TestSerializeSingleObject(obj);
  }

  // hash_set
  {
    printf("hash_set:\n");
    hash_set<int32> obj;
    TestSerializeSingleObject(obj);
    obj.insert(10023);
    TestSerializeSingleObject(obj);
    obj.insert(348238424);
    TestSerializeSingleObject(obj);
    obj.insert(82823);
    TestSerializeSingleObject(obj);
  }

  // hash_multiset
  {
    printf("hash_multiset:\n");
    hash_multiset<int32> obj;
    TestSerializeSingleObject(obj);
    obj.insert(10023);
    TestSerializeSingleObject(obj);
    obj.insert(348238424);
    TestSerializeSingleObject(obj);
    obj.insert(10023);
    TestSerializeSingleObject(obj);
  }

  // hash_map
  {
    printf("hash_map:\n");
    hash_map<int8,float64> obj;
    TestSerializeSingleObject(obj);
    obj[1] = 10034.32;
    TestSerializeSingleObject(obj);
    obj[6] = 384234.28;
    TestSerializeSingleObject(obj);
  }

  // hash_multimap
  {
    printf("hash_multimap:\n");
    hash_multimap<uint8,float32> obj;
    typedef hash_multimap<uint8,float32>::value_type valtype;
    TestSerializeSingleObject(obj);
    obj.insert( valtype(1, 10034.24f) );
    TestSerializeSingleObject(obj);
    obj.insert( valtype(2, 348.4234f) );
    TestSerializeSingleObject(obj);
    obj.insert( valtype(1, 98234.3471f) );
    TestSerializeSingleObject(obj);
    obj.insert( valtype(2, 10034.24f) );
    TestSerializeSingleObject(obj);
  }
}
