/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 7.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: ViewCone.h
 *    Desc: code to handle n-sided viewing cones
 *          (for portal rendering) 
 * copyright (c) 1999-2000 by Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "..\math3d\point3.h"
#include "..\math3d\plane3.h"

#define MAX_PLANES 32

class cViewCone  
{
	plane3	m_planes[MAX_PLANES];
	int		m_nPlanes;
	point3	m_camLoc;
	
	/**
	 * We need this functionality twice, encapsulate it
	 */
	void GenFromPoly( const point3& camLoc, const polygon< point3 >& in );

public:

	/**
	 * Default constructor/destructor
	 */
	cViewCone();

	/**
	 * Construct a frustum from an inputted polygon.  The polygon
	 * is assumed to wind clockwise from the point of view of the
	 * camera
	 */
	cViewCone( const point3& camLoc, const polygon< point3 >& in ); 

	/**
	 * Construct a frustum from the viewport data.  uses the 
	 * data to construct a cameraspace polygon, 
	 * back-transforms it to worldspace, then constructs a 
	 * frustum out of it.
	 */
	cViewCone( float fov, int width, int height, matrix4& viewMat );

	/**
	 * Clip a polygon to the frustum.  
	 * true if there was anything left
	 */
	bool Clip( 
		const polygon<point3>& in, 
		polygon<point3>* out );

	/**
	 * Get the center point of a frustum
	 * this is needed when we create frustums
	 * from other frustums
	 */
	const point3& GetLoc()
	{
		return m_camLoc;
	}

};

#endif // _FRUSTUM_H
