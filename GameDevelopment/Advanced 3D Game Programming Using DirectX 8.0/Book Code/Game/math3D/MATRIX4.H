/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: matrix4.h
 *    Desc: Declaration for a 4x4 homogenous matrix class
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _MATRIX4_H
#define _MATRIX4_H

#include "point3.h"
#include "point4.h"
#include "lineSeg3.h"
#include "plane3.h"

struct matrix4 
{

	/**
	 * we're using m[y][x] as our notation.
	 */
    union 
	{
        struct 
		{
            float	_11, _12, _13, _14;
            float	_21, _22, _23, _24;
            float	_31, _32, _33, _34;
            float	_41, _42, _43, _44;
        };
        float	m[4][4];
    };

	/**
	 * justification for a function this ugly:
	 * provides an easy way to initialize static matrix variables 
	 * like base matrices for bezier curves and the identity
	 */
	matrix4(
		float in11, float in12, float in13, float in14,
        float in21, float in22, float in23, float in24,
        float in31, float in32, float in33, float in34,
        float in41, float in42, float in43, float in44 )
	{
        _11 = in11; _12 = in12; _13 = in13; _14 = in14;
        _21 = in21; _22 = in22; _23 = in23; _24 = in24;
        _31 = in31; _32 = in32; _33 = in33; _34 = in34;
        _41 = in41; _42 = in42; _43 = in43; _44 = in44;
	}

	matrix4()
	{
		// Do nothing.  
	}


	void ToXRot( float theta );
	static matrix4 XRot( float theta );


	void ToYRot( float theta );
	static matrix4 YRot( float theta );


	void ToZRot( float theta );
	static matrix4 ZRot( float theta );


	void ToAxisAngle( const point3& inAxis, float angle );
	static matrix4 AxisAngle( const point3& inAxis, float angle );


	/**
	 * Do not confuse with Translate()
	 * Translation clears the matrix, making it a Translation matrix
	 */
	void ToTranslation( const point3& p );
	static matrix4 Translation( const point3& p );

	
	/**
	 * Note that scale is not a rigid-body transform
	 */
	void ToScale( const point3& scale );
	static matrix4 Scale( const point3& scale );

	
	void ToObjectLookAt( 
		const point3& loc, 
		const point3& lookAt, 
		const point3& inUp = point3(0,1,0) );
	static matrix4 ObjectLookAt( 
		const point3& loc, 
		const point3& lookAt, 
		const point3& inUp = point3(0,1,0) );

	
	void ToCameraLookAt( 
		const point3& loc, 
		const point3& lookAt, 
		const point3& inUp = point3(0,1,0) );
	static matrix4 CameraLookAt( 
		const point3& loc, 
		const point3& lookAt, 
		const point3& inUp = point3(0,1,0) );

	/**
	 * Computes the inverse of a rigid-body transform
	 */
	void ToInverse( const matrix4& in );
	static matrix4 Inverse( const matrix4& in );

	/**
	 * Make the matrix the identity matrix
	 */
	void MakeIdent();

	/**
	 * Rotate an existing matrix by a given yaw, pitch,
	 * and roll.
	 */
	void Rotate( float yaw, float pitch, float roll );
	
	/**
	 * These functions are incremental; 
	 * they take an existing matrix and modify it.
	 * rotation information stays.
	 */
	void Translate( const point3& amt );
	void Place( const point3& loc );

	const point3 GetLoc() const;

	/**
	 * GetAxis only returns the correct axis if we're talking about a matrix that
	 * transforms 'up' towards worldspace.  For example, this will return the 
	 * correct axis if called on a object->world matrix, but incorrect one if
	 * called on the world->camera matrix.  (If one needs the axes for such a 
	 * matrix, invert the world->camera transform to get a camera->world 
	 * transform)
	 */
	point3 GetAxis( int axis ) const;

	static const matrix4 Identity;
};



//==========--------------------------  matrix4 operators

/**
 * returns matrix4*matrix4
 * not inlined due to the size.
 */
matrix4 operator*(matrix4 const &a, matrix4 const &b);
void MatMult( const matrix4 &a, const matrix4& b, matrix4* result );


/**
 * Specialization for multiplication by a point3 structure.
 * in a mathematically perfect world, we would construct
 * a 1x4 matrix for the point, and then multiply the 1x4
 * and the 4x4 matrices toegether.  However, this performs
 * much better.
 */
inline const point3 operator*( const matrix4 &a, const point3 &b)
{
	return point3(
		b.x*a._11 + b.y*a._21 + b.z*a._31 + a._41,
		b.x*a._12 + b.y*a._22 + b.z*a._32 + a._42,
		b.x*a._13 + b.y*a._23 + b.z*a._33 + a._43
	);
};

inline const point3 operator*( const point3 &a, const matrix4 &b)
{
	return b*a;
};


/**
 * Specialization for multiplication by a point4 structure.
 */
inline const point4 operator*( const matrix4 &a, const point4 &b)
{
	return point4(
		b.x*a._11 + b.y*a._21 + b.z*a._31 + b.w*a._41,
		b.x*a._12 + b.y*a._22 + b.z*a._32 + b.w*a._42,
		b.x*a._13 + b.y*a._23 + b.z*a._33 + b.w*a._43,
		b.x*a._14 + b.y*a._24 + b.z*a._34 + b.w*a._44
	);
};

inline const point4 operator*( const point4 &a, const matrix4 &b)
{
	return b*a;
};


//returns matrix4*lineSeg3
inline lineSeg3 operator*( lineSeg3 const &b, matrix4 const &a )
{
	return lineSeg3( b.v0 * a, b.v1 * a );
};
//returns matrix4*lineSeg3
inline lineSeg3 operator*( matrix4 const &a, lineSeg3 const &b )
{
	return lineSeg3( b.v0 * a, b.v1 * a );
};



#endif //_MATRIX4_H