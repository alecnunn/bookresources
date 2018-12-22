////////////////////////////////////////////////////////////////////////////
//	Copyright : Sebastien Schertenleib
//	          : VRLab-EPFL
//
//	Email :Sebastien.Schertenleib@epfl.ch
//
//	This source code is a part of the Game Programming Gems 5 Source Code
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
////////////////////////////////////////////////////////////////////////////



#include <math.h>


inline vhdVector3::vhdVector3()
{
	//x=0;
	//y=0;
	//z=0;
	//For effeciency reasons the constructor does NOT initialize the vector. 
}

inline vhdVector3::vhdVector3( vhtReal fX, vhtReal fY, vhtReal fZ ) 
: x( fX ), y( fY ), z( fZ )
{
}

inline vhdVector3::vhdVector3( const vhdVector3& rkVector )
: x( rkVector.x ), y( rkVector.y ), z( rkVector.z )
{
}


inline vhtReal& vhdVector3::operator [] ( int index ) const
{
	return ((vhtReal*)this)[index];
}


/** Assigns the value of the other vector.
@param
rkVector The other vector
*/
inline vhdVector3& vhdVector3::operator = ( const vhdVector3& rkVector )
{
	x = rkVector.x;
	y = rkVector.y;
	z = rkVector.z;            

	return *this;
}

inline bool vhdVector3::operator == ( const vhdVector3& rkVector ) const
{
	return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
}

inline bool vhdVector3::operator != ( const vhdVector3& rkVector ) const
{
	return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
}

// arithmetic operations
inline vhdVector3 vhdVector3::operator + ( const vhdVector3& rkVector ) const
{
	vhdVector3 kSum;

	kSum.x = x + rkVector.x;
	kSum.y = y + rkVector.y;
	kSum.z = z + rkVector.z;

	return kSum;
}

inline vhdVector3 vhdVector3::operator - ( const vhdVector3& rkVector ) const
{
	vhdVector3 kDiff;

	kDiff.x = x - rkVector.x;
	kDiff.y = y - rkVector.y;
	kDiff.z = z - rkVector.z;

	return kDiff;
}

inline vhdVector3 vhdVector3::operator * ( vhtReal fScalar ) const
{
	vhdVector3 kProd;

	kProd.x = fScalar*x;
	kProd.y = fScalar*y;
	kProd.z = fScalar*z;

	return kProd;
}

inline vhdVector3 vhdVector3::operator * ( const vhdVector3& rhs) const
{
	vhdVector3 kProd;

	kProd.x = rhs.x * x;
	kProd.y = rhs.y * y;
	kProd.z = rhs.z * z;

	return kProd;
}

inline vhdVector3 vhdVector3::operator / ( vhtReal fScalar ) const
{
	vhdVector3 kDiv;

	vhtReal fInv = 1.0 / fScalar;
	kDiv.x = x * fInv;
	kDiv.y = y * fInv;
	kDiv.z = z * fInv;

	return kDiv;
}



inline vhdVector3 vhdVector3::operator - () const
{
	vhdVector3 kNeg;

	kNeg.x = -x;
	kNeg.y = -y;
	kNeg.z = -z;

	return kNeg;
}

inline vhdVector3 operator * ( vhtReal fScalar, const vhdVector3& rkVector )
{
	vhdVector3 kProd;

	kProd.x = fScalar * rkVector.x;
	kProd.y = fScalar * rkVector.y;
	kProd.z = fScalar * rkVector.z;

	return kProd;
}

// arithmetic updates
inline vhdVector3& vhdVector3::operator += ( const vhdVector3& rkVector )
{
	x += rkVector.x;
	y += rkVector.y;
	z += rkVector.z;

	return *this;
}

inline vhdVector3& vhdVector3::operator -= ( const vhdVector3& rkVector )
{
	x -= rkVector.x;
	y -= rkVector.y;
	z -= rkVector.z;

	return *this;
}

inline vhdVector3& vhdVector3::operator *= ( vhtReal fScalar )
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
	return *this;
}

inline vhdVector3& vhdVector3::operator /= ( vhtReal fScalar )
{
	vhtReal fInv = 1.0 / fScalar;

	x *= fInv;
	y *= fInv;
	z *= fInv;

	return *this;
}

inline vhdVector3& vhdVector3::operator *= ( const vhdVector3& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}

inline vhdVector3& vhdVector3::operator /= ( const vhdVector3& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	return *this;
}


