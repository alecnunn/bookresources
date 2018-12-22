///////////////////////////////////////////////////////////////////////////////
//
// Trig.h
//
// Demonstrates template metaprogramming for Trigonometry
//
// Defines the following template functions, where r is between 0 and 2pi
//
//    SineT( r )  recursive version
//    SineN( r )  non-recursive version
//    CosN ( r )  non-recursive version
//    TanN ( r )  non-recursive version
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

#pragma warning( disable: 4710 ) // ignore function not inlined message
#pragma inline_depth( 255 )
#pragma inline_recursion( on )

///////////////////////////////////////////////////////////////////////////////
//
// Recursive Sine

// Series expansion for sin( R ).
// For conforming compilers, change double R to double& R
#ifdef WIN32
template< double R > struct Sine
#else
template< double& R > struct Sine
#endif
{
   enum { MaxTerms = 10 }; // increase for accuracy
   static inline double sin()
   {
      return R * Series< R, 0, MaxTerms >::val();
   }
};

template< double R, int I, int MaxTerms >
struct Series
{
   enum
   {
      // Continue is true until we've evaluated M terms
      Continue = I + 1 != MaxTerms,
      NxtI = ( I + 1 ) * Continue,
      NxtMaxTerms = MaxTerms * Continue
   };

   // Recursive definition called once per term
   static inline double val()
   {
      return 1 - R * R / (2.0 * I + 2.0) / 
             (2.0 * I + 3.0) * Series< R * Continue, NxtI, NxtMaxTerms >::val();
   }
};

// Specialization to terminate the loop
template <> struct Series< 0.0, 0, 0 >
{
   static inline double val() { return 1.0; }
};

// Make the template appear like a function
#define SineT( r ) Sine< r >::sin()

///////////////////////////////////////////////////////////////////////////////
//
// Nonrecursive Sine

// Series expansion for sin( R ).
// For conforming compilers, change double R to double& R
#ifdef WIN32
template < double R > struct NrSine
#else
template < double& R > struct NrSine
#endif
{
   // All values known at compile time.
   // A decent compiler should be able to reduce to a single constant.
   static inline double sin()
   {
      double Rsqr = R * R;
      return R * ( 1.0 - Rsqr / 2.0  / 3.0
               * ( 1.0 - Rsqr / 4.0  / 5.0
               * ( 1.0 - Rsqr / 6.0  / 7.0
               * ( 1.0 - Rsqr / 8.0  / 9.0
               * ( 1.0 - Rsqr / 10.0 / 11.0
               * ( 1.0 - Rsqr / 12.0 / 13.0
               * ( 1.0 - Rsqr / 14.0 / 15.0
               * ( 1.0 - Rsqr / 16.0 / 17.0
               * ( 1.0 - Rsqr / 18.0 / 19.0
               * ( 1.0 - Rsqr / 20.0 / 21.0 
               ) ) ) ) ) ) ) ) ) );
   }
};

// Make the template appear like a function
#define SineN( r ) NrSine< r >::sin()

///////////////////////////////////////////////////////////////////////////////
//
// Nonrecursive Cosine

// Series expansion for cos( R ).
// For conforming compilers, change double R to double& R
#ifdef WIN32
template < double R > struct NrCos
#else
template < double& R > struct NrCos
#endif
{
   // All values known at compile time.
   // A decent compiler should be able to reduce to a single constant.
   static inline double cos()
   {
      double Rsqr = R * R;
      return     ( 1.0 - Rsqr / 2.0
               * ( 1.0 - Rsqr / 3.0  / 4.0
               * ( 1.0 - Rsqr / 5.0  / 6.0
               * ( 1.0 - Rsqr / 7.0  / 8.0
               * ( 1.0 - Rsqr / 9.0  / 10.0
               * ( 1.0 - Rsqr / 11.0 / 12.0
               * ( 1.0 - Rsqr / 13.0 / 14.0
               * ( 1.0 - Rsqr / 15.0 / 16.0
               * ( 1.0 - Rsqr / 17.0 / 18.0
               * ( 1.0 - Rsqr / 19.0 / 20.0 
               * ( 1.0 - Rsqr / 21.0 / 22.0 
               ) ) ) ) ) ) ) ) ) ) );
   }
};

// Make the template appear like a function
#define CosN( r ) NrCos< r >::cos()

///////////////////////////////////////////////////////////////////////////////
//
// Nonrecursive Tangent

// Series expansion for tan( R ).
// For conforming compilers, change double R to double& R
#ifdef WIN32
template < double R > struct NrTan
#else
template < double& R > struct NrTan
#endif
{
   // All values known at compile time.
   // A decent compiler should be able to reduce to a single constant.
   static inline double tan()
   {
      return NrSine< R >::sin() / NrCos< R >::cos();
   }
};

// Make the template appear like a function
#define TanN( r ) NrTan< r >::tan()

///////////////////////////////////////////////////////////////////////////////
