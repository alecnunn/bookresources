///////////////////////////////////////////////////////////////////////////////
//
// Matrix.cpp
//
// Demonstrates template metaprogramming for matrix operations
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
#include <time.h>
#endif
#include <cstdlib>
#include "matrix.h"

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
// Initialize matrix to random values

template < class Mtx, unsigned N >
void InitRandom( Mtx& mtx)
{
   const float fRandMax = float( RAND_MAX );
#ifdef WIN32
   srand( GetTickCount() );
#else
   srand(time(NULL));
#endif
   for( int i = 0; i < N; ++i )
      for( int j = 0; j < N; ++j )
         mtx[i][j] = float( rand() ) / fRandMax;
}

#define RandMtxT( MtxType, Mtx, N ) InitRandom< MtxType, N >( Mtx )

} // end of local namespace

///////////////////////////////////////////////////////////////////////////////
//
// Test function

int __cdecl main( int, char* [] )
{
   // Test identity matrix
   matrix33 m;
   matrix33	n;
   IdentityMtxT( matrix33, m, 3 );
   n.identity();
   cassert( m == n );

   matrix44 p;
   matrix44 q;
   IdentityMtxT( matrix44, p, 4 );
   q.identity();
   cassert( p == q );

   // Test initialize matrix
   InitMtxT( matrix33, m, 3, float, 0 ); // init to zeros
   n.col[0].x = 0.0; n.col[0].y = 0.0; n.col[0].z = 0.0;
   n.col[1].x = 0.0; n.col[1].y = 0.0; n.col[1].z = 0.0;
   n.col[2].x = 0.0; n.col[2].y = 0.0; n.col[2].z = 0.0;
   cassert( m == n );

   InitMtxT( matrix33, m, 3, float, 1 ); // init to ones
   n.col[0].x = 1.0; n.col[0].y = 1.0; n.col[0].z = 1.0;
   n.col[1].x = 1.0; n.col[1].y = 1.0; n.col[1].z = 1.0;
   n.col[2].x = 1.0; n.col[2].y = 1.0; n.col[2].z = 1.0;
   cassert( m == n );

   InitMtxT( matrix44, p, 4, float, 0 ); // init to zeros
   q.col[0].x = 0.0; q.col[0].y = 0.0; q.col[0].z = 0.0; q.col[0].w = 0.0;
   q.col[1].x = 0.0; q.col[1].y = 0.0; q.col[1].z = 0.0; q.col[1].w = 0.0;
   q.col[2].x = 0.0; q.col[2].y = 0.0; q.col[2].z = 0.0; q.col[2].w = 0.0;
   q.col[3].x = 0.0; q.col[3].y = 0.0; q.col[3].z = 0.0; q.col[3].w = 0.0;
   cassert( p == q );

   InitMtxT( matrix44, p, 4, float, 1 ); // init to ones
   q.col[0].x = 1.0; q.col[0].y = 1.0; q.col[0].z = 1.0; q.col[0].w = 1.0;
   q.col[1].x = 1.0; q.col[1].y = 1.0; q.col[1].z = 1.0; q.col[1].w = 1.0;
   q.col[2].x = 1.0; q.col[2].y = 1.0; q.col[2].z = 1.0; q.col[2].w = 1.0;
   q.col[3].x = 1.0; q.col[3].y = 1.0; q.col[3].z = 1.0; q.col[3].w = 1.0;
   cassert( p == q );

   // Test transpose matrix
   RandMtxT( matrix33, m, 3 );
   n = m;
   TransMtxT( matrix33, m, 3 );
   n.transpose();
   cassert( m == n );

   RandMtxT( matrix44, p, 4 );
   q = p;
   TransMtxT( matrix44, p, 4 );
   q.transpose();
   cassert( p == q );

   // Test multiply matrix 
   matrix33 mn;
   matrix33 mnc;
   RandMtxT( matrix33, m, 3 );
   RandMtxT( matrix33, n, 3 );
   MultMtxT( matrix33, mn, m, n, 3 );
   mnc = m * n;
   cassert( mn == mnc );

   matrix44 pq;
   matrix44 pqc;
   RandMtxT( matrix44, p, 4 );
   RandMtxT( matrix44, q, 4 );
   MultMtxT( matrix44, pq, p, q, 4 );
   pqc = p * q;
   cassert( pq == pqc );

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
