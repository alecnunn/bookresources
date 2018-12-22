///////////////////////////////////////////////////////////////////////////////
//
// Fib.cpp
//
// Demonstrates template metaprogramming for Fibonacci sequence
//
// Copyright © 2000 Pete Isensee (PKIsensee@msn.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#else
#include <stdio.h>
#endif
#include "fib.h"

#ifndef WIN32
#define __cdecl
#endif

#ifdef WIN32
#define cassert(exp) (void)( (exp) || ( MessageBox( NULL, #exp, "Assert", 0 ) ) )
#else
#define cassert(exp) (void)( (exp) || ( fprintf(stderr, "Assert: %s ", #exp ) ) )
#endif

namespace
{

///////////////////////////////////////////////////////////////////////////////
//
// Recursive Fibonacci

unsigned RecursiveFib( unsigned n )
{
   if( n <= 1 )
      return n;
   return RecursiveFib( n-1 ) + RecursiveFib( n-2 );
}

///////////////////////////////////////////////////////////////////////////////
//
// Nonrecursive Fibonacci

unsigned NonRecursiveFib( unsigned n )
{
   int iPrev = -1;
   int iResult = 1;
   for( unsigned i = 0; i <= n; ++i )
   {
      int iSum = iResult + iPrev;
      iPrev = iResult;
      iResult = iSum;
   }
   return( unsigned( iResult ) );
}

} // end of local namespace

///////////////////////////////////////////////////////////////////////////////
//
// Test function

int __cdecl main( int, char* [] )
{
   // Test recursive vs nonrecursive
   for( unsigned i = 0; i < 10; ++i )
   {
      cassert( RecursiveFib( i ) == NonRecursiveFib( i ) );
   }

   // Test template version
   cassert( FibT( 0 ) == NonRecursiveFib( 0 ) );
   cassert( FibT( 1 ) == NonRecursiveFib( 1 ) );
   cassert( FibT( 2 ) == NonRecursiveFib( 2 ) );
   cassert( FibT( 3 ) == NonRecursiveFib( 3 ) );
   cassert( FibT( 4 ) == NonRecursiveFib( 4 ) );
   cassert( FibT( 5 ) == NonRecursiveFib( 5 ) );
   cassert( FibT( 6 ) == NonRecursiveFib( 6 ) );
   cassert( FibT( 7 ) == NonRecursiveFib( 7 ) );
   cassert( FibT( 8 ) == NonRecursiveFib( 8 ) );
   cassert( FibT( 9 ) == NonRecursiveFib( 9 ) );

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
