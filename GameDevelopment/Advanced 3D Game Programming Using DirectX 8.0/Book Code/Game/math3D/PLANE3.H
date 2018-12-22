/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: plane3.h
 *    Desc: 3D Plane code
 *          Uses EQ Ax + By + Cz + D = 0
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _PLANE3_H
#define _PLANE3_H

#include "point3.h"
#include "bSphere3.h"
#include "polygon.h"

// Defines the three possible locations of a point in 
// relation to a plane
enum ePointLoc
{
	ptFront,
	ptBack,
	ptCoplanar
};

// Defines the four possible locations of a point list in 
// relation to a plane.  A point list is a more general
// example of a polygon.
enum ePListLoc
{
	plistFront,
	plistBack,
	plistSplit,
	plistCoplanar
};

enum eSphereLoc
{
	sphereFront, // Sphere was entirely in front
	sphereBack, // Sphere was entirely in back
	sphereCoplanar // Sphere was partially on either side
};

struct lineSeg3;

struct plane3 {

	point3 n; // Normal of the plane
	float d; // Distance along the normal to the origin 

	plane3( float nX, float nY, float nZ, float D) : n( nX, nY, nZ ), d( D )
	{
		// All done.
	}
	plane3( const point3& N, float D) : n( N ), d( D )
	{
		// All done.
	}

	// Construct a plane from three 3-D points
	plane3( const point3& a, const point3& b, const point3& c);

	// Construct a plane from a normal direction and a point on the plane
	plane3( const point3& norm, const point3& loc);

	// Construct a plane from a polygon
	plane3( const polygon<point3>& poly );

	plane3()
	{
		// Do nothing
	}

	ePointLoc TestPoint( const point3& point ) const;// tests a point against this plane
	ePListLoc TestPoly( const polygon<point3> &poly ) const;// can test lines, polys, and bboxes.
	ePListLoc TestPList( point3 *list, int num ) const;// can test lines, polys, and bboxes.
	eSphereLoc TestBSphere( const bSphere3& sphere ) const;

	const point3 Split( const point3 &a, const point3 &b ) const;

	const point3 BringToPlane( const point3& pt );

	// These functions all assume the output polygons have adequate space in them.
	bool Clip( const polygon<point3>& in, polygon<point3> *out ) const;
	bool Split( const polygon<point3>& in, polygon<point3> *front, polygon<point3> *back ) const;
	bool Split( const lineSeg3& in, lineSeg3 *front, lineSeg3 *back ) const;

	// Flip the orientation of the plane
	void Flip();
};

inline plane3::plane3( const point3& a, const point3& b, const point3& c)
{
	n = (b-a)^(c-a);
	n.Normalize();
	d = -(n*a);
}

inline plane3::plane3( const point3& norm, const point3& loc) :
    n( norm ), d( -(norm*loc) )
{
    // all done
}

inline plane3::plane3( const polygon<point3>& poly ) 
{
	point3 a = poly.pList[0];
	point3 b = poly.pList[1];
	point3 c = poly.pList[2];

	n = (b-a)^(c-a);
	n.Normalize();
	d = -(n*a);
}

// we're inlineing this because we do it constantly
inline ePointLoc plane3::TestPoint( point3 const &point ) const
{
	float dp = (point * n) + d;

	if(dp > EPSILON)
	{
		return ptFront;
	}
	if(dp < -EPSILON )
	{
		return ptBack;
	}
	return ptCoplanar; // it was between EP and -EP
}

inline const point3 plane3::Split( const point3 &a, const point3 &b ) const
{
	float aDot = (a * n);
	float bDot = (b * n);

	float scale = ( -d - aDot ) / ( bDot - aDot );

	return a + (scale * (b - a));
}


inline const point3 plane3::BringToPlane( const point3& pt )
{
	float distToPlane = (pt * n);
	return point3( pt - (d + distToPlane)*n );
}


void inline plane3::Flip()
{
	n = -n;
	d = -d;
}

//returns true if point3==point3
inline bool operator==(plane3 const &a, plane3 const &b)
{
	if( fabs(a.d-b.d) < EPSILON )
		if( a.n == b.n )
			return true;
	return false;
};

#endif /*_PLANE3_H*/