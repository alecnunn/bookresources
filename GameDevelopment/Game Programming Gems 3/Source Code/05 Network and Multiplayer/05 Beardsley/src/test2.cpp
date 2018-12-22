// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// test2.cpp

#include "test.h"

//
// test Put/Get int/float/bool
//

#define TEST(typ, val, fmt) \
  { printf("Test_" #typ ": val=" fmt "\n", (typ) val); \
    typ x = (typ) val; \
    TestSerializeSingleObject(x); }

void test2()
{
  printf("\n\n*** test2() ***\n\n");

  TEST( int8, 24, "%d" );
  TEST( int8, -42, "%d" );
  TEST( uint8, 34, "%u" );
  TEST( int16, 10043, "%d" );
  TEST( int16, -3482, "%d" );
  TEST( uint16, 32422, "%u" );
  TEST( int32, 33, "%d" );
  TEST( int32, 100223, "%d" );
  TEST( uint32, 456456, "%u" );
  TEST( int64, MAKE_INT64(123456789012345), PRINTF_INT64 );
  TEST( uint64, MAKE_UINT64(123456789012345), PRINTF_UINT64 );
  TEST( float32, 1.0, "%f" );
  TEST( float32, 12345.678, "%g" );
  TEST( float64, 3.14159, "%lg" );
  TEST( float64, 1.5e170, "%lg" );
  TEST( bool, true, "%d" );
  TEST( bool, false, "%d" );
}

#undef TEST
