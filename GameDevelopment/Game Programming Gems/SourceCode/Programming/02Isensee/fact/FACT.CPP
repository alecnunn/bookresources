///////////////////////////////////////////////////////////////////////////////
//
// Fact.cpp
//
// Demonstrates template metaprogramming for Factorials
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
#include "Fact.h"

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
// Recursive Factorial

unsigned RecursiveFact( unsigned n )
{
   return ( ( n <= 1 ) ? 1 : ( n * RecursiveFact( n - 1 ) ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Nonrecursive Factorial

unsigned NonRecursiveFact( unsigned n )
{
   unsigned int iResult = 1;
   for( unsigned int i = 1; i <= n; ++i )
      iResult *= i;
   return iResult;
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
      cassert( RecursiveFact( i ) == NonRecursiveFact( i ) );
   }

   // Test template version
   cassert( FactT( 0 ) == NonRecursiveFact( 0 ) );
   cassert( FactT( 1 ) == NonRecursiveFact( 1 ) );
   cassert( FactT( 2 ) == NonRecursiveFact( 2 ) );
   cassert( FactT( 3 ) == NonRecursiveFact( 3 ) );
   cassert( FactT( 4 ) == NonRecursiveFact( 4 ) );
   cassert( FactT( 5 ) == NonRecursiveFact( 5 ) );
   cassert( FactT( 6 ) == NonRecursiveFact( 6 ) );
   cassert( FactT( 7 ) == NonRecursiveFact( 7 ) );
   cassert( FactT( 8 ) == NonRecursiveFact( 8 ) );
   cassert( FactT( 9 ) == NonRecursiveFact( 9 ) );

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
