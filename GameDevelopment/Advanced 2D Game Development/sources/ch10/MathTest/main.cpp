#include <iostream>
#include <iomanip>
#include "Math.h"

using namespace std;

int main(int argc, char *argv[])
{
	Math math;
	double angle,x,y;

	cout << "MATH TEST" << endl << endl;
	cout.setf(ios::fixed); 
	cout << setprecision(2);

	Vector3 A(5,5,1);
	cout << "A = " << A.getX() << "," << A.getY() << "," << A.getZ() << endl;

	Vector3 B(90,80,1);
	cout << "B = " << B.getX() << "," << B.getY() << "," << B.getZ() << endl;

	cout << endl << "Distance: " << math.Distance( A, B ) << endl;

	cout << "Dot Product: " << math.DotProduct( A, B ) << endl;

	Vector3 D = math.CrossProduct( A,B );
	cout << "Cross Product: " << D.getX() << "," << D.getY() << "," << D.getZ() << endl;

	D = math.Normal( A );
	cout << "Normalized A: " << D.getX() << "," 
		<< D.getY() << "," << D.getZ() << endl;
	D = math.Normal( B );
	cout << "Normalized B: " << D.getX() << "," 
		<< D.getY() << "," << D.getZ() << endl << endl;

	angle = math.AngleToTarget( A, B );
	cout << "Angle to target: " << angle << " radians (";
	cout << math.toDegrees(angle) << " degrees)" << endl << endl;

	for (angle=0; angle<360; angle+=45) 
	{
		x = math.LinearVelocityX(angle);
		y = math.LinearVelocityY(angle);
		cout << "Linear velocity (" << setprecision(0) << angle << " degrees): ";
		cout << setprecision(2) << x << "," << y << endl;
	}

	system("pause");

	return 0;
}
