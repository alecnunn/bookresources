//////////////////////////////////////////////////////
// File: MatDemo2.cpp
// Copyright (c) 1997-2002 Techsoft Private Limited
// Purpose: Demonstrates how to use Matrix TCL Lite v1.13
// Web: http://www.techsoftpl.com/matrix/
// Email: matrix@techsoftpl.com
//

#include "matrix.h"

////////////////////////////////
// Note: The following conditional compilation statements are included
//       so that you can (likely to) compile this sample, without any 
//       modification, using a compiler which does not support any or 
//       all of the ANSI C++ features like NAMEPACE, TEMPLATE, and 
//       EXCEPTION, used in this class.
//
//       If you have a compiler, such as C++ Builder, Borland C++ 5.0,
//       MS Visual C++ 5.0 or higher, etc., which supports most of the ANSI 
//       C++ features used in this class, you do not need to include these
//       statements in your program.
//
#ifndef _NO_NAMESPACE 
using namespace std;
using namespace math;
#define STD std
#else
#define STD
#endif
 
#if !defined(_NO_TEMPLATE)
#  if defined(_MSC_VER)
#     if _MSC_VER > 1000
#        include <complex>
         typedef complex<double> type;
#     else
         typedef double type;
#     endif
#  elif defined(__BORLANDC__)
#     if defined(__WIN32__)
#        include <complex>
         typedef complex<double> type;
#     else
#        include <complex.h>
         typedef complex type;
#     endif         
#  elif defined( __GNUG__ ) 
#       include <complex>
        typedef complex<double> type;
# elif defined( _SGI_BROKEN_STL )
# include <complex>
typedef std::complex<double> type;
#  endif
  typedef matrix<type> Matrix;
#else
   typedef matrix Matrix;
#endif

#ifndef _NO_EXCEPTION
#  define TRYBEGIN()	try {
#  define CATCHERROR()	} catch (const STD::exception& e) { \
                     cerr << "Error: " << e.what() << endl; }
#else
#  define TRYBEGIN()
#  define CATCHERROR()
#endif


int main ()
{
   Matrix m(3,3),v(3,1),s(3,1);

   cout << "Solution of linear equations of complex number:\n\n";
   cout << "Enter the linear equations as (3x3) matrix:\n";
   cin >> m;
   cout << "Enter the vector as (3x1) matrix to solve it:\n";
   cin >> v;

   TRYBEGIN()
   {
     s = m.Solve(v);
   }
   CATCHERROR();

   cout << "Solution:\n" << s << endl;

   return 0;
}

