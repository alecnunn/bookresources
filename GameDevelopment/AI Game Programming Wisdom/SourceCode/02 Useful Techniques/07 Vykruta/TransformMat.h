/////////////////////////////////////////////////////////////////////////////
// TransformMat.h : vector definitions, constant defines, etc.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _TRANSFORMMAT_H_
#define _TRANSFORMMAT_H_

#include <math.h>

#define ELEMWID 20
#define rELEMWID 20.0f

#define SIGN(n)		(((n) > 0) ? 1 : (((n) < 0) ? (-1) : 0))
#define ABS(n)		(((n) < 0) ? (-(n)) : (n))
#define MIN(a,b)	(((a) < (b)) ? (a):(b))
#define MAX(a,b)	(((a) > (b)) ? (a):(b))


// 2D Vector class
class VECTOR2D
{
public:
	union
	{
		float dx;
		float x;
	};
	union
	{
		float dy;
		float y;
	};

public:
	// constructor
	VECTOR2D() {};

	//constructor
	VECTOR2D( float x, float y )
		{ dx = x; dy = y; }

	void Set(float x, float y)
		{ dx = x; dy = y; }

	// Set this equal to another
	VECTOR2D& operator=( const VECTOR2D& vRHS )
	{
		x = vRHS.x;
		y = vRHS.y;

		return( *this );
	}
	
	//negate a vector
	inline friend VECTOR2D operator - ( const VECTOR2D& v)
	{
		return VECTOR2D(-v.dx,-v.dy);
	}

	//increment by another vector
	void operator += ( const VECTOR2D& v )
		{ dx += v.dx; dy += v.dy; }

	//decrement by another vector
	void operator -= ( const VECTOR2D& v )
		{ dx -= v.dx; dy -= v.dy; }

	//multiply by a number
	void operator *= ( float n )
	{
		dx *= n;
		dy *= n;
	}

	//add two vectors
	VECTOR2D operator + ( const VECTOR2D& v ) const
	{ return VECTOR2D( dx + v.dx, dy + v.dy ); }

	//subtract two vectors
	VECTOR2D operator - ( const VECTOR2D& v ) const
	{ return VECTOR2D( dx - v.dx, dy - v.dy ); }

	//post-multiply by a scalar
	VECTOR2D operator * ( float n ) const
	{
		return VECTOR2D( dx * n, dy * n );
	}

	//pre-multiply by a scalar
	inline friend VECTOR2D operator * ( float n, const VECTOR2D& v )
	{
		return VECTOR2D( v.dx * n, v.dy * n );
	}
};

class VECTOR3D; // forward dec.

// 3D Vector class
class VECTOR3D
{
public:
	union
	{
		float dx;
		float x;
	};
	union
	{
		float dy;
		float y;
	};
	union
	{
		float dz;
		float z;
	};
public:

	// constructor
	VECTOR3D() {};

	// constructor
	VECTOR3D(float x, float y, float z)
		{ dx = x; dy = y; dz = z; }

	VECTOR3D( const VECTOR3D& vPos )
		{ dx = vPos.x; dy = vPos.y; dz = vPos.z; }

	void Set(float x, float y, float z)
		{ dx = x; dy = y; dz = z; }

	// Set this equal to another
	VECTOR3D& operator=( const VECTOR3D& vRHS )
	{
		x = vRHS.x;
		y = vRHS.y;
		z = vRHS.z;

		return( *this );
	}

	// Equality test
	BOOL operator == (const VECTOR3D &v) const
		{ return(dx == v.dx && dy == v.dy && dz == v.dz); }

	// Increment by another vector
	void operator += (const VECTOR3D &v)
		{ dx += v.dx; dy += v.dy; dz += v.dz; }

	// Decrement by another vector
	void operator -= (const VECTOR3D &v)
		{ dx -= v.dx; dy -= v.dy; dz -= v.dz; }

	// Increment by a float
	void operator += (const float f)
		{ dx += f; dy += f; dz += f; }

	// Decrement by a float
	void operator -= (const float f)
		{ dx -= f; dy -= f; dz -= f; }

	// Multiply by a number
	void operator *= (const float n)
	{
		dx = dx * n;
		dy = dy * n;
		dz = dz * n;
	}

	// boolean != operator
	BOOL operator != (const VECTOR3D v) const
		{ return(dx != v.dx || dy != v.dy || dz != v.dz); }

};


// The idea behind this class, is to take a arbitrary 2d line, and transform it such that it has a slope between 0 and 0.5
class TransformMatrix
{
public:
	enum eGridTransMatrix {tInvertXY,tNegateX,tNegateY,tNum};

	TransformMatrix(const VECTOR2D& vOrigin, const VECTOR2D& vDirVec)
	{	
		vMyOrigin = vDirVec;
		InitMatrix(vDirVec);	
	};
	// Transforms to and from local/global grid space. order of operations is important here!
	// Use templates, because we'll be passing in floats and ints, and ftoi is very expensive.
	template <class tGM1>
	inline void TransformToGridSpace(tGM1& x, tGM1& y)
	{
		if (matrix[tInvertXY] )
		{
			tGM1 iTmp = x;
			x = y;
			y = iTmp;
		}
		if (matrix[tNegateX] )
			x = -x;
		if (matrix[tNegateY] )
			y = -y;
	}
	template <class tGM2>
	inline void TransformToRealSpace(tGM2& x, tGM2& y)
	{
		if (matrix[tNegateX] )
			x = -x;
		if (matrix[tNegateY] )
			y = -y;
		if (matrix[tInvertXY] )
		{
			tGM2 iTmp = x;
			x = y;
			y = iTmp;
		}
	}
// Documentation for TransformOffsetToRealSpace:
// Given an offset to a point (can be 0,0  1,0  0,1  1,0  1,1) returned a transformed offset
// there are 8 possible cases.. here is a chart.  The function, given input, gives output
// INPUT ||      MATRIX          ||  OUTPUT
// X | Y || NegX | NegY | FlipXY ||  X | Y
// 1   0       0     0        0      1   0
// 1   0       0     1        0      1   1
// 1   0       1     0        1      0   0
// 1   0       1     1        1      1   0
// 1   0       1     1        0      0   1
// 1   0       1     0        0      0   0
// 1   0       0     1        1      1   1
// 1   0       0     0        1      0   1
	inline void TransformOffsetToRealSpace(int& x, int& y)
	{
		// if NegX is true, flip the state of X (can be 0 or 1)
		// if NetY is true, flip the satte of Y (can be 0 or 1)
		// if FlipXY is true, reverse thea nswer (easier than reversing both beforehand)
		x ^= matrix[tNegateX];
		y ^= matrix[tNegateY];
		if (matrix[tInvertXY])
		{
			int temp = x;
			x = y;
			y = temp;
		}
	}

	void InitMatrix(const VECTOR2D& vDirVec) // given a world-vector, create a local transform
	{
		matrix[tInvertXY] = fabs(vDirVec.y) > fabs(vDirVec.x);
		if (!matrix[tInvertXY])
		{
			matrix[tNegateX]  = vDirVec.x < 0;
			matrix[tNegateY]  = vDirVec.y < 0;
		}
		else
		{
			matrix[tNegateX]  = vDirVec.y < 0;
			matrix[tNegateY]  = vDirVec.x < 0;
		}
	}

	BOOL operator [] (const eGridTransMatrix e)
		{	return(matrix[e]);	};
private:
public:
	BOOL     matrix[tNum];
	VECTOR2D vMyOrigin;
};

// very simple landscape-chunk
class Layer
{
public:
	Layer(const int iXOrig, const int iZOrig, const int iWidth, const int iHeight, const float rYOrig) : 
								m_iXOrig(iXOrig), m_iZOrig(iZOrig), m_iWidth(iWidth), m_iHeight(iHeight), m_rYOrig(rYOrig)
	{
	}
	
	const int	GetXOrigin() const 
		{	return(m_iXOrig);	}
	const int	GetZOrigin() const 
		{	return(m_iZOrig);	}
	const float	GetYOrigin() const 
		{	return(m_rYOrig);	}
	const VECTOR3D GetOrigin() const
		{	return( VECTOR3D(0,0,0));	}
	bool DoesElemCollideRay(const VECTOR3D &vFrom, const VECTOR3D &vTo, VECTOR3D* pvImpact) const;
	
	const float VertHeight(const int iX, const int iZ) const
		{ return(m_rYOrig);	} // assume a uniform height for all vertices for academic purposes

private:
	int m_iXOrig;
	int m_iZOrig;
	int m_iWidth;
	int m_iHeight;
	
	float m_rYOrig;
};

#endif // _TRANSFORMMAT_H
