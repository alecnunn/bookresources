// RadiosityCalc.cpp: implementation of the RadiosityCalc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RadiosityCalc.h"

#include <algorithm>

using namespace std;

cRadiosityCalc::sPolygon::sPolygon() : 
	m_pSPList( NULL ), 
	m_pPtList( NULL )
{

}

cRadiosityCalc::sPolygon::~sPolygon()
{
}

void cRadiosityCalc::sPolygon::CalcArea()
{
	m_area = ((m_v[1]-m_v[0])^(m_v[2]-m_v[0])).Mag();
}

void cRadiosityCalc::sPolygon::CalcCenter()
{
	m_center = m_v[0] + m_v[1] + m_v[2] + m_v[3];
	m_center /= 4.f;
}
void cRadiosityCalc::sPolygon::GenPlane()
{
	m_plane = plane3( m_v[0], m_v[1], m_v[2] );
}


void cRadiosityCalc::sPolygon::Subdivide( float targetArea, list< sPatch* >& inList )
{
	point3 uDir = m_v[1] - m_v[0];
	point3 vDir = m_v[3] - m_v[0];

	float uMag = uDir.Mag();
	float vMag = vDir.Mag();

	float targetDim = (float)sqrt( targetArea ); // the length of a side of our target square 

	int uDim = (int) ceil( uDir.Mag() / targetDim );
	int vDim = (int) ceil( vDir.Mag() / targetDim );

	m_nVerts = (uDim+1)*(vDim+1);
	//m_nTris = 2 * (uDim)*(vDim);
	m_nSubPatches = (uDim)*(vDim);

	// This memory should be released after the radiosity
	// simulator has run its course
	m_pPtList = new point3[ m_nVerts ]; 
	m_pSPList = new sPatch[ m_nSubPatches ];

	// build the list of points
	int u, v;
	float du, dv;
	point3* pCurrPt = m_pPtList;
	for( v=0; v< vDim+1; v++ )
	{
		if( v == vDim )
		{
			dv = 1.f;
		}
		else
		{
			dv = (float)v * targetDim / vMag;
		}
		for( u=0; u< uDim+1; u++ )
		{
			if( u == uDim )
			{
				du = 1.f;
			}
			else
			{
				du = (float)u * targetDim / uMag;
			}

			*pCurrPt = m_v[0] + (du * uDir) + (dv * vDir);

			pCurrPt++;
		}
	}

	// build the patch list.
	sPatch* pCurrSP = m_pSPList;
	for( v=0; v< vDim; v++ )
	{
		for( u=0; u< uDim; u++ )
		{
			// tesselating square [u,v]
			pCurrSP->m_v[0] = &m_pPtList[(uDim+1)*(v+0) + (u+0)];
			pCurrSP->m_v[1] = &m_pPtList[(uDim+1)*(v+0) + (u+1)];
			pCurrSP->m_v[2] = &m_pPtList[(uDim+1)*(v+1) + (u+1)];
			pCurrSP->m_v[3] = &m_pPtList[(uDim+1)*(v+1) + (u+0)];

			pCurrSP->m_area = 
				((*pCurrSP->m_v[1] - *pCurrSP->m_v[0]) ^
				 (*pCurrSP->m_v[3] - *pCurrSP->m_v[0])).Mag();

			pCurrSP->m_center = (*pCurrSP->m_v[0] + *pCurrSP->m_v[1] + *pCurrSP->m_v[2] + *pCurrSP->m_v[3]) / 4.f;

			pCurrSP->m_deltaRad = m_deltaRad;

			pCurrSP->m_intensity = 
				pCurrSP->m_area * 
				(pCurrSP->m_deltaRad.r + pCurrSP->m_deltaRad.g + pCurrSP->m_deltaRad.b);

			pCurrSP->m_plane = m_plane;
			pCurrSP->m_radiosity = m_radiosity;
			pCurrSP->m_reflect = m_reflect;
			pCurrSP->m_pParent = this;

			inList.push_back( pCurrSP );
			pCurrSP++;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cRadiosityCalc::cRadiosityCalc()
{
	m_stdArea = 1.f;
}

cRadiosityCalc::~cRadiosityCalc()
{

}

void cRadiosityCalc::Load( cFile &file )
{
	int nPatches;
	char currLine[255];

	// get the first line without comments on it.
	do 
	{
		file.ReadLine( currLine );
	} while( currLine[0] == '\n' || currLine[0] == '#' );

	sscanf( currLine, "%i", &nPatches );

	sPolygon* pPatch;
	for( int pIndex=0; pIndex<nPatches; pIndex++ )
	{
		pPatch = new sPolygon();

		// get a good line of text
		do 
		{
			file.ReadLine( currLine );
	} while( currLine[0] == '\n' || currLine[0] == '#' );

		sscanf( currLine, "%f %f %f %f", 
			&pPatch->m_radiosity.r, 
			&pPatch->m_radiosity.g, 
			&pPatch->m_radiosity.b, 
			&pPatch->m_reflect );
		pPatch->m_deltaRad = pPatch->m_radiosity;

		// we implicitly assume quads.
		for( int vIndex=0; vIndex<4; vIndex++ )
		{
			// get a good line of text
			do 
			{
				file.ReadLine( currLine );
			} while( currLine[0] == '\n' || currLine[0] == '#' );
			sscanf( 
				currLine, 
				"%f %f %f", 
				&pPatch->m_v[vIndex].x,
				&pPatch->m_v[vIndex].y,
				&pPatch->m_v[vIndex].z );
		}
		pPatch->GenPlane();
		pPatch->CalcCenter();
		pPatch->CalcArea();

		pPatch->Subdivide( m_stdArea, m_patchList );
		
		m_polyList.push_back( pPatch );

		// add the patch to the tree
		polygon<point3> poly(4);
		poly.nElem = 4;
		poly.pList[0] = pPatch->m_v[0];
		poly.pList[1] = pPatch->m_v[1];
		poly.pList[2] = pPatch->m_v[2];
		poly.pList[3] = pPatch->m_v[3];
		m_tree.AddPolygon( poly );
	}

	m_startTime = timeGetTime(); 
}

void cRadiosityCalc::Draw()
{
	LPDIRECT3DDEVICE8 pDevice = Graphics()->GetDevice();
	list<sPolygon*>::iterator iter;

	sPolygon* pCurr;
	sLitVertex v[4];
	// init
	for( iter = m_polyList.begin(); iter != m_polyList.end(); iter++ )
	{
		pCurr = *iter;

		color3 col;

		sPatch* pCurrSP;
		for( int i=0; i<pCurr->m_nSubPatches; i++ )
		{
			pCurrSP = &pCurr->m_pSPList[i];

			v[0] = sLitVertex( *pCurrSP->m_v[0], pCurrSP->m_radiosity.MakeDWordSafe() );
			v[1] = sLitVertex( *pCurrSP->m_v[1], pCurrSP->m_radiosity.MakeDWordSafe() );
			v[2] = sLitVertex( *pCurrSP->m_v[2], pCurrSP->m_radiosity.MakeDWordSafe() );
			v[3] = sLitVertex( *pCurrSP->m_v[3], pCurrSP->m_radiosity.MakeDWordSafe() );
			
			pDevice->DrawPrimitiveUP( 
				D3DPT_TRIANGLEFAN,
				2,
				v,
				sizeof( sLitVertex ) );
		}
	}
}


bool cRadiosityCalc::LineOfSight( sPatch* a, sPatch* b )
{
	// Early-out 1: they're sitting on the same spot
	if( a->m_plane == b->m_plane )
		return false;

	// Early-out 2: b is behind a
	if( a->m_plane.TestPoint( b->m_center ) == ptBack )
		return false;

	// Early-out 3: a is behind b
	if( b->m_plane.TestPoint( a->m_center ) == ptBack )
		return false;

	// Compute the slow
	return m_tree.LineOfSight( a->m_center, b->m_center );
}

float cRadiosityCalc::FormFactor( sPatch *pSrc, sPatch *pDest ) 
{
	float   angle1, angle2, dist, factor;
	point3	vec;

	// find vij first.  If it's 0, we can early-out.
	if( !LineOfSight( pSrc, pDest ) )
		return 0.f;

	point3 srcLoc = pSrc->m_center;
	point3 destLoc = pDest->m_center;

	vec = destLoc - srcLoc;
	dist = vec.Mag();
	vec /= dist;

	angle1 = vec * pSrc->m_plane.n;
	angle2 = -( vec * pDest->m_plane.n );

	factor = angle1 * angle2 * pDest->m_area;
	factor /= PI * dist * dist;

	return factor;
}


cRadiosityCalc::sPatch* cRadiosityCalc::FindBrightest()
{
	sPatch* pBrightest = NULL;
	float brightest = 0.05f;

	float currIntensity;

	list<sPatch*>::iterator iter;

	// Blech. Linear search
	sPatch* pCurr;
	for( iter = m_patchList.begin(); iter != m_patchList.end(); iter++ )
	{
		pCurr = *iter;

		currIntensity = pCurr->m_intensity;

		if( currIntensity > brightest )
		{
			brightest = currIntensity;
			pBrightest = pCurr;
		}
	}

	// This will be NULL if nothing was bright enough
	return pBrightest;
}

bool cRadiosityCalc::CalcNextIteration()
{
	// Find the next patch that we need to 
	sPatch* pSrc = FindBrightest();

	// If there was no patch, we're done.
	if( !pSrc )
	{
		DWORD diff = timeGetTime() - m_startTime; 
		float time = (float)diff/1000;

		char buff[255];
		sprintf(buff,"Radiosity : Done - took %f seconds to render", time );
		SetWindowText( MainWindow()->GetHWnd(), buff );
		return false; // no more to calculate
	}

	sPatch* pDest;
	list<sPatch*>::iterator iter;

	float formFactor; // form factor Fi-j
	color3 deltaRad; // Incremental radiosity shot from src to dest

	for( iter = m_patchList.begin(); iter != m_patchList.end(); iter++ )
	{
		pDest = *iter;

		// Skip sending energy to ourself
		if( pDest == pSrc )
			continue;

		// Compute the form factor
		formFactor = FormFactor( pDest, pSrc );

		// Early out if the form factor was 0.
		if( formFactor == 0.f ) 
			continue;

		// Compute the energy being sent from src to dest
		deltaRad = pDest->m_reflect * pSrc->m_deltaRad * formFactor;

		// Send said energy
		pDest->m_radiosity += deltaRad;
		pDest->m_deltaRad += deltaRad;

		// Cache the new intensity.
		pDest->m_intensity = 
			pDest->m_area * 
			(pDest->m_deltaRad.r + 
			 pDest->m_deltaRad.g + 
			 pDest->m_deltaRad.b );

	}
	// this patch has shot out all of its engergy.
	pSrc->m_deltaRad = color3::Black; 
	pSrc->m_intensity = 0.f;

	return true;
}
