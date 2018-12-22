#include <iostream>
#include "Vector3.h"
using namespace std;

int main(int argc, char *argv[])
{
	cout << "VECTOR TEST" << endl;

	Vector3 A(5,5,1);
	cout << "A = " << A.getX() << "," 
		<< A.getY() << "," << A.getZ() << endl;

	Vector3 B(90,80,1);
	cout << "B = " << B.getX() << "," 
		<< B.getY() << "," << B.getZ() << endl;

	cout << "Distance A to B: " 
		<< A.Distance( B ) << endl;

	cout << "Length of A: " << A.Length() << endl;
	cout << "Length of B: " << B.Length() << endl;

	A.Move(5, 0, 0);
	cout << "A moved: " << A.getX() << "," 
		<< A.getY() << "," << A.getZ() << endl;

	Vector3 C = A;
	cout << "C = " << C.getX() << "," << C.getY() 
		<< "," << C.getZ() << endl;

	cout << "Dot Product of A and B: " 
		<< A.DotProduct(B) << endl;

	Vector3 D = A.CrossProduct(B);
	cout << "Cross Product of A and B: " << D.getX() 
		<< "," << D.getY() << "," << D.getZ() << endl;

	D = A.Normal();
	cout << "Normal of A: " << D.getX() << "," 
		<< D.getY() << "," << D.getZ() << endl;

	A.Set(2.1,2.2,2.3);
	B.Set(3.1,3.2,3.3);
	cout << "A = " << A.getX() << "," << A.getY() 
		<< "," << A.getZ() << endl;
	cout << "B = " << B.getX() << "," << B.getY() 
		<< "," << B.getZ() << endl;

	A += B;
	cout << "A + B: " << A.getX() << "," << A.getY() 
		<< "," << A.getZ() << endl;

	A -= B;
	cout << "A - B: " << A.getX() << "," << A.getY() 
		<< "," << A.getZ() << endl;

	A *= B;
	cout << "A * B: " << A.getX() << "," << A.getY() 
		<< "," << A.getZ() << endl;

	A /= B;
	cout << "A / B: " << A.getX() << "," << A.getY() 
		<< "," << A.getZ() << endl;

	cout << "A == B: " << (A == B) << endl;
	
	system("pause");

	return 0;
}
