#ifndef _MATH_H
#define _MATH_H

#include <math.h>

// Basic mathematical structures and algorithms
// Common operations such as dot products are put here to make sample code clearer

// Epsilon value
const float EPSILON	= 1e-03f;
const float NORMAL_EPSILON		= 0.00001f;		// Epsilon used for normal comparisons

// Tests if two floating point values are within EPSILON range of each other.
#define IS_FLOAT_EQUAL( f , t ) ( (f) < ( (t) + EPSILON ) && (f) > ( (t) - EPSILON ) )
#define IS_FLOAT_EQUAL_EPSILON( f , t, e ) ( (f) < ( (t) + e ) && (f) > ( (t) - e ) )

// Vectors have 3 components
enum eVectorComponents
{
	evcX,
	evcY,
	evcZ,
};

// Vector class

class CVector
{

public :

	float fX;					// x component
	float fY;					// y component
	float fZ;					// z component

	// Constructors.
	inline CVector();
	inline CVector( float fVx, float fVy, float fVz );

	// Equivalence operators.
	inline bool		operator == ( const CVector& vVec ) const;
    inline bool		operator != ( const CVector& vVec ) const;

	// Arithmetic operators.
	inline CVector	operator - ( const CVector& vVec ) const;
	inline CVector	operator + ( const CVector& vVec ) const;
	inline CVector	operator - () const;

	// Dot product.
    inline float	operator * ( const CVector& vVec ) const;

	// Cross product.
	inline CVector	CrossProduct( const CVector& vVec ) const;	
	// Normalized cross product.
	inline CVector	NormCrossProduct( const CVector& vVec ) const;

	// Normalizes vector; Returns the old vector length.
	inline float	Normalize();

	// Scalar operations.
    inline CVector	operator * ( float fScalar ) const;
    inline CVector	operator / ( float fScalar ) const;

	// Accesses the vectors as an array of floats.
	inline float&	operator[] ( DWORD uIndex ) const;

	// Returns length of vector.
	inline float	Length() const;

	// Sets the vector's values directly.
	inline void		Set( float fVx, float fVy, float fVz );

};

// Plane class

class CPlane
{
public :

	CVector	m_vNormal;		// Normal vector of the plane
	float	m_fConstant;	// Constant of the plane

	// This enum is used as a result when testing where polygons are with respect to the plane
	enum ePlaneSides 
	{ 
		epsUNKNOWN	= -1,	// Unknown state
		epsFRONT,			// Object is fully in front of plane
		epsBACK,			// Object is fully behind the plane
		epsALIGNED,			// Object straddles the plabe
		epsMAXSIDES			// Number of valid plane states
	};

	// Returns distance between a point and the plane.
	//
	// Parameters:
	//	vPoint		Point to test against the plane.
	//
	// Returns:		The distance between vPoint and the plane.
	inline	float		GetDistance( const CVector& vPoint ) const;

	// Constructs plane from three points on the plane.
			void		Set( const CVector& vPoint0, const CVector& vPoint1, const CVector& vPoint2 );

	// Flips plane normal and constant.
	inline	void		Reverse();

	// Returns on which side of the plane a point is ( front, back or aligned with the plane ).
	//
	// Parameters :
	//	vPoint			Point to test against the plane.
	//
	// Returns:			One of the values enumerated in ePlaneSides.
	inline	ePlaneSides	GetWhichSide( const CVector& vPoint ) const;

	// Equivalence operators
	inline bool		operator == ( const CPlane& Plane ) const;
    inline bool		operator != ( const CPlane& Plane ) const;
	
	// Returns true if Plane is equal to a flipped version of the current plane
	inline bool		EqualToReverse( const CPlane& Plane ) const;

};

// Bounding sphere class

class CSphere
{

public:

	CVector		m_vPosition;
	float		m_fRadius;

	// Grow the current sphere to include the sphere given as a pareeters.
	//
	// Parameters:
	//	Sphere		Sphere to merge with.
	void Merge( const CSphere& Sphere );

};

// Inlines

//-------------------------------------------------------------------------------------------------
inline CVector::CVector()
{ 
};

//-------------------------------------------------------------------------------------------------
inline CVector::CVector( float fVx, float fVy, float fVz )
{ 
	fX = fVx; fY = fVy; fZ = fVz;				
};

//-------------------------------------------------------------------------------------------------
inline bool	CVector::operator == ( const CVector& vVec ) const	
{
	return ( IS_FLOAT_EQUAL( fX, vVec.fX ) && IS_FLOAT_EQUAL( fY, vVec.fY ) && IS_FLOAT_EQUAL( fZ, vVec.fZ ) );
};

//-------------------------------------------------------------------------------------------------
inline bool	CVector::operator != ( const CVector& vVec ) const	
{
	return !(*this == vVec);
};

//-------------------------------------------------------------------------------------------------
inline CVector CVector::operator - ( const CVector& vVec ) const	
{ 
	return CVector( fX - vVec.fX, fY - vVec.fY, fZ - vVec.fZ );
};

//-------------------------------------------------------------------------------------------------
inline CVector CVector::operator - () const	
{ 
	return CVector( -fX, -fY, -fZ );
};

//-------------------------------------------------------------------------------------------------
inline CVector CVector::operator + ( const CVector& vVec ) const	
{
	return CVector( fX + vVec.fX, fY + vVec.fY, fZ + vVec.fZ );
};

//-------------------------------------------------------------------------------------------------
inline float CVector::operator * ( const CVector& vVec ) const		
{ 
	return fX * vVec.fX + fY * vVec.fY + fZ * vVec.fZ;
}

//-------------------------------------------------------------------------------------------------
inline CVector CVector::operator * ( float fScalar ) const
{ 
	return CVector( fScalar * fX, fScalar * fY, fScalar * fZ );
};

//-------------------------------------------------------------------------------------------------
inline CVector CVector::operator / ( float fScalar ) const		
{
	float fInvS = 1.0f / fScalar;
	return CVector( fInvS * fX, fInvS * fY, fInvS * fZ );
};

//-------------------------------------------------------------------------------------------------
inline float& CVector::operator[] ( DWORD uIndex ) const
{
	const float* pBase = &fX;
	return (float&)pBase[uIndex];
}

//-------------------------------------------------------------------------------------------------
inline float CVector::Length() const
{
	return (float)sqrtf( fX * fX + fY * fY + fZ * fZ );
};

//-------------------------------------------------------------------------------------------------
inline CVector CVector::CrossProduct( const CVector& vVec ) const	
{ 
	return CVector( fY * vVec.fZ - fZ * vVec.fY, fZ * vVec.fX - fX * vVec.fZ, fX * vVec.fY - fY * vVec.fX );	
};

//-------------------------------------------------------------------------------------------------
inline float CVector::Normalize()
{
    float fLength = Length();

    if ( fLength > 0.00000001f ) //EPSILON )
    {
        float fRecip = 1.0f / fLength;
        fX *= fRecip; fY *= fRecip; fZ *= fRecip;
    }
    else
    {
        fX = 0.0f; fY = 0.0f; fZ = 0.0f;
        fLength = 0.0f;
    }
    return fLength;
}

//-------------------------------------------------------------------------------------------------
inline CVector CVector::NormCrossProduct (const CVector& vVec ) const
{
    CVector cross( fY * vVec.fZ - fZ * vVec.fY, fZ * vVec.fX - fX * vVec.fZ, fX * vVec.fY - fY * vVec.fX );
	cross.Normalize();
	return cross;
}

//-------------------------------------------------------------------------------------------------
inline bool	CPlane::operator == ( const CPlane& Plane ) const
{
	return ( IS_FLOAT_EQUAL( Plane.m_fConstant, m_fConstant ) && Plane.m_vNormal == m_vNormal );
}

//-------------------------------------------------------------------------------------------------
inline bool	CPlane::operator != ( const CPlane& Plane ) const
{
	return !(*this == Plane);
}

//-------------------------------------------------------------------------------------------------
inline bool	CPlane::EqualToReverse( const CPlane& Plane ) const
{
	return	IS_FLOAT_EQUAL( Plane.m_fConstant, -m_fConstant ) &&
			IS_FLOAT_EQUAL( Plane.m_vNormal.fX, -Plane.m_vNormal.fX ) &&
			IS_FLOAT_EQUAL( Plane.m_vNormal.fY, -Plane.m_vNormal.fY ) &&
			IS_FLOAT_EQUAL( Plane.m_vNormal.fZ, -Plane.m_vNormal.fZ );
}

//-------------------------------------------------------------------------------------------------
inline void CVector::Set( float fVx, float fVy, float fVz )		
{
	fX = fVx; fY = fVy; fZ = fVz;	
}

//-------------------------------------------------------------------------------------------------
inline float CPlane::GetDistance( const CVector& vPoint ) const
{ 
	return m_vNormal * vPoint - m_fConstant;	
}

//-------------------------------------------------------------------------------------------------
inline CPlane::ePlaneSides CPlane::GetWhichSide( const CVector& vPoint ) const
{
    float fDistance =	m_vNormal.fX * vPoint.fX + 
						m_vNormal.fY * vPoint.fY + 
						m_vNormal.fZ * vPoint.fZ - m_fConstant;

    if ( fDistance < -EPSILON )		
		return epsBACK;
    else if ( fDistance > EPSILON )	
		return epsFRONT;
    else								
		return epsALIGNED;
}

//-------------------------------------------------------------------------------------------------
inline void CPlane::Reverse()
{
	m_vNormal	= -m_vNormal;
	m_fConstant	= -m_fConstant;
}

#endif	// _MATH_H