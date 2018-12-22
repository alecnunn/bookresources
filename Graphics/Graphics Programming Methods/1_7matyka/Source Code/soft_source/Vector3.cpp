
//#include <iostream>

#include <stdio.h>
#include <math.h>
#include "Vector3.h"




// metody



double Vector3::length()
{
	return sqrt( (*this) * (*this));			// szybciej niz iloczyn skalarny
}


void Vector3::norm()
{
	double d = length();

	if(d)
		*this = *this * (1/d);
}
	


// definicje operatorów


// + dodawanie skladowych
Vector3 Vector3::operator+(Vector3 const &w)
{
	return Vector3(x + w.x, y + w.y, z + w.z);
}

// - odejmowanie skladowych
Vector3 Vector3::operator-(Vector3 const &w)
{
	return Vector3(x - w.x, y - w.y, z - w.z);
}

// * iloczyn Vector3 - skalar
Vector3 Vector3::operator*(double liczba)
{
	return Vector3(x*liczba, y*liczba, z*liczba);
}

// iloczyn skalarny
double Vector3::operator*(Vector3 const &w)
{
	return (x*w.x + y*w.y + z*w.z);
}

// iloczyn Vector3owy (bo modulo z Vector3a nie istnieje, wiec wykorzystuje operator modulo)
Vector3 Vector3::operator%(Vector3 const &w)
{
	return Vector3(y*w.z - z*w.y, z*w.x - x*w.z, x*w.y - y*w.x);
}

// rownosc dwu Vector3ow
void Vector3::operator=(Vector3 const &w)
{
	x = w.x;
	y = w.y;
	z = w.z;
}

  












