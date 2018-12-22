/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: ViewCone.cpp
 *    Desc: code to handle n-sided viewing cones 
 *          (for portal rendering)
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include <assert.h>

#include <algorithm> // for swap()

#include "ViewCone.h"

using namespace std;


cViewCone::cViewCone()
: m_nPlanes( 0 )
, m_camLoc( point3::Zero )
{
	// Do nothing
}


cViewCone::cViewCone( const point3& camLoc, const polygon< point3 >& in )
{
	assert( in.nElem );
	assert( in.pList );
	GenFromPoly( camLoc, in );
}


cViewCone::cViewCone( float fov, int width, int height, matrix4& viewMat )
{
	/**
	 * This function is kind of a magic trick, as it tries to
	 * invert the projection matrix.  If you stare at the way
	 * we make projection matrices for long enough this should
	 * make sense.
	 */
	float aspect = ((float)height) / width;

	float z = 10;

	float w =  aspect * (float)( cos(fov/2)/sin(fov/2) );
	float h =   1.0f  * (float)( cos(fov/2)/sin(fov/2) );

	float x0 = -z/w;
	float x1 = z/w;
	float y0 = z/h;
	float y1 = -z/h;

	/**
	 * Construct a clockwise camera-space polygon
	 */
	polygon<point3> poly(4);
	poly.nElem = 4;
	poly.pList[0] = point3( x0, y0,z); // top-left
	poly.pList[1] = point3( x1, y0,z); // top-right
	poly.pList[2] = point3( x1, y1,z); // bottom-right
	poly.pList[3] = point3( x0, y1,z); // bottom-left

	/**
	 * Create a camspace->worldspace transform
	 */
	matrix4 camMatInv = matrix4::Inverse( viewMat );

	/**
	 * Convert it to worldspace
	 */
	poly.pList[0] = poly.pList[0] * camMatInv;
	poly.pList[1] = poly.pList[1] * camMatInv;
	poly.pList[2] = poly.pList[2] * camMatInv;
	poly.pList[3] = poly.pList[3] * camMatInv;

	/**
	 * Generate the frustum
	 */
	GenFromPoly( camMatInv.GetLoc(), poly );

}


void cViewCone::GenFromPoly( const point3& camLoc, const polygon< point3 >& in )
{
	int i;
	m_camLoc = camLoc;
	m_nPlanes = 0;
	for( i=0; i< in.nElem; i++ )
	{
		/**
		 * Plane 'i' contains the camera location and the 'ith'
		 * edge around the polygon
		 */
		m_planes[ m_nPlanes++ ] = plane3(
			camLoc,
			in.pList[(i+1)%in.nElem],
			in.pList[i] );
	}
}


bool cViewCone::Clip( const polygon<point3>& in, polygon<point3>* out )
{
	/**
	 * Temporary polygons.  This isn't thread safe
	 */
	static polygon<point3> a(32), b(32);
	polygon<point3>* pSrc = &a;
	polygon<point3>* pDest = &b;

	int i;

	/**
	 * Copy the input polygon to a.
	 */
	a.nElem = in.nElem;
	for( i=0; i<a.nElem; i++ )
	{
		a.pList[i] = in.pList[i];
	}

	/**
	 * Iteratively clip the polygons
	 */
	for( i=0; i<m_nPlanes; i++ )
	{
		if( !m_planes[i].Clip( *pSrc, pDest ) )
		{
			/**
			 * Faliure
			 */
			return false;
		}
		std::swap( pSrc, pDest );
	}

	/**
	 * If we make it here, we have a polygon that survived.
	 * Copy it to out.
	 */
	out->nElem = pSrc->nElem;
	for( i=0; i<pSrc->nElem; i++ )
	{
		out->pList[i] = pSrc->pList[i];
	}

	/**
	 * Success
	 */
	return true;
}


