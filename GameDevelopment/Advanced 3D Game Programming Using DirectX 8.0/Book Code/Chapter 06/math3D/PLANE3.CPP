/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: plane3.cpp
 *    Desc: Implementation of several plane functions
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include <assert.h>

#include "plane3.h"
#include "point3.h"
#include "polygon.h"
#include "lineSeg3.h"

ePListLoc plane3::TestPList( point3 *list, int num ) const
{
	bool allfront=true, allback=true;

	ePointLoc res;

	for( int i=0; i<num; i++ )
	{
		res = TestPoint( list[i] );

		if( res == ptBack )
		{
			allfront = false;
		}
		else if( res == ptFront )
		{
			allback = false;
		}
	}
	if( allfront && !allback )
	{
		// All the points were either in front or coplanar
		return plistFront;
	}
	else if( !allfront && allback )
	{
		// All the points were either in back or coplanar
		return plistBack;
	}
	else if( !allfront && !allback )
	{
		// Some were in front, some were in back
		return plistSplit;
	}
	// All were coplanar
	return plistCoplanar;
}


eSphereLoc plane3::TestBSphere( const bSphere3& sphere ) const
{
	float dp = (sphere.m_loc * n) + d;

	if((dp - sphere.m_radius) > EPSILON)
	{
		// Sphere was completely in front
		return sphereFront;
	}
	if((dp + sphere.m_radius) < -EPSILON )
	{
		// Sphere was completely in back
		return sphereBack;
	}
	// Sphere was partially in each
	return sphereCoplanar; 
}


ePListLoc plane3::TestPoly( const polygon<point3> &poly ) const
{
	return TestPList( poly.pList, poly.nElem );
}


bool plane3::Clip( const polygon<point3> &in, polygon<point3> *out ) const
{
	assert( out ); // Make sure our pointer to the out polygon is valid
	assert( in.nElem > 2 ); // Make sure we're not passed a degenerate polygon

	int thisInd=in.nElem-1;
	int nextInd=0;

	ePointLoc thisRes = TestPoint( in.pList[thisInd] );
	ePointLoc nextRes;

	out->nElem = 0;

	for( nextInd=0; nextInd<in.nElem; nextInd++ ) 
	{

		nextRes = TestPoint( in.pList[nextInd] );

		if( thisRes == ptFront || thisRes == ptCoplanar )
		{
			// Add the point
			out->pList[out->nElem++] = in.pList[thisInd];
		}

		if( ( thisRes == ptBack && nextRes == ptFront ) ||
			( thisRes == ptFront && nextRes == ptBack ) )
		{
			// Add the split point
			out->pList[out->nElem++] = Split( in.pList[thisInd], in.pList[nextInd] );
		}

		thisInd = nextInd;
		
		thisRes = nextRes;
	}
	if( out->nElem >= 3 )
	{
		return true;
	}
	return false;
}


bool plane3::Split( polygon<point3> const &in, polygon<point3> *pFront, polygon<point3> *pBack ) const
{

	assert( pFront ); // Make sure our pointer to the out polygon is valid
	assert( pBack ); // Make sure our pointer to the out polygon is valid
	assert( in.nElem > 2 ); // Make sure we're not passed a degenerate polygon

	// Start with curr as the last vertex and next as 0. 
	pFront->nElem = 0;
	pBack->nElem = 0;

	int thisInd=in.nElem-1;
	int nextInd=0;

	ePointLoc thisRes = TestPoint( in.pList[thisInd] );
	ePointLoc nextRes;

	for( nextInd=0; nextInd<in.nElem; nextInd++) {

		nextRes = TestPoint( in.pList[nextInd] );

		if( thisRes == ptFront )
		{
			// Add the point to the front
			pFront->pList[pFront->nElem++] = in.pList[thisInd];
		}

		if( thisRes == ptBack )
		{
			// Add the point to the back
			pBack->pList[pBack->nElem++] = in.pList[thisInd];
		}

		if( thisRes == ptCoplanar ) 
		{ 
			// Add the point to both
			pFront->pList[pFront->nElem++] = in.pList[thisInd];
			pBack->pList[pBack->nElem++] = in.pList[thisInd];
		}

		if( ( thisRes == ptBack && nextRes == ptFront ) ||
			( thisRes == ptFront && nextRes == ptBack ) )
		{ 
			// Add the split point to both
			point3 split = Split( in.pList[thisInd], in.pList[nextInd] );
			pFront->pList[pFront->nElem++] = split;
			pBack->pList[pBack->nElem++] = split;
		}

		thisInd = nextInd;
		thisRes = nextRes;
	}
	if( pFront->nElem >= 3 && pBack->nElem >= 3 )
	{ 
		// Nothing ended up degenerate
		return true;
	}
	return false;
}


// Behavior is undefined if seg can't actually be split
bool plane3::Split( const lineSeg3 &in, lineSeg3 *front, lineSeg3 *back ) const
{
	point3 temp = Split( in.v0, in.v1 );

	if( TestPoint( in.v0 ) == ptFront )
	{
		front->v0 = in.v0;
		front->v1 = temp;
		back->v0 = temp;
		back->v1 = in.v1;
		return true;
	}
	else 
	{
		front->v0 = temp;
		front->v1 = in.v1;
		back->v0 = in.v0;
		back->v1 = temp;
		return true;
	}
	return false;
}

