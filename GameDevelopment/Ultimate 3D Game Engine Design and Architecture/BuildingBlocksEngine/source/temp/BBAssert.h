/*

*/


#ifndef _BB_ENGINE_ASSERT_H_
#define _BB_ENGINE_ASSERT_H_


#include<assert.h>


#if defined(_DEBUG)
   #define Assert(a, b) assert(a && b)
#else
   #define Assert(a, b)
#endif

#endif