///////////////////////////////////////////////////////////////////////////////
//
// Fib.h
//
// Demonstrates template metaprogramming for Fibonacci sequence
//
// Defines the following template functions, where n is an unsigned int
//
//    FibT( n )
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

// Template Fibonacci
template< unsigned N > struct Fib
{
   enum
   {
      // Recursive definition
      Val = Fib< N-1 >::Val + Fib< N-2 >::Val
   }; 
};

// Specialization for base cases (termination conditions)
template <> struct Fib< 0 > { enum { Val = 0 }; };
template <> struct Fib< 1 > { enum { Val = 1 }; };

// Make the template appear like a function
#define FibT( n ) Fib< n >::Val

///////////////////////////////////////////////////////////////////////////////
