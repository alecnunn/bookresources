///////////////////////////////////////////////////////////////////////////////
//
// Fact.h
//
// Demonstrates template metaprogramming for Factorials
//
// Defines the following template functions, where n is an unsigned int
//
//    FactT( n )
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

// Template Factorial
template< unsigned N > struct Fact
{
   enum { Val = N * Fact< N - 1 >::Val };
};

// Specialization for base cases (termination conditions)
template <> struct Fact< 0 > { enum { Val = 1 }; };
template <> struct Fact< 1 > { enum { Val = 1 }; };

// Make the template appear like a function
#define FactT( n ) Fact< n >::Val

///////////////////////////////////////////////////////////////////////////////
