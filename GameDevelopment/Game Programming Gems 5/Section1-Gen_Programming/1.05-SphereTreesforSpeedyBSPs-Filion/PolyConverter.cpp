#include "stdafx.h"
#include "PolyConverter.h"
#include "Polygon.h"
#include <vector>

int	ComparePlanes( const void* pA, const void* pB );

bool ConvertTrisToPolys(	DWORD dwTriCount, DWORD dwVertexCount, DWORD* pIndices, CVector* pVertices,
							std::vector<CPolygon*>& Polygons )
{
	DWORD* pCurTri = pIndices;
	for ( DWORD i = 0; i < dwTriCount; i++ )
	{
		CPolygon* pPolygon = new CPolygon();
		pPolygon->SetVertexCount( 3 );
		CPolygonVertex* pDestVertex = pPolygon->m_pVertices;

		pDestVertex->m_vPosition = pVertices[*pCurTri++];
		CPolygon::SnapVertex( pDestVertex->m_vPosition );
		pDestVertex = pDestVertex->m_pNext;

		pDestVertex->m_vPosition = pVertices[*pCurTri++];
		CPolygon::SnapVertex( pDestVertex->m_vPosition );
		pDestVertex = pDestVertex->m_pNext;

		pDestVertex->m_vPosition = pVertices[*pCurTri++];
		CPolygon::SnapVertex( pDestVertex->m_vPosition );

		pPolygon->ComputePlane();
		pPolygon->ComputeBounds();
		// If the normal is zero, presumably the polygon was too tiny or degenerate so just ignore it
		if ( IS_FLOAT_EQUAL( pPolygon->m_Plane.m_vNormal.Length(), 1.0f ) )
			Polygons.push_back( pPolygon );
		else delete pPolygon;
	}

	return true;
	/*
	DWORD uMerges = 0;

	// Find coplanar neighbor triangles and merge them

	// Allocated the temporary array to sort the triangles by facet planes
	CTriangle* pSortedTris = new CTriangle[ dwTriCount ];

	DWORD* pCurTri = pIndices;
	for( DWORD i = 0; i < dwTriCount; i++ )
	{
		pSortedTris[i].bValid = true;
		pSortedTris[i].Vertices[0] = pVertices[*pCurTri++];
		pSortedTris[i].Vertices[1] = pVertices[*pCurTri++];
		pSortedTris[i].Vertices[2] = pVertices[*pCurTri++];
	}

	// Sort the triangles by facet plane
	qsort( pSortedTris, dwTriCount, sizeof( CTriangle* ), ComparePlanes );

	// Triangles are sorted by plane - now, triangles that share the same facet plane are neighbours 
	// on the list
	for( DWORD i = 0; i < dwTriCount; i++ ) 
	{
		if( !pSortedTris[i].bValid )
			continue;

		DWORD j = i + 1;
		CTriangle* pTriangle = &pSortedTris[i];
		CPolygon* pPolygon = new CPolygon;
		// :TODO: Assign first triangle
		while( true )
		{
			if ( j >= dwTriCount )
				break;

			if( !pSortedTris[j].bValid ) 
			{
				j++;
				continue;
			}

			if( pTriangle->Plane != pSortedTris[j].Plane )
				break;

			// if( pPolygon->TryMerge( pSortedTris[j] ) )
			{
				pSortedTris[j].bValid = false;
				uMerges++;
			}
			j++;
		}

		pPolygon->ComputeBounds();
	}

	DWORD dwPolyCount = dwTriCount -= uMerges;

	delete[] pSortedTris;
	return uMerges > 0;
	*/
}

int ComparePlanes( const void* pA, const void* pB )
{
	CTriangle* pTriA = (CTriangle*)pA;
	CTriangle* pTriB = (CTriangle*)pB;

	if( pTriA->Plane.m_vNormal.fX < pTriB->Plane.m_vNormal.fX - NORMAL_EPSILON )
		return -1;
	else if( pTriA->Plane.m_vNormal.fX > pTriB->Plane.m_vNormal.fX + NORMAL_EPSILON )
		return 1;
	else if( pTriA->Plane.m_vNormal.fY < pTriB->Plane.m_vNormal.fY - NORMAL_EPSILON )
		return -1;
	else if( pTriA->Plane.m_vNormal.fY > pTriB->Plane.m_vNormal.fY + NORMAL_EPSILON )
		return 1;
	else if( pTriA->Plane.m_vNormal.fZ < pTriB->Plane.m_vNormal.fZ - NORMAL_EPSILON )
		return -1;
	else if( pTriA->Plane.m_vNormal.fZ > pTriB->Plane.m_vNormal.fZ + NORMAL_EPSILON )
		return 1;
	else if( pTriA->Plane.m_fConstant < pTriB->Plane.m_fConstant - EPSILON )
		return -1;
	else if( pTriA->Plane.m_fConstant > pTriB->Plane.m_fConstant + EPSILON )
		return 1;
	else
		return 0;
};