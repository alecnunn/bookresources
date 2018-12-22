/************************************************************************/
/*                                                                      */
/*                       Soft Body 3.0                                  */
/*                 by Maciej Matyka, 21.IX.2004                         */
/*                                                                      */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/*                                                                      */
/*  Made for purposes of Grame Programming Gems 5 article.              */
/*                                                                      */
/*  note: this code has been attached to article  only as               */
/*  an example of described method. No any guarrantee of anything is    */
/*  is given. Use at your own risk.                                     */
/*                                                                      */
/*  if you want to use any part of that code in your project,           */
/*  email Author at:                                                    */
/*                                                                      */
/*  maq@panoramix.ift.uni.wroc.pl                                       */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/************************************************************************/




#include <cmath>
#include "CVector3.h"


/// Output vector (console)

ostream &operator<<(ostream &F, CVector3 &v)
{
	F << "(";
	F << v.x << "," << v.y << "," << v.z;
	F << ")";

	return F;
}



/// Multiply scalar * vector

CVector3 operator*(float scalar, CVector3 &v)
{
	return CVector3(v.x*scalar,v.y*scalar,v.z*scalar);
}


/// Add two vectors.

CVector3 CVector3::operator+(CVector3 const &w)
{
	return CVector3(x + w.x, y + w.y, z + w.z);
}


/// Substract.

CVector3 CVector3::operator-(CVector3 const &w)
{
	return CVector3(x - w.x, y - w.y, z - w.z);
}


/// Multiply CVector3 times scalar.

CVector3 CVector3::operator*(float scalar)
{
	return CVector3(x*scalar, y*scalar, z*scalar);
}


/// Dot product.

float CVector3::operator*(CVector3 const &w)
{
	return (x*w.x + y*w.y + z*w.z);
}


/// Cross product.

CVector3 CVector3::operator%(CVector3 const &w)
{
	return CVector3(y*w.z - z*w.y, z*w.x - x*w.z, x*w.y - y*w.x);
}


/// Operator =.

void CVector3::operator=(CVector3 const &w)
{
	x = w.x;
	y = w.y;
	z = w.z;
}

  




