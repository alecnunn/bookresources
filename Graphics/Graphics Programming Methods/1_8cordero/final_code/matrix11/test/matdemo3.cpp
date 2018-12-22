//////////////////////////////////////////////////////
// File: MatDemo3.cpp
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

#ifndef _NO_EXCEPTION
#  define TRYBEGIN()	try {
#  define CATCHERROR()	} catch (const STD::exception& e) { \
                     cerr << "Error: " << e.what() << endl; }
#else
#  define TRYBEGIN()
#  define CATCHERROR()
#endif

typedef matrix<double> Matrix;

int main ()
{
	Matrix m(3,3);

	TRYBEGIN()
	{
		cout << "Enter a matrix of (3X3) order:\n";
		cin >> m;
		cout << "This is a ";

		if (m.IsSquare())
			cout << "Square ";

		if (m.IsSingular())
			cout << "Singular ";

		if (m.IsDiagonal())
			cout << "Diagonal ";

		if (m.IsUnit())
			cout << "Unit ";

		if (m.IsNull())
			cout << "Null ";

		if (m.IsSymmetric())
			cout << "Symmetric ";

		if (m.IsSkewSymmetric())
			cout << "SkewSymmetric ";

		if (m.IsUpperTriangular())
			cout << "UpperTiangular ";

		if (m.IsLowerTriangular())
			cout << "LowerTiangular ";

		if (m.IsScalar())
			cout << "Scaler ";
	}
	CATCHERROR();

  cout << "matrix.\n";

   return 0;
}
