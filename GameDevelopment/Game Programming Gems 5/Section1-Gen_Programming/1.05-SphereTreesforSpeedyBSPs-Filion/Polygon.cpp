//
// CPolygon.cpp
//

#include "stdafx.h"
#include "Polygon.h"
#include <float.h>

const DWORD	MAXPOLYVERTICES		= 128;

CPolygonVertex::CPolygonVertex() :
	m_pNext( NULL ),
	m_pPrev( NULL )
{
}

CPolygon::CPolygon() :
	m_uVertexCount	( 0		),
	m_pVertices		( NULL	),
	m_pPrev			( NULL	),
	m_pNext			( NULL	),
	m_pParentNode	( NULL	),
	m_pNextCoplanar	( NULL	)
{
}

CPolygon::~CPolygon()
{
	CPolygonVertex* pVertex = m_pVertices;
	for ( DWORD i = 0; i < m_uVertexCount; i++ )
	{
		CPolygonVertex* pNext = pVertex->m_pNext;
		delete pVertex;
		pVertex = pNext;
	}
}

void CPolygon::ComputeBounds()
{
	// Compute polygon bounds
	// Note: This is a simple version of the algorithm, it produces a correct, but non-optimal bounding sphere

	// Sphere center is average of vectors
	CVector vBoundPos;
	float  fBoundRadius;
	vBoundPos.Set( 0.0f, 0.0f, 0.0f );

	CPolygonVertex* pVertex = m_pVertices;
	for( DWORD i = 0; i < m_uVertexCount; i++ )
	{
		vBoundPos = vBoundPos + pVertex->m_vPosition;
		pVertex = pVertex->m_pNext;
	}
	vBoundPos = vBoundPos / (float)m_uVertexCount;

	// Make bounding sphere radius around calculated center
	fBoundRadius = 0.0f;
	pVertex = m_pVertices;
	for( DWORD i = 0; i < m_uVertexCount; i++ )
	{
		CVector vTemp = pVertex->m_vPosition - vBoundPos;
		float fRadius = vTemp * vTemp;			// Length squared

		if( fRadius > fBoundRadius )
			fBoundRadius = fRadius;
		pVertex = pVertex->m_pNext;
	}
	fBoundRadius = sqrtf( fBoundRadius );

	m_Bounds.m_fRadius = fBoundRadius;
	m_Bounds.m_vPosition = vBoundPos;
}

void CPolygon::ComputePlane()
{
	// Calculate facet plane
	m_Plane.Set(	m_pVertices->m_vPosition, 
					m_pVertices->m_pNext->m_vPosition,
					m_pVertices->m_pNext->m_pNext->m_vPosition );
	//ASSERT( IS_FLOAT_EQUAL( m_Plane.m_vNormal.Length(), 1.0f ) );

	// Snap axial planes for robustness
	/*
	for( DWORD i = 0; i < 3; i++ )
	{
		if( fabs( m_Plane.m_vNormal[i] - 1 ) < NORMAL_EPSILON )
		{
			m_Plane.m_vNormal.Set( 0.0f, 0.0f, 0.0f );
			m_Plane.m_vNormal[i] = 1;
			break;
		}
		if( fabs( m_Plane.m_vNormal[i] + 1 ) < NORMAL_EPSILON )
		{
			m_Plane.m_vNormal.Set( 0.0f, 0.0f, 0.0f );
			m_Plane.m_vNormal[i] = -1;
			break;
		}
	}
	*/
	//if ( fabs( m_Plane.m_fConstant - Round( m_Plane.m_fConstant ) ) < EPSILON )
	//	m_Plane.m_fConstant = (float)Round( m_Plane.m_fConstant );
	TestPlaneTolerance();
}

void CPolygon::CopyFrom( CPolygon* pPolygon )
{
	// Copy polygon into this one
	ASSERT( pPolygon );
	if( pPolygon == this )
		return;

	SetVertexCount( pPolygon->m_uVertexCount );
	CPolygonVertex* pSrc = pPolygon->m_pVertices;
	CPolygonVertex* pDest = m_pVertices;
	for( DWORD i = 0; i < pPolygon->m_uVertexCount; i++ )
	{
		*pDest	= *pSrc;
		pSrc	= pSrc->m_pNext;
		pDest	= pDest->m_pNext;
	}
	m_Plane		= pPolygon->m_Plane;
	m_Bounds	= pPolygon->m_Bounds;
}


CPolygon::ePolyPlaneSides CPolygon::GetSide( const CPlane& Plane, bool bAlignedIsFront, float fEpsilon )
{
	ePolyPlaneSides ePolySide = epsUNKNOWN;
	CPolygonVertex* pVertex = m_pVertices;
	for( DWORD i = 0; i < m_uVertexCount; i++, pVertex = pVertex->m_pNext )
	{
		CPlane::ePlaneSides eSide;
		float fDistance = Plane.GetDistance( pVertex->m_vPosition );

		if( fDistance < -fEpsilon )
			eSide = CPlane::epsBACK;
		else if( fDistance > fEpsilon )
			eSide = CPlane::epsFRONT;
		else
			eSide = CPlane::epsALIGNED;

		switch( eSide )
		{
			case CPlane::epsFRONT:
				{
					if( ePolySide == epsBACK )
						return epsBOTH;
					ePolySide = epsFRONT;
				}
				break;
			case CPlane::epsBACK:
				{
					if( ePolySide == epsFRONT )
						return epsBOTH;
					ePolySide = epsBACK;
				}
				break;
			case CPlane::epsALIGNED:
				{
					// Empty
				}
				break;
		}
	}
	if( ePolySide == epsUNKNOWN )
		ePolySide = bAlignedIsFront ? epsFRONT : epsALIGNED;

	return ePolySide;
}

void CPolygon::LinkAsPrev( CPolygon*& pNewNext, CPolygon*& pOldFirst )
{
	if( pNewNext == this )
		return;

	if( m_pPrev )
		m_pPrev->m_pNext = m_pNext;
	else
		pOldFirst = m_pNext;

	if( m_pNext )
		m_pNext->m_pPrev = m_pPrev;

	if( pNewNext ) 
	{	
		pNewNext->m_pPrev	= this;
		m_pNext			= pNewNext;
	}
	else 
	{
		m_pNext = NULL;
	}

	pNewNext		= this;
	m_pPrev	= NULL;
}

void CPolygon::Unlink( CPolygon*& pOldFirst )
{
	if( m_pPrev )
		m_pPrev->m_pNext = m_pNext;
	else
		pOldFirst = m_pNext;

	if( m_pNext )
		m_pNext->m_pPrev = m_pPrev;

	m_pNext = NULL;
	m_pPrev = NULL;
}

DWORD CPolygon::Split( const CPlane& Plane, CPolygon* pFront, CPolygon* pBack, float fEpsilon )
{
	ASSERT( this != pFront );
	ASSERT( this != pBack );

	// Clip polygon with plane
	double					fDistances[ MAXPOLYVERTICES ];
	CPlane::ePlaneSides	eSides[MAXPOLYVERTICES];
	DWORD						uCounts[ CPlane::epsMAXSIDES ];

	DWORD i;
	for( i = 0; i < CPlane::epsMAXSIDES; i++ )
		uCounts[i] = 0;

	// Classify individual vertices with respect to plane
	CPolygonVertex* pVertex = m_pVertices;
	for( i = 0; i < m_uVertexCount; i++, pVertex = pVertex->m_pNext )
	{
		fDistances[i] = Plane.GetDistance( pVertex->m_vPosition );

		if( fDistances[i] < -fEpsilon )
			eSides[i] = CPlane::epsBACK;
		else if( fDistances[i] > fEpsilon )
			eSides[i] = CPlane::epsFRONT;
		else 
			eSides[i] = CPlane::epsALIGNED;

		uCounts[ eSides[i] ]++;
	}
	fDistances[i]	= fDistances[0];
	eSides[i]		= eSides[0];

	// Look if the polygon is aligned with the plane.
	if( uCounts[ CPlane::epsALIGNED ] == m_uVertexCount )
	{
		pFront->CopyFrom( this );
		pBack->CopyFrom( this );
		return HAS_FRONT | HAS_BACK;
	}

	// Look if the polygon is completly behind the plane.
	if( uCounts[ CPlane::epsFRONT ] == 0 )
	{
		pFront->SetVertexCount( 0 );
		pBack->CopyFrom( this );
		return HAS_BACK;
	}

	// Look if the polygon is completely in front of the plane.
	if( uCounts[ CPlane::epsBACK ] == 0 )
	{
		pBack->SetVertexCount( 0 );
		pFront->CopyFrom( this );
		return HAS_FRONT;
	};

	// Straddles the plane - we must clip

	DWORD uFrontPointCount = 0;
	DWORD uBackPointCount = 0;

	// Precount resulting vertices
	for( i = 0; i < m_uVertexCount; i++ )
	{
		switch( eSides[i] )
		{
			case CPlane::epsALIGNED:
				{
					uBackPointCount++;
					uFrontPointCount++;
					continue;
				}
				break;
			case CPlane::epsFRONT:
				{
					uFrontPointCount++;
				}
				break;
			case CPlane::epsBACK:
				{
					uBackPointCount++;
				}
				break;
		}

		if( eSides[i + 1] == CPlane::epsALIGNED || eSides[i + 1] == eSides[i] ) 
			continue;

		uFrontPointCount++;
		uBackPointCount++;
	}

	pFront->SetVertexCount( uFrontPointCount );
	pBack->SetVertexCount( uBackPointCount );

	// Clip it
						pVertex			= m_pVertices;
	CPolygonVertex*	pBackVertex		= pBack->m_pVertices;
	CPolygonVertex*	pFrontVertex	= pFront->m_pVertices;
	for( i = 0; i < m_uVertexCount; i++, pVertex = pVertex->m_pNext )
	{
		CVector vP1 = pVertex->m_vPosition;
		switch( eSides[i] )
		{
			case CPlane::epsALIGNED:
				{
					pBackVertex->m_vPosition	= vP1;	
					pFrontVertex->m_vPosition	= vP1;
					pBackVertex = pBackVertex->m_pNext;
					pFrontVertex = pFrontVertex->m_pNext;
					continue;
				}
				break;
			case CPlane::epsFRONT:
				{
					pFrontVertex->m_vPosition = vP1;
					pFrontVertex = pFrontVertex->m_pNext;
				}
				break;
			case CPlane::epsBACK:
				{
					pBackVertex->m_vPosition = vP1;
					pBackVertex = pBackVertex->m_pNext;
				}
				break;
		}

		// Test if the current edge crosses the plane.
		if( eSides[i + 1] == CPlane::epsALIGNED || eSides[i + 1] == eSides[i] ) 
			continue;

		// Need to clip the edge.
		CVector vP2	= pVertex->m_pNext->m_vPosition;
		float	 fT		= (float)( fDistances[i] / ( fDistances[i] - fDistances[i + 1] ) );
		CVector vMid	= vP1 + ( vP2 - vP1 ) * fT;
		
		// Robustness tests
		float fDistToMid = Plane.GetDistance( vMid );
		ASSERT( IS_FLOAT_EQUAL_EPSILON( fDistToMid, 0.0f, EPSILON ) );
		fDistToMid = m_Plane.GetDistance( vP1 );
		ASSERT( IS_FLOAT_EQUAL_EPSILON( fDistToMid, 0.0f, EPSILON ) );
		fDistToMid = m_Plane.GetDistance( vP2 );
		ASSERT( IS_FLOAT_EQUAL_EPSILON( fDistToMid, 0.0f, EPSILON ) );
		
		pBackVertex->m_vPosition	= vMid;
		pBackVertex					= pBackVertex->m_pNext;
		pFrontVertex->m_vPosition	= vMid;
		pFrontVertex				= pFrontVertex->m_pNext;
	}

	// Returns flags
	DWORD uResult = 0;
	if( uFrontPointCount > 0 )
	{
		pFront->m_Plane = m_Plane;
		uResult |= HAS_FRONT;
	}
	if( uBackPointCount > 0 )
	{
		pBack->m_Plane = m_Plane;
		uResult |= HAS_BACK;
	}

	// Robustness tests
	pFront->TestPlaneTolerance();
	pBack->TestPlaneTolerance();

	return uResult;
}

bool CPolygon::SetVertexCount( DWORD uNewVertexCount )
{
	// Allocate space for uNewVertexCount vertices
	ASSERT( uNewVertexCount > 2 || uNewVertexCount == 0 );

	if( m_uVertexCount == 0 )
		m_pVertices = NULL;

	int sDiff = uNewVertexCount - m_uVertexCount;
	if( sDiff < 0 )
	{
		CPolygonVertex* pLast		= m_pVertices->m_pPrev;
		CPolygonVertex* pVertex	= pLast;
		for( int i = 0; i > sDiff; i-- )
		{
			CPolygonVertex* pPrev = pVertex->m_pPrev;
			delete pVertex;
			pVertex = pPrev;
		}
		if( uNewVertexCount > 0 )
		{
			pVertex->m_pNext		= m_pVertices;				// Relink in a cyclic fashion
			m_pVertices->m_pPrev	= pVertex;
		}
	}
	else if( sDiff > 0 )
	{
		for( int i = 0; i < sDiff; i++ )
		{
			CPolygonVertex* pVertex = new CPolygonVertex();

			if( m_pVertices ) 
			{
				pVertex->m_pPrev				= m_pVertices->m_pPrev;
				pVertex->m_pNext				= m_pVertices;
				m_pVertices->m_pPrev->m_pNext	= pVertex;
				m_pVertices->m_pPrev			= pVertex;
				ASSERT( pVertex->m_pPrev);
			}
			else 
			{
				m_pVertices = pVertex;
				pVertex->m_pNext = pVertex;
				pVertex->m_pPrev = pVertex;
			}
			ASSERT( pVertex->m_pNext );
			ASSERT( pVertex->m_pPrev );
		}
	}
	m_uVertexCount = uNewVertexCount;

	return true;
}

bool CPolygon::TestPlaneTolerance()
{
	// Test if facet plane alignes within a certain error margin with the polygon
	CPolygonVertex* pVertex = m_pVertices;
	for( DWORD i = 0; i < m_uVertexCount; i++, pVertex = pVertex->m_pNext )
	{
		float fDistance = m_Plane.GetDistance( pVertex->m_vPosition );
		if( !IS_FLOAT_EQUAL_EPSILON( fDistance, 0.0f, EPSILON ) )
		{
			ASSERT( false );
			return false;
		}
	}
	return true;
}