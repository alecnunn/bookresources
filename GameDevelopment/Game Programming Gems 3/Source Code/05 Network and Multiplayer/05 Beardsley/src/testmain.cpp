// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// testmain.cpp

#include "test.h"

// use tagging or not (see main())
bool useTagging = false;

//************************************************************

//
// utility functions
//

void DumpBuffer(const Buffer& buf)
{
  uint32 len = buf.GetLength();
  printf("Buffer: size=%u", len);
  if (len > 0)
  {
    printf("  [ ");
    const uint8* pBuf = (const uint8*) buf.GetData();
    for (uint32 i = 0; i < len; ++i)
      printf("%02x ", pBuf[i]);
    printf("]");
  }
  printf("\n");
}

//************************************************************

//
// main()
//

int main(int argc, const char** /* argv */)
{
  // an arg == use tagging
  if (argc > 1)
    useTagging = true;

  // tests to run
  test0();
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();

  return 0;
}
