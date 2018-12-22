/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: bezier.h
 *    Desc: Bezier patch and object classes
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _BEZIER_H
#define _BEZIER_H

#include "stdafx.h"
#include "..\math3d\tri.h"

class cBezierObject;

#define sTri tri<short>

/**
 * Bezier Patch class
 */
class cBezierPatch
{
	// Size (width, height) of the patch.
	int			m_size; 
	
	// Control points for the patch
	point3		m_ctrlPoints[16];

	// holds the tesselated points
	sVertex*	m_vertList; 
	
	// list of all the triangle indices for triangle drawing.		
	sTri*		m_triList;

	// holds the u-dir tangents (for normal generation)
	point3*		m_uTangList; 
	// holds the v-dir tangents (for normal generation)
	point3*		m_vTangList; 

	static matrix4		m_basisMatrix;

	friend class cFwdDiffIterator;
	friend class cTangentIterator;
	friend class cBezierObject;

public:
	cBezierPatch(); // Default constructor
	~cBezierPatch();

	void Init( int size ); // size is the dimension we should init to

	void Tesselate(); // fill the vert list with tesselated values

	void Draw( bool bDrawNet );
	void Load( cFile& file );

};

/**
 * Bezier object (set of patches)
 */
class cBezierObject
{
	int				m_nPatches;
	cBezierPatch	*m_patchList; // our list of patches

public:

	// increment and decrement the level of patch tesselation in the object
	void IncTesselation()
	{

		for( int i=0; i< m_nPatches; i++ )
		{
			if( m_patchList[i].m_size < 10 )
				m_patchList[i].Init( m_patchList[i].m_size+1 );
			else
				m_patchList[i].Init( m_patchList[i].m_size+2 );
		}
	}
	void DecTesselation()
	{
		for( int i=0; i< m_nPatches; i++ )
		{
			if( m_patchList[i].m_size > 10 )
				m_patchList[i].Init( m_patchList[i].m_size-2 );
			else if( m_patchList[i].m_size > 3 )
				m_patchList[i].Init( m_patchList[i].m_size-1 );
		}
	}


	cBezierObject();
	~cBezierObject();

	void Load( char* filename );
	void Draw( const matrix4& mat, bool bDrawNet );
};


class cTangentIterator
{
	int		m_i; // our current step in the iteration
	int		m_nSteps; // the number of steps
	point4	m_p[3]; // for x, y, and z
	point3	m_cPts[4];

	point3	m_Q; // the point at the current iteration location

	void CalcQ()
	{
		float t = (float)m_i / (m_nSteps-1);
		point4 tVec( 3*t*t, 2*t, 1, 0 );
		point4 pVec;

		point4 temp = tVec * cBezierPatch::m_basisMatrix;
		m_Q.x = m_p[0] * temp; 
		m_Q.y = m_p[1] * temp; 
		m_Q.z = m_p[2] * temp; 
	}

public:
	cTangentIterator( int nSteps, point3 p1, point3 p2, point3 p3, point3 p4 )
	{
		m_cPts[0] = p1;
		m_cPts[1] = p2;
		m_cPts[2] = p3;
		m_cPts[3] = p4;

		m_nSteps = nSteps;
		m_p[0].Assign( p1.x, p2.x, p3.x, p4.x );
		m_p[1].Assign( p1.y, p2.y, p3.y, p4.y );
		m_p[2].Assign( p1.z, p2.z, p3.z, p4.z );
	}

	void Start()
	{
		m_i = 0;

		CalcQ();
	}

	bool Done()
	{
		return !(m_i<m_nSteps); 
	}

	point3& GetCurr()
	{
		return m_Q;
	}

	operator point3&()
	{
		return m_Q;
	}

	void CalcNext()
	{
		m_i++;
		CalcQ();
	}
};


class cFwdDiffIterator
{
	int		m_i; // our current step in the iteration
	int		m_nSteps; // the number of steps

	point3	m_p[4]; // The 4 control points

	point3	m_Q; // the point at the current iteration location

	point3	m_dQ; // First derivative (initially at zero)
	point3	m_ddQ; // Second derivative (initially at zero)
	point3	m_dddQ; // Triple derivative (constant)

public:
	cFwdDiffIterator()
	{
		// Do nothing
	}
	cFwdDiffIterator( int nSteps, point3 p1, point3 p2, point3 p3, point3 p4 )
	{
		m_nSteps = nSteps;
		m_p[0] = p1;
		m_p[1] = p2;
		m_p[2] = p3;
		m_p[3] = p4;
	}

	void Start()
	{
		m_i = 0;

		float d = 1.f/(m_nSteps-1);
		float d2 = d*d; // d^2
		float d3 = d*d2;// d^3

		point4 px( m_p[0].x, m_p[1].x, m_p[2].x, m_p[3].x );
		point4 py( m_p[0].y, m_p[1].y, m_p[2].y, m_p[3].y );
		point4 pz( m_p[0].z, m_p[1].z, m_p[2].z, m_p[3].z );

		point4 cVec[3]; // <a, b, c, d> for x, y, and z.
		cVec[0] = px * cBezierPatch::m_basisMatrix;
		cVec[1] = py * cBezierPatch::m_basisMatrix;
		cVec[2] = pz * cBezierPatch::m_basisMatrix;

		m_Q = m_p[0];

		// Do the work for each component
		int i = 3;
		while (i--)
		{
			// remember that t=0 here so many of the terms 
			// in the text drop out.
			float a = cVec[i].v[0];
			float b = cVec[i].v[1];
			float c = cVec[i].v[2];
			// luckily d isn't used, which 
			// would clash with the other d.

			m_dQ.v[i] = a * d3 + b * d2 + c * d;
			m_ddQ.v[i] = 6 * a * d3 + 2 * b * d2;
			m_dddQ.v[i] = 6 * a * d3;
		}
	}

	bool Done()
	{
		return !(m_i<m_nSteps); 
	}

	point3& GetCurr()
	{
		return m_Q;
	}

	operator point3&()
	{
		return m_Q;
	}

	void CalcNext()
	{
		m_Q += m_dQ;
		m_dQ += m_ddQ;
		m_ddQ += m_dddQ;
		
		m_i++;
	}
};

#endif//_BEZIER_H