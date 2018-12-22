/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: RadiosityCalc.h
 *    Desc: class to calculate the radiosity solution for a scene
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _RADIOSITYCALC_H
#define _RADIOSITYCALC_H

#include "..\math3d\BspTree.h"
#include "..\math3d\tri.h"

#include <list>

// disable the annoying 255 character limit warning
#pragma warning( disable : 4786 )

class cRadiosityCalc  
{

	struct sPolygon;

	struct sPatch
	{
		point3* m_v[4];
		point3  m_center; // center of the subpatch (average of the 4 points)
		float	m_area;

		color3 m_radiosity; // radiosity at this point
		color3 m_deltaRad; // 'unshot' radiosity

		float m_reflect; // how reflective we are

		float m_intensity; // cached intensity for FindBrightest

		plane3 m_plane;

		sPolygon* m_pParent;

		float CalcIntensity()
		{
			return m_area * (m_deltaRad.r + m_deltaRad.g + m_deltaRad.b);
		}
	};

	struct sPolygon
	{
		point3* m_pPtList; 
		sPatch* m_pSPList;

		int m_nVerts;
		int m_nSubPatches;

		point3 m_v[4]; // only works for quads

		color3 m_radiosity; // radiosity at this point
		float m_reflect; // how reflective we are
		color3 m_deltaRad; // 'unshot' radiosity
		
		plane3 m_plane;
		point3 m_center;
		float m_area;

		void CalcArea();
		void CalcCenter();
		void GenPlane();

		void UpdateBrightest();

		// pass in a list to populate
		void Subdivide( float targetArea, list< sPatch* >& inList );

		sPolygon();
		~sPolygon();
	};

	bool LineOfSight( sPatch* a, sPatch* b );
	float FormFactor( sPatch *pSrc, sPatch *pDest );

	std::list< sPolygon* >		m_polyList;
	std::list< sPatch* >		m_patchList;

	float	m_stdArea;

	DWORD m_startTime;

	sPatch* FindBrightest();

	cBspTree m_tree;

public:

	void Draw();
	void Load( cFile& file );

	bool CalcNextIteration();

	cRadiosityCalc();
	virtual ~cRadiosityCalc();
};

#endif //_RADIOSITYCALC_H
