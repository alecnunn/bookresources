//////////////////////////////////////////////////////
// File: MatDemo1.cpp
// Copyright (c) 1997-2002 Techsoft Private Limited
// Purpose: Demonstrates how to use Matrix TCL Lite v1.13
// Web: http://www.techsoftpl.com/matrix/
// Email: matrix@techsoftpl.com
//
                          
#include <time.h>                    
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

#ifndef _NO_TEMPLATE
typedef matrix<double> Matrix;
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


void WaitForEnterKey ();

int main ()
{
	TRYBEGIN()
	{
		cout << "Creating two random number matrices M1 and M2:\n";
		Matrix m1;
		int i,j;

		srand( (unsigned)time( NULL ) );
		for (i=0; i < 4; i++)
			for (j=0; j < 4; j++)
				m1(i,j) = 12/((rand()%25)+1.0);
		m1.SetSize(4,4);

		Matrix m2(4,4);

		for (i=0; i < 4; i++)
        for (j=0; j < 4; j++)
           m2(i,j) = 12/((rand()%25)+1.0);

		cout << "\nThe matrix M1 is:\n" << m1 << endl;
		WaitForEnterKey();

		cout << "\nThe matrix M2 is:\n" << m2 << endl;
		WaitForEnterKey();

		Matrix m3 = m1 + m2;
        cout << "\nMatrix addition:\nM1 + M2 = \n" << m3 << endl;
		WaitForEnterKey();

		m3 = m1 - m2;
        cout << "\nMatrix subtraction:\nM1 - M2 = \n" << m3 << endl;
		WaitForEnterKey();

		m3 = m1 * m2;
        cout << "\nMatrix multiplication:\nM1 * M2 = \n" << m3 << endl;
		WaitForEnterKey();

		m3 = !m1;
        cout << "\nMatrix inversion:\nInv M1 = \n" << m3 << endl;
		WaitForEnterKey();

		m3 = m1 ^ 4U;
        cout << "\nMatrix power:\nM ^ 4 = \n" << m3 << endl;
		WaitForEnterKey();

		m3 = m1.Adj();
        cout << "\nAdjoint of M1:\nAdj M1 = \n" <<  m3 << endl;
		WaitForEnterKey();

		cout << "\nCofactor of M1(0,0) = " << m1.Cofact(0,0) << endl;
		cout << "Determinant of M1 = " << m1.Det() << endl;
		cout << "Condition No. of M1 = " << m1.Cond() << endl;
		cout << "Norm of M1 = " << m1.Norm() << endl << endl;
		WaitForEnterKey();

#if !defined(_MSC_VER) || _MSC_VER > 1020
		m3 = (m1 * 2.0) + (m2 ^ 3U) - (2.0 * m1 / 3.0) * (1.0 / m2);
		cout << "\nMatrix equation:\n" ;
		cout << "(M1 * 2) + (M2 ^ 3U) - (2 * M1 /3) * (1/M2) = \n" << m3 << endl;
		WaitForEnterKey();
#endif
	}
	CATCHERROR();

   return 0;
}


void WaitForEnterKey ()
{
	char ch = '\0';

	cout << "Press ENTER to continue . . . ";
	cout.flush();

	while (!(ch == '\n' || ch == '\r'))
		ch = (char)getchar();

	return;
}


