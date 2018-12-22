/*============================================================================
	Title: Vector.h
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: Vector classes and functions.
	Changes:
		22/10/2000 - Ignacio Castaño
			Math library started.

============================================================================*/

#ifndef _PI_VECTOR_H_
#define _PI_VECTOR_H_


/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file Vector.h
 * Vector classes and functions.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

//#include "Core.h"
//#include "Debug.h"
#include "PAR.h"		// instead of Core & Debug & MathLib
//#include "Archive.h"

//#include "MathLib.h"
//#include "Real.h"


/*----------------------------------------------------------------------------
	Globals:
----------------------------------------------------------------------------*/

extern MATHLIB_API struct Vec3	Vec3Origin;


/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/


/**
 * 2D vector. This class can also be used as a complex number.
**/
struct MATHLIB_API Vec2 {

	Vec2( void ) { };
	Vec2( REAL _x, REAL _y ) : x(_x), y(_y) { }
	Vec2( Vec2 & v ) { x=v.x; y=v.y; }

	void Set(REAL _x, REAL _y) { x=_x; y=_y; }
	void Copy(Vec2 & v) { x=v.x; y=v.y; }

	REAL Normalize(void){
		float isqr = InverseSqrt( x*x + y*y );
		x *= isqr;
		y *= isqr;
		return isqr;
	}

	/** Compute the product of two complex numbers. */
	void ComplexProduct( const Vec2 &a, const Vec2 &b ) {
		x = a.x*b.x - a.y*b.y;
		y = a.x*b.y + a.y*b.x;
	}

	/** Compute the division of two complex numbers. */
	void ComplexDivide( const Vec2 &a, const Vec2 &b ) {
		REAL r2 = SQ(b.x)+SQ(b.y);
		x = (a.x*b.x + a.y*b.y) / r2;
		y = (-a.x*b.y + a.y*b.x) / r2;
	}

	/** Compute the reciprocal of this complex number. */
	void ComplexReciprocal( void ) {
		REAL r2 = SQ(x)+SQ(y);
		x /= r2;
		y /= -r2;
	}

	/** Negate the 2d vector. */
	void Negate( void ) {
		x = -x;
		y = -y;
	}

	/** Add two 2d vectors. */
	void Add( const Vec2 &a, const Vec2 &b ) {
		x = a.x + b.x;
		y = a.y + b.y;
	}

	inline bool operator== (const Vec2 &v) { return x==v.x && y==v.y; }

	/** Vector serialization. */
//	friend PiArchive & operator<< ( PiArchive & Ar, Vec2 & v ) {
//		Ar << v.x << v.y;
//	}

	REAL x, y;
};


/**
 * 3D vector
**/
struct MATHLIB_API Vec3 {

	Vec3( void ) { }
	Vec3( REAL _x, REAL _y, REAL _z ) : x(_x), y(_y), z(_z) { }
	Vec3( const Vec2 & v ) { x=v.x; y=v.y; z=0; }
	Vec3( const Vec3 & v ) { x=v.x; y=v.y; z=v.z; }
	Vec3( const REAL * v ) { x=v[0]; y=v[1]; z=v[2]; }

	void Set(const REAL * v) { x=v[0]; y=v[1]; z=v[2]; }
	void Set(REAL _x, REAL _y, REAL _z) { x=_x; y=_y; z=_z; }
	void Copy(Vec3 & v) { x=v.x; y=v.y; z=v.z; }

	/** Cast operator, return a pointer to REAL. */
	operator REAL * () const { return (REAL *) this; }

	/** Normalizes the vector. To return the lenght or check for errors see NormalizeSlow. */
	inline void Normalize(void) {
		float isqr = InverseSqrt( x*x + y*y + z*z );
		x *= isqr;
		y *= isqr;
		z *= isqr;
	}

	/** Normalizes the vector and returns the original lenght. Checks for null vectors. */
	inline REAL NormalizeSlow( void ) {
		REAL l = sqrtf(x*x + y*y + z*z);
		if( l > PI_EPSILON ) {
			float il = 1/l;
			x *= il;
			y *= il;
			z *= il;
		}

		return l;
	}


	/** Return the lenght of the vector. @sa LengthSquared() */
	inline REAL Length( void ) const {
		return sqrtf( x*x + y*y + z*z );
	}

	/** Return the squared lenght of the vector. @sa Length() */
	inline REAL LengthSquared( void ) const {
		return x*x + y*y + z*z;
	}


	/** Negate in place. */
	inline void Negate(void) { x=-x; y=-y; z=-z; }

	inline void Sub( const Vec3 &v0, const Vec3 &v1 ) { x=v0.x-v1.x; y=v0.y-v1.y; z=v0.z-v1.z; }
	inline void Add( const Vec3 &v0, const Vec3 &v1 ) { x=v0.x+v1.x; y=v0.y+v1.y; z=v0.z+v1.z; }
	inline void Mul( const Vec3 &v0, const Vec3 &v1 ) { x=v0.x*v1.x; y=v0.y*v1.y; z=v0.z*v1.z; }
	inline void Div( const Vec3 &v0, const Vec3 &v1 ) { x=v0.x/v1.x; y=v0.y/v1.y; z=v0.z/v1.z; }
	inline void Mad( const Vec3 &v0, const Vec3 &v1, float s ) { x=v0.x+v1.x*s; y=v0.y+v1.y*s; z=v0.z+v1.z*s; }
	inline void Scale( const Vec3 &v0, float s ) { x=v0.x*s; y=v0.y*s; z=v0.z*s; }
	inline Vec3 &operator= (const Vec3 &v){ x=v.x; y=v.y; z=v.z; return *this; }
	inline void operator*= (float f){ x*=f; y*=f; z*=f; }
	inline void operator*= (const Vec3 &v){ x*=v.x; y*=v.y; z*=v.z; }
	inline void operator+= (float f){ x+=f; y+=f; z+=f; }
	inline void operator+= (const Vec3 &v){ x+=v.x; y+=v.y; z+=v.z; }
	inline void operator-= (float f){ x-=f; y-=f; z-=f; }
	inline void operator-= (const Vec3 &v){ x-=v.x; y-=v.y; z-=v.z; }
	inline void operator/= (float f){ x/=f; y/=f; z/=f; }
	inline void operator/= (const Vec3 &v){ x/=v.x; y/=v.y; z/=v.z; }


	/** Negate operator. 
	 * @warning this creates a vector on the stack and returns it by copy. 
	 * @sa Negate()
	**/
	inline Vec3 operator- (void) const { Vec3 temp(-x, -y, -z); return temp; }


	/** Cross product. */
	inline void Cross(const Vec3 &v0, const Vec3 &v1){
		x = v0.y*v1.z - v0.z*v1.y;
		y = v0.z*v1.x - v0.x*v1.z;
		z = v0.x*v1.y - v0.y*v1.x;
	}

	/** Blend two vectors with the factor a in the [0,1] range. */
	void Blend( const Vec3 &v0, const Vec3 &v1, REAL a ) {
		x = v0.x + a * (v1.x - v0.x);
		y = v0.y + a * (v1.y - v0.y);
		z = v0.z + a * (v1.z - v0.z);
	}

	/** Like Blend, but with the [-1,1] range. */
	void Mix( const Vec3 &v0, const Vec3 &v1, REAL a ) {
		a = a*0.5 + 0.5;
		x = v0.x + a * (v1.x - v0.x);
		y = v0.y + a * (v1.y - v0.y);
		z = v0.z + a * (v1.z - v0.z);
	}

	/** Returns the largest component of the vector (0=X, 1=Y, 2=Z). */
	int	LargestAxis( void ) const {
		const REAL * v = &x;
		int m = 0;
		if(v[1] > v[m]) m = 1;
		if(v[2] > v[m]) m = 2;
		return m;
	}

	/** Returns true if this vector is in front of @a p acording to direction @a d. */
	bool InFrontOf( const Vec3 & p, const Vec3 & n ) {
		Vec3 d;

		d.x = (x - p.x) * n.x;
		d.y = (y - p.y) * n.x;
		d.z = (z - p.z) * n.x;
		return (d.x + d.y + d.z) > 0;
	}

	/** Naive hash function. */
	inline uint32 GetHashValue( void ) const {
        const uint32 * h = (const uint32 *)(this);
        uint32 f = (h[0]+h[1]*11-(h[2]*17))&0x7fffffff;     // avoid problems with +-0
        return (f>>22)^(f>>12)^(f);
    }

	/** Vector serialization. */
//	friend PiArchive & operator<< ( PiArchive & Ar, Vec3 & v ) {
//		Ar << v.x << v.y << v.z;
//	}


	REAL x, y, z;
};


/**
 * 4D vector (3D vector y homogeneous coordinates)
**/
struct MATHLIB_API Vec4 {

	Vec4( void ) { }
	Vec4( REAL _x, REAL _y, REAL _z, REAL _w ) : x(_x), y(_y), z(_z), w(_w) { }
	Vec4( Vec2 & v ) { x=v.x; y=v.y; z=0; w=0; }
	Vec4( Vec3 & v ) { x=v.x; y=v.y; z=v.z; w=0; }
	Vec4( Vec4 & v ) { x=v.x; y=v.y; z=v.z; w=v.w; }

	void Set(REAL _x, REAL _y, REAL _z, REAL _w) { x=_x; y=_y; z=_z; w=_w; }
	void Copy(Vec4 & v) { x=v.x; y=v.y; z=v.z; w=v.w; }

	operator REAL * () const { return (REAL *) this; }

	/** Project the vector to the 3d plane, dividing by the fourth coordinate. */
	void Homogeinize( void ) {
		REAL iw = 1/w;
		x *= iw;
		y *= iw;
		z *= iw;
	}

	/** Vector serialization. */
//	friend PiArchive & operator<< ( PiArchive & Ar, Vec4 & v ) {
//		Ar << v.x << v.y << v.z << v.w;
//	}

	friend REAL Vec3DotProduct( const Vec3 &a, const Vec3 &b );
	friend Vec3 * Vec3Lerp( Vec3 * out, const Vec3 * v0, const Vec3 * v1, REAL s );

	REAL x, y, z, w;
};



/*----------------------------------------------------------------------------
	Functions:
----------------------------------------------------------------------------*/

/**
 * Linear interpolation. V1 + s(V2-V1)
**/
inline Vec3 * Vec3Lerp( Vec3 * out, const Vec3 * v0, const Vec3 * v1, REAL s ) {
    out->x = v0->x + s * (v1->x - v0->x);
    out->y = v0->y + s * (v1->y - v0->y);
    out->z = v0->z + s * (v1->z - v0->z);
    return out;
}


/**
 * 3d dot product.
**/
inline REAL Vec3DotProduct( const Vec3 &a, const Vec3 &b ) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}



#endif // _PI_VECTOR_H_
