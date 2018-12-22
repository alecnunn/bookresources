///////////////////////////////////////////////////////////////////////////////
//
// Matrix.h
//
// Demonstrates template metaprogramming for matrix operations
//
// Defines the following template functions, where MtxType is the matrix
// type (e.g. matrix33) and N is the matrix size.
//
//    IdentityMtxT( MtxType, Mtx, N )              make Mtx the identity matrix
//    InitMtxT    ( MtxType, Mtx, N, F, Init )     initialize Mtx with Init
//    TransMtxT   ( MtxType, Mtx, N )              transpose Mtx
//    MultMtxT    ( MtxType, r, a, b, N )          compute r = a * b
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

#include "mtxlib.h"
#include <algorithm> // std::swap
#pragma warning( disable: 4127 ) // ignore conditional expression is constant
#pragma inline_depth( 255 )
#pragma inline_recursion( on )


///////////////////////////////////////////////////////////////////////////////
//
// Identity matrix

// N is matrix size
template< class Mtx, unsigned N > struct IdMtx
{
   static inline void eval( Mtx& mtx )
   {
      IdMtxImpl< Mtx, N, 0, 0, 0 >::eval( mtx );
   }
};

// Assigns each element of the matrix
template< class Mtx, unsigned N, unsigned C, unsigned R, unsigned I >
struct IdMtxImpl
{
   enum
   {
      NxtI = I + 1,          // Counter
      NxtR = NxtI % N,       // Row (inner loop)
      NxtC = NxtI / N % N    // Column (outer loop)
   };
   static inline void eval( Mtx& mtx )
   {
      mtx[ C ][ R ] = ( C == R ) ? 1.0 : 0.0;
      IdMtxImpl< Mtx, N, NxtC, NxtR, NxtI >::eval( mtx );
   }
};

// Specialize for 3x3 and 4x4 matrix 
template<> struct IdMtxImpl< matrix33, 3, 0, 0, 3*3 >
{
   static inline void eval( matrix33& ) {}
};
template<> struct IdMtxImpl< matrix44, 4, 0, 0, 4*4 >
{
   static inline void eval( matrix44& ) {}
};

// Make the template appear like a function
#define IdentityMtxT( MtxType, Mtx, N ) IdMtx< MtxType, N >::eval( Mtx )

///////////////////////////////////////////////////////////////////////////////
//
// Initialize matrix

// N is matrix size
template< class Mtx, unsigned N, class F, int Init > struct InitMtx
{
   static inline void eval( Mtx& mtx )
   {
      InitMtxImpl< Mtx, N, F, Init, 0, 0, 0 >::eval( mtx );
   }
};

// Assigns each element of the matrix
template< class Mtx, unsigned N, class F, int Init, unsigned C, unsigned R, unsigned I >
struct InitMtxImpl
{
   enum
   {
      NxtI = I + 1,          // Counter
      NxtR = NxtI % N,       // Row (inner loop)
      NxtC = NxtI / N % N    // Column (outer loop)
   };
   static inline void eval( Mtx& mtx )
   {
      mtx[ C ][ R ] = static_cast< F >( Init );
      InitMtxImpl< Mtx, N, F, Init, NxtC, NxtR, NxtI >::eval( mtx );
   }
};

// Specialize for 3x3 and 4x4 matrix for initial values of zero and one
template<> struct InitMtxImpl< matrix33, 3, float, 0, 0, 0, 3*3 >
{
   static inline void eval( matrix33& ) {}
};
template<> struct InitMtxImpl< matrix33, 3, float, 1, 0, 0, 3*3 >
{
   static inline void eval( matrix33& ) {}
};
template<> struct InitMtxImpl< matrix44, 4, float, 0, 0, 0, 4*4 >
{
   static inline void eval( matrix44& ) {}
};
template<> struct InitMtxImpl< matrix44, 4, float, 1, 0, 0, 4*4 >
{
   static inline void eval( matrix44& ) {}
};

// Make the template appear like a function
#define InitMtxT( MtxType, Mtx, N, F, Init ) InitMtx< MtxType, N, F, Init >::eval( Mtx )

///////////////////////////////////////////////////////////////////////////////
//
// Transpose matrix

// Templatized transpose; N is matrix size
template< class Mtx, unsigned N > struct TransMtx
{
   static inline void eval( Mtx& mtx )
   {
      TransMtxImpl< Mtx, N, 0, 1, 0 >::eval( mtx );
   }
};

template< class Mtx, unsigned N, unsigned C, unsigned R, unsigned I >
struct TransMtxImpl
{
   enum
   {
      NxtI = I + 1,
      NxtC = NxtI / N % N,
      NxtR = ( NxtI % N ) + NxtC + 1
   };
   static inline void eval( Mtx& mtx )
   {
      if( R < N )
         std::swap( mtx[ C ][ R ], mtx[ R ][ C ] );
      TransMtxImpl< Mtx, N, NxtC, NxtR, NxtI >::eval( mtx );
   }
};

// Specialize for 3x3 and 4x4 matrix
template<> struct TransMtxImpl< matrix33, 3, 0, 1, 3*3 >
{
   static inline void eval( matrix33& ) {}
};
template<> struct TransMtxImpl< matrix44, 4, 0, 1, 4*4 >
{
   static inline void eval( matrix44& ) {}
};

// Make the template appear like a function 
#define TransMtxT( MtxType, Mtx, N ) TransMtx< MtxType, N >::eval( Mtx )

///////////////////////////////////////////////////////////////////////////////
//
// Multiply matrix

// Templatized multiplication; N is matrix size 
template< class Mtx, unsigned N > struct MultMtx
{
   static inline void eval( Mtx& r, const Mtx& a, const Mtx& b )
   {
      InitMtxT( Mtx, r, N, float, 0 ); // clear the result
      MultMtxImpl< Mtx, N, 0, 0, 0, 0 >::eval( r, a, b );
   }
};

template< class Mtx, unsigned N, unsigned C, unsigned R, unsigned K, unsigned I >
struct MultMtxImpl
{
   enum
   {
      NxtI = I + 1,           // Counter
      NxtK = NxtI % N,        // Internal loop
      NxtC = NxtI / N % N,    // Column
      NxtR = NxtI / N / N % N // Row
   };
   static inline void eval( Mtx& r, const Mtx& a, const Mtx& b )
   {
      r[ C ][ R ] += a[ K ][ R ] * b[ C ][ K ];
      MultMtxImpl< Mtx, N, NxtC, NxtR, NxtK, NxtI >::eval( r,a,b );
   }
};

// Specialize for 3x3 and 4x4 matrix
template<> struct MultMtxImpl< matrix33, 3, 0, 0, 0, 3*3*3 >
{
   static inline void eval( matrix33&, const matrix33&, const matrix33& ) {}
};
template<> struct MultMtxImpl< matrix44, 4, 0, 0, 0, 4*4*4 >
{
   static inline void eval( matrix44&, const matrix44&, const matrix44& ) {}
};

// Make the template appear like a function 
#define MultMtxT( MtxType, r, a, b, N ) MultMtx< MtxType, N >::eval( r, a, b )

///////////////////////////////////////////////////////////////////////////////
