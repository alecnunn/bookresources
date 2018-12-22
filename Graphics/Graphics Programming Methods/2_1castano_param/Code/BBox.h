/*============================================================================
	Title: BBox.h
	Module: Pi/MathLib
	Author: Ignacio Castano
	Description: Axis Aligned Bounding Box Class.
	Changes:
		22/10/2000 - Ignacio Castano
			Math library started.

============================================================================*/

#ifndef _PI_BBOX_H_
#define _PI_BBOX_H_

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file BBox.h
 * Vector classes and functions.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

//#include "Core.h"			// for REAL
//#include "Templates.h"		// for piMin/piMax

//#include "MathLib.h"
//#include "BSphere.h"
//#include "FPU.h"

#include "PAR.h"


/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/

/** Axis Aligned Bounding Box class */
struct MATHLIB_API BBox {

	/** Default ctor. */
	BBox() { };

	/** Copy ctor. */
	BBox( const BBox & b ) : Mins(b.Mins), Maxs(b.Maxs) { }

	/** Copy the box */
	void Copy( const BBox & b ){
		Mins = b.Mins;
		Maxs = b.Maxs;
	}

	/** Returns a REAL pointer */
	operator REAL * () const { return (REAL *) this; }


	/** Build a cube centered on center and with edge = 2*dist */
	void Cube( const Vec3 & center, REAL dist ) {
		Mins.x = center.x - dist;
		Mins.y = center.y - dist;
		Mins.z = center.z - dist;
		Maxs.x = center.x + dist;
		Maxs.y = center.y + dist;
		Maxs.z = center.z + dist;
	}

	/** Clear the bounds. */
	void ClearBounds( void ) {
		Mins.x = Mins.y = Mins.z = PI_MAX_FLOAT;
		Maxs.x = Maxs.y = Maxs.z = PI_MIN_FLOAT;
	}

	/** Translate box. */
	inline void Translate( const BBox &box, const Vec3 &v ) {
		Mins.Add( box.Mins, v );
		Maxs.Add( box.Maxs, v );
	}

	/** Build a box, given center and extents. */
	void SetCenterExtents( const Vec3 &center, const Vec3 &extents ) {
		Mins.Sub( center, extents );
		Maxs.Add( center, extents );
	}

	/** Build a box, given center and extents. */
	void SetCenterExtents( const Vec3 &center, const Vec3 &xcenter, const Vec3 &extents ) {
		Mins.x = center.x + xcenter.x - extents.x;
		Mins.y = center.y + xcenter.y - extents.y;
		Mins.z = center.z + xcenter.z - extents.z;
		Maxs.x = center.x + xcenter.x + extents.x;
		Maxs.y = center.y + xcenter.y + extents.y;
		Maxs.z = center.z + xcenter.z + extents.z;
	}

	/** Add a point to this box. */
	void AddPointToBounds( const Vec3 & p ) {
		if( p.x < Mins.x ) Mins.x = p.x;
		if( p.x > Maxs.x ) Maxs.x = p.x;

		if( p.y < Mins.y ) Mins.y = p.y;
		if( p.y > Maxs.y ) Maxs.y = p.y;

		if( p.z < Mins.z ) Mins.z = p.z;
		if( p.z > Maxs.z ) Maxs.z = p.z;
	}

	/** Add a box to this box. */
	void AddBoxToBounds(const BBox & b) {
		if( b.Mins.x < Mins.x ) Mins.x = b.Mins.x;
		if( b.Maxs.x > Maxs.x ) Maxs.x = b.Maxs.x;

		if( b.Mins.y < Mins.y ) Mins.y = b.Mins.y;
		if( b.Maxs.y > Maxs.y ) Maxs.y = b.Maxs.y;

		if( b.Mins.z < Mins.z ) Mins.z = b.Mins.z;
		if( b.Maxs.z > Maxs.z ) Maxs.z = b.Maxs.z;
	}

	/** @todo Generic box on plane side test! */
	int OnPlaneSide( ) {
		//return BoxOnPlaneSide(mins, maxs, p);
		return 0;
	}

	/** Point inside box test. */
	bool PointInsideBox( const Vec3 & p ) const {
		if( Mins.x < p.x && Mins.y < p.y && Mins.z < p.z && 
			Maxs.x > p.x && Maxs.y > p.y && Maxs.z > p.z ) return true;
		return false;
	}

	/** Overlap test between boxes. */
	bool Overlap( const BBox & b ) const {
		if( Mins.x>=b.Maxs.x || Maxs.x<=b.Mins.x ) return false;
		if( Mins.y>=b.Maxs.y || Maxs.y<=b.Mins.y ) return false;
		if( Mins.z>=b.Maxs.z || Maxs.z<=b.Mins.z ) return false;
		return true;
	}


	/** 
	 * Overlap test between box and sphere. @a inside is set to true if the box is
	 * completely inside the sphere. This is usefull when testing the sphere against
	 * an AABB tree, because child boxes do not have to be tested.
	**/
/*	const bool Overlap( const BSphere & S, bool &inside ) const { 

		int i;
		REAL s, d = 0;

		// find the square of the distance
		// from the sphere to the box
		for( i=0; i<3; i++ ) {
			if( S.Center[i] < Mins[i] ) {
				s = S.Center[i] - Mins[i];
				d += s*s;
			}
			else if( S.Center[i] > Maxs[i] ) {
				s = S.Center[i] - Maxs[i];
				d += s*s;
			}
		}

	//	return d <= S.Radius*S.Radius;

		if( d>S.Radius*S.Radius ) return false;


		// determine if the box is inside the sphere:
		for( i=0; i<3; i++ ) {
			if( S.Center[i] < GetCenter(i) ) {
				s = S.Center[i] - Maxs[i];
				d += s*s;
			}
			else {
				s = S.Center[i] - Mins[i];
				d += s*s;
			}
		}

		inside = (d <= S.Radius*S.Radius);

		return true;
	}
*/

	/** 
	 * Segment - box overlap test.
	 * @param l		Line direction.
	 * @param mid	Midpoint of the line.
	 * @param hl	Segment half-length.
	 */
	bool Overlap( const Vec3 &l, const Vec3 &mid, REAL hl ) const {
		Vec3 C, E;
		GetCenter( C );
		GetExtents( E );
		Vec3 T;
		T.Sub( C, mid );

		/* ALGORITHM: Use the separating axis 
		theorem to see if the line segment 
		and the box overlap. A line 
		segment is a degenerate OBB. */

		if( fabs(T.x) > E.x + hl*fabs(l.x) )
		return false;

		if( fabs(T.y) > E.y + hl*fabs(l.y) )
		return false;

		if( fabs(T.z) > E.z + hl*fabs(l.z) )
		return false;

		/* NOTE: Since the separating axis is
		perpendicular to the line in these
		last four cases, the line does not
		contribute to the projection. */

		//l.cross(x-axis)?

		REAL r = E.y*fabs(l.z) + E.z*fabs(l.y);

		if( fabs(T.y*l.z - T.z*l.y) > r )
			return false;

		//l.cross(y-axis)?

		r = E.x*fabs(l.z) + E.z*fabs(l.x);

		if( fabs(T.z*l.x - T.x*l.z) > r )
			return false;

		//l.cross(z-axis)?

		r = E.x*fabs(l.y) + E.y*fabs(l.x);

		if( fabs(T.x*l.y - T.y*l.x) > r )
			return false;

		return true;

	}


	/** Return center on given axis. 0=X, 1=Y, 2=Z */
	REAL GetCenter( int axis ) const {
		return ( Mins[axis] + Maxs[axis] ) * 0.5f;
	}

	/** Get box center. */
	void GetCenter( Vec3 & center ) const {
		center.x = ( Mins.x + Maxs.x ) * 0.5f;
		center.y = ( Mins.y + Maxs.y ) * 0.5f;
		center.z = ( Mins.z + Maxs.z ) * 0.5f;
	}

	/** Return extents on given axis. */
	REAL GetExtents( int axis ) const {
		return fabs(Maxs[axis] - Mins[axis]) * 0.5f;
	}

	/** Return extents on given axis. */
	void GetExtents( Vec3 & extents ) const {
		extents.x = fabs(Maxs.x - Mins.x) * 0.5f;
		extents.y = fabs(Maxs.y - Mins.y) * 0.5f;
		extents.z = fabs(Maxs.z - Mins.z) * 0.5f;
	}


	/** Calculate the bounding cube of the box. */
	void CubeForBox(void) {
		Vec3 center;
		GetCenter(center);

		REAL f;
		f = piMin( Mins.x-center.x, Mins.y-center.y );
		f = piMin( f, Mins.z-center.z );
		Mins.x = center.x + f;
		Mins.y = center.y + f;
		Mins.z = center.z + f;
		Maxs.x = center.x - f;
		Maxs.y = center.y - f;
		Maxs.z = center.z - f;
	}

	/** Returns the farthest point of a box in the given dir. */
	void FarPoint( const Vec3 & dir, Vec3 & far_point ) const {
		if(dir.x<0) far_point.x = Mins.x;
		else far_point.x = Maxs.x;
		if(dir.y<0) far_point.y = Mins.y;
		else far_point.y = Maxs.y;
		if(dir.z<0) far_point.z = Mins.z;
		else far_point.z = Maxs.z;
	}

	/** Returns the nearest point of a box in the given dir. */
	void NearPoint( const Vec3 dir, Vec3 & near_point ) const {
		if(dir.x>0) near_point.x = Mins.x;
		else near_point.x = Maxs.x;
		if(dir.y>0) near_point.y = Mins.y;
		else near_point.y = Maxs.y;
		if(dir.z>0) near_point.z = Mins.z;
		else near_point.z = Maxs.z;
	}

	/** Calculate the minimum distance to the fardest face. */
	REAL MaxFaceDist( const Vec3 & point ) const {
		REAL dist=-99999;
		dist = piMax((REAL)fabsf(point.x-Mins.x), dist);
		dist = piMax((REAL)fabsf(point.y-Mins.y), dist);
		dist = piMax((REAL)fabsf(point.z-Mins.z), dist);
		dist = piMax((REAL)fabsf(point.x-Maxs.x), dist);
		dist = piMax((REAL)fabsf(point.y-Maxs.y), dist);
		dist = piMax((REAL)fabsf(point.z-Maxs.z), dist);
		return dist;
	}

	/** Increase the box size by @a padding. */
	void Pad( REAL padding ){
		Mins.x -= padding;
		Mins.y -= padding;
		Mins.z -= padding;
		Maxs.x += padding;
		Maxs.y += padding;
		Maxs.z += padding;
	}


	/** Calculate the bounds for a sliding box from start to end (speed box). */
	void SlideBounds( const BBox & b, const Vec3 & start, const Vec3 & end) {
		Mins.x = piMin( b.Mins.x + end.x, b.Mins.x + start.x );
		Mins.y = piMin( b.Mins.y + end.y, b.Mins.y + start.y );
		Mins.z = piMin( b.Mins.z + end.z, b.Mins.z + start.z );
		Maxs.x = piMax( b.Maxs.x + end.x, b.Maxs.x + start.x );
		Maxs.y = piMax( b.Maxs.y + end.y, b.Maxs.y + start.y );
		Maxs.z = piMax( b.Maxs.z + end.z, b.Maxs.z + start.z );
	}


	/** Calculate the bounds for a sliding box from center start to end (speed). */
	void SlideBounds( const Vec3 & center, const Vec3 & extents, const Vec3 & delta) {
		Mins.x = piMin( center.x - extents.x + delta.x, center.x - extents.x );
		Mins.y = piMin( center.y - extents.y + delta.y, center.y - extents.y );
		Mins.z = piMin( center.z - extents.z + delta.z, center.z - extents.z );
		Maxs.x = piMax( center.x + extents.x + delta.x, center.x + extents.x );
		Maxs.y = piMax( center.y + extents.y + delta.y, center.y + extents.y );
		Maxs.z = piMax( center.z + extents.z + delta.z, center.z + extents.z );
	}


	/** Calculate the minimum distance from the point to the box. */
	REAL DistanceSquared( const Vec3 & v ) const {
		
		Vec3 Near;

		if( v.x < Mins.x ) Near.x = Mins.x;
		else if( v.x > Maxs.x ) Near.x = Maxs.x;
		else Near.x = v.x;

		if( v.y < Mins.y ) Near.y = Mins.y;
		else if( v.y > Maxs.y ) Near.y = Maxs.y;
		else Near.y = v.y;

		if( v.z < Mins.z ) Near.z = Mins.z;
		else if( v.z > Maxs.z ) Near.z = Maxs.z;
		else Near.z = v.z;

		return SQ(Near.x-v.x) + SQ(Near.y-v.y) + SQ(Near.z-v.z);
	}


	/** 
	 * Actually, it only applies the translation.
	 * @todo recompute box after rotation!
	**/
/*	void Transform( const BBox &b, const Vec3 &pos, const Quat &rot ){
		Mins.x = b.Mins.x + pos.x;
		Mins.y = b.Mins.y + pos.y;
		Mins.z = b.Mins.z + pos.z;
		Maxs.x = b.Maxs.x + pos.x;
		Maxs.y = b.Maxs.y + pos.y;
		Maxs.z = b.Maxs.z + pos.z;
	}
*/

	/** Get the lenght of largest axis of the box. */
	REAL GetAbsMaxAxis( void ) {
		Vec3 ext;
		GetExtents(ext);
		REAL max, value;

		max = fabs(ext.x);

		value = fabs(ext.y);
		if(value > max) max = value;

		value = fabs(ext.z);
		if(value > max) max = value;

		return max;
	}

	/** 
	 * Ray AABB intersection.
	 *	Original code by Andrew Woo, from "Graphics Gems", Academic Press, 1990
	 *	Optimized code by Pierre Terdiman, 2000
	 */	
	bool TestRay( const Vec3 & origin, const Vec3 & dir, Vec3 &coord ) {
		bool Inside = true;
		Vec3 MaxT( -1, -1, -1 );

		// Find candidate planes.
		for( int i=0; i<3; i++ ) {
		
			if( origin[i] < Mins[i] ) {
				coord[i] = Mins[i];
				Inside = false;

				// Calculate T distances to candidate planes
				MaxT[i] = (Mins[i] - origin[i]) / dir[i];
			}
			else if( origin[i] > Maxs[i] ) {
				coord[i] = Maxs[i];
				Inside = false;

				// Calculate T distances to candidate planes
				MaxT[i] = (Maxs[i] - origin[i]) / dir[i];
			}
		}

		// Ray origin inside bounding box
		if( Inside ) return true;

		// Get largest of the maxT's for final choice of intersection
		int WhichPlane = 0;
		if( MaxT[1] > MaxT[WhichPlane])	WhichPlane = 1;
		if( MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

		// Check final candidate actually inside box
		if( IR(MaxT[WhichPlane])&SIGN_BITMASK ) return false;

		for( i=0; i<3; i++ ) {
			if( i!=WhichPlane ) {
				coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];
				if( coord[i]<Mins[i] || coord[i]>Maxs[i] )	return false;
			}
		}
		return true;	// ray hits box
	}


	/** Segment box intersection test. */
	bool TestSegment( const Vec3 & start, const Vec3 & end ) {
		
		Vec3 dir; dir.Sub( end, start );
		REAL hl = dir.NormalizeSlow() * 0.5f;
		Vec3 mid;
		mid.Mad( start, dir, hl );

		return Overlap( dir, mid, hl );
	}

	/** Extrude a box. */
	void Extrude( const Vec3 & center, REAL scale ) {
		Mins.x = (Mins.x - center.x) * scale + center.x;
		Mins.y = (Mins.y - center.y) * scale + center.y;
		Mins.z = (Mins.z - center.z) * scale + center.z;
		Maxs.x = (Maxs.x - center.x) * scale + center.x;
		Maxs.y = (Maxs.y - center.y) * scale + center.y;
		Maxs.z = (Maxs.z - center.z) * scale + center.z;
	}
	


	Vec3	Mins;		///< Min corner of the box.
	Vec3	Maxs;		///< Max corner of the box.
};







#endif // _PI_BBOX_H_
