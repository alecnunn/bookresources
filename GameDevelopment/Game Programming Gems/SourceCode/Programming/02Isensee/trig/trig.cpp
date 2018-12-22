///////////////////////////////////////////////////////////////////////////////
//
// Trig.cpp
//
// Demonstrates template metaprogramming for trigonometry
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
#endif
#include <cstdio>
#include <cstring>
#include <cmath>
#include "trig.h"

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

const double pi = 3.14159265358979323846264338328;

///////////////////////////////////////////////////////////////////////////////
//
// Recursive Sine

double SineSeries( double fRad, int i, int iMaxTerms );

double RecursiveSine( double fRad )
{
   const int iMaxTerms = 10;
   return fRad * SineSeries( fRad, 0, iMaxTerms );  
}

double SineSeries( double fRad, int i, int iMaxTerms )
{
   if( i > iMaxTerms )
      return 1.0;

   return 1.0 - ( fRad * fRad / (2.0 * i + 2.0) / (2.0 * i + 3.0) 
                  * SineSeries( fRad, i + 1, iMaxTerms ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Nonrecursive Sine

double NonRecursiveSine( double fRad )
{
   double fRsqr = fRad * fRad;
   return fRad * ( 1.0 - fRsqr / 2.0  / 3.0
               * ( 1.0 - fRsqr / 4.0  / 5.0
               * ( 1.0 - fRsqr / 6.0  / 7.0
               * ( 1.0 - fRsqr / 8.0  / 9.0
               * ( 1.0 - fRsqr / 10.0 / 11.0
               * ( 1.0 - fRsqr / 12.0 / 13.0
               * ( 1.0 - fRsqr / 14.0 / 15.0
               * ( 1.0 - fRsqr / 16.0 / 17.0
               * ( 1.0 - fRsqr / 18.0 / 19.0
               * ( 1.0 - fRsqr / 20.0 / 21.0 
               ) ) ) ) ) ) ) ) ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Simple float comparison uses sprintf to compare given number of significant digits

bool CompareFloat( double x, double y, int iSignificantDigits = 3 )
{
   char xBuffer[128];
   char yBuffer[128];

   sprintf( xBuffer, "%.*f", iSignificantDigits, x );
   sprintf( yBuffer, "%.*f", iSignificantDigits, y );

   return( strcmp( xBuffer, yBuffer ) == 0 );
}

} // end of local namespace

///////////////////////////////////////////////////////////////////////////////
//
// Test function

int __cdecl main( int, char* [] )
{
   double fRad;

   // Test recursive vs nonrecursive
   for( fRad = 0.0; fRad < 2 * pi; fRad += 0.01 )
   {
      cassert( CompareFloat( RecursiveSine( fRad ), NonRecursiveSine( fRad ) ) );
   }

   // Test nonrecursive vs CRT
   for( fRad = 0.0; fRad < 2 * pi; fRad += 0.01 )
   {
      cassert( CompareFloat( sin( fRad ), NonRecursiveSine( fRad ) ) );
   }

   // Test template recursive version
   cassert( CompareFloat( SineT( 0.0    ), sin( 0.0    ) ) );
   cassert( CompareFloat( SineT( 0.1    ), sin( 0.1    ) ) );
   cassert( CompareFloat( SineT( 0.21   ), sin( 0.21   ) ) );
   cassert( CompareFloat( SineT( 0.3    ), sin( 0.3    ) ) );
   cassert( CompareFloat( SineT( 0.41   ), sin( 0.41   ) ) );
   cassert( CompareFloat( SineT( 0.501  ), sin( 0.501  ) ) );
   cassert( CompareFloat( SineT( 0.6001 ), sin( 0.6001 ) ) );
   cassert( CompareFloat( SineT( 0.7    ), sin( 0.7    ) ) );
   cassert( CompareFloat( SineT( 0.85   ), sin( 0.85   ) ) );
   cassert( CompareFloat( SineT( 0.9    ), sin( 0.9    ) ) );
   cassert( CompareFloat( SineT( 1.03   ), sin( 1.03   ) ) );
   cassert( CompareFloat( SineT( 2.22   ), sin( 2.22   ) ) );
   cassert( CompareFloat( SineT( 3.23   ), sin( 3.23   ) ) );
   cassert( CompareFloat( SineT( 4.89   ), sin( 4.89   ) ) );
   cassert( CompareFloat( SineT( 5.66   ), sin( 5.66   ) ) );
   cassert( CompareFloat( SineT( 6.1    ), sin( 6.1    ) ) );

   // Test template nonrecursive verison
   cassert( CompareFloat( SineN( 0.0    ), sin( 0.0    ) ) );
   cassert( CompareFloat( SineN( 0.1    ), sin( 0.1    ) ) );
   cassert( CompareFloat( SineN( 0.21   ), sin( 0.21   ) ) );
   cassert( CompareFloat( SineN( 0.3    ), sin( 0.3    ) ) );
   cassert( CompareFloat( SineN( 0.41   ), sin( 0.41   ) ) );
   cassert( CompareFloat( SineN( 0.501  ), sin( 0.501  ) ) );
   cassert( CompareFloat( SineN( 0.6001 ), sin( 0.6001 ) ) );
   cassert( CompareFloat( SineN( 0.7    ), sin( 0.7    ) ) );
   cassert( CompareFloat( SineN( 0.85   ), sin( 0.85   ) ) );
   cassert( CompareFloat( SineN( 0.9    ), sin( 0.9    ) ) );
   cassert( CompareFloat( SineN( 1.03   ), sin( 1.03   ) ) );
   cassert( CompareFloat( SineN( 2.22   ), sin( 2.22   ) ) );
   cassert( CompareFloat( SineN( 3.23   ), sin( 3.23   ) ) );
   cassert( CompareFloat( SineN( 4.89   ), sin( 4.89   ) ) );
   cassert( CompareFloat( SineN( 5.66   ), sin( 5.66   ) ) );
   cassert( CompareFloat( SineN( 6.1    ), sin( 6.1    ) ) );

   // Test cosine
   cassert( CompareFloat( CosN( 0.0    ), cos( 0.0    ) ) );
   cassert( CompareFloat( CosN( 0.1    ), cos( 0.1    ) ) );
   cassert( CompareFloat( CosN( 0.21   ), cos( 0.21   ) ) );
   cassert( CompareFloat( CosN( 0.3    ), cos( 0.3    ) ) );
   cassert( CompareFloat( CosN( 0.41   ), cos( 0.41   ) ) );
   cassert( CompareFloat( CosN( 0.501  ), cos( 0.501  ) ) );
   cassert( CompareFloat( CosN( 0.6001 ), cos( 0.6001 ) ) );
   cassert( CompareFloat( CosN( 0.7    ), cos( 0.7    ) ) );
   cassert( CompareFloat( CosN( 0.85   ), cos( 0.85   ) ) );
   cassert( CompareFloat( CosN( 0.9    ), cos( 0.9    ) ) );
   cassert( CompareFloat( CosN( 1.03   ), cos( 1.03   ) ) );
   cassert( CompareFloat( CosN( 2.22   ), cos( 2.22   ) ) );
   cassert( CompareFloat( CosN( 3.23   ), cos( 3.23   ) ) );
   cassert( CompareFloat( CosN( 4.89   ), cos( 4.89   ) ) );
   cassert( CompareFloat( CosN( 5.66   ), cos( 5.66   ) ) );
   cassert( CompareFloat( CosN( 6.1    ), cos( 6.1    ) ) );

   // Test tangent
   cassert( CompareFloat( TanN( 0.0    ), tan( 0.0    ) ) );
   cassert( CompareFloat( TanN( 0.1    ), tan( 0.1    ) ) );
   cassert( CompareFloat( TanN( 0.21   ), tan( 0.21   ) ) );
   cassert( CompareFloat( TanN( 0.3    ), tan( 0.3    ) ) );
   cassert( CompareFloat( TanN( 0.41   ), tan( 0.41   ) ) );
   cassert( CompareFloat( TanN( 0.501  ), tan( 0.501  ) ) );
   cassert( CompareFloat( TanN( 0.6001 ), tan( 0.6001 ) ) );
   cassert( CompareFloat( TanN( 0.7    ), tan( 0.7    ) ) );
   cassert( CompareFloat( TanN( 0.85   ), tan( 0.85   ) ) );
   cassert( CompareFloat( TanN( 0.9    ), tan( 0.9    ) ) );
   cassert( CompareFloat( TanN( 1.03   ), tan( 1.03   ) ) );
   cassert( CompareFloat( TanN( 2.22   ), tan( 2.22   ) ) );
   cassert( CompareFloat( TanN( 3.23   ), tan( 3.23   ) ) );
   cassert( CompareFloat( TanN( 4.89   ), tan( 4.89   ) ) );
   cassert( CompareFloat( TanN( 5.66   ), tan( 5.66   ) ) );
   cassert( CompareFloat( TanN( 6.1    ), tan( 6.1    ) ) );

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
