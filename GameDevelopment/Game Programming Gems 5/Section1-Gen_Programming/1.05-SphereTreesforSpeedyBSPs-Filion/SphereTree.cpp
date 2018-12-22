//
// CSphereTree.cpp
//

#include "stdafx.h"
#include "SphereTree.h"
#include "Polygon.h"
#include "PolyConverter.h"
#include "Trace.h"

CSphereTreeNode::CSphereTreeNode():
 m_pNextFree	( NULL		),
 m_pNextAlloc	( NULL		),
 m_pPrevAlloc	( NULL		)
{
	static DWORD dwNextID = 0;
	m_dwID = dwNextID++;
	Reset();
}

void CSphereTreeNode::Reset()
{
	m_Bounds.m_fRadius = 0.0f;
	m_Bounds.m_vPosition.Set( 0.0f, 0.0f, 0.0f );
	m_pPolygons		= NULL;
	m_pParent		= NULL;
	m_dwPolyCount	= 0;
	m_pChildren[0]	= NULL;
	m_pChildren[1]	= NULL;
}

CSphereTree::CSphereTree():
	m_uNodeCount		( 0		),
	m_pFreeNodes		( NULL	),
	m_pLastAllocNode	( NULL	),
	m_pRoot				( NULL	),
	m_dwDepth			( 0		),
	m_dwLeaves			( 0		),
	m_fAvgDepth			( 0.0f	),
	m_pProgress			( NULL	),
	m_dwPolysProcessed	( 0		)
{
	ZeroMemory( &m_Bounds, sizeof( m_Bounds ) );
}

CSphereTree::~CSphereTree()
{
	Finalize();
}

void CSphereTree::Finalize()
{
	//for( std::vector< CSphereTreeNode* >::iterator i = m_Nodes.begin(); i != m_Nodes.end(); ++i )
	//	delete (*i);
	m_Nodes.clear();
	for( std::vector<CSphereTreeNode*>::iterator i = m_FreeBatches.begin(); i != m_FreeBatches.end(); ++i )
		delete[] (*i);

	m_FreeBatches.clear();
	m_pFreeNodes = NULL;

	for ( DWORD i = 0; i < m_Polygons.size(); i++ )
		delete m_Polygons[i];
	m_Polygons.clear();
	m_pLastAllocNode = NULL;
	m_uNodeCount		= 0;
	m_pFreeNodes		= 0;
	m_pLastAllocNode	= NULL;
	m_pRoot				= NULL;
	m_dwDepth			= 0;
	m_dwLeaves			= 0;
	m_fAvgDepth			= 0.0f;
	m_pProgress			= NULL;
	m_dwPolysProcessed	= 0;
}

void CSphereTree::AddTriangles( DWORD dwTriCount, DWORD dwVertexCount, DWORD* pIndices, CVector* pVertices )
{
	Finalize();
	ConvertTrisToPolys( dwTriCount, dwVertexCount, pIndices, pVertices, m_Polygons );
}

CSphereTreeNode* CSphereTree::AllocNode()
{
	if( !m_pFreeNodes )
	{
		m_pFreeNodes = new CSphereTreeNode[ 100 ];
		m_FreeBatches.push_back( m_pFreeNodes );

		for( DWORD i = 0; i < 100; i++ )
			m_pFreeNodes[i].m_pNextFree = &m_pFreeNodes[i + 1];
		m_pFreeNodes[99].m_pNextFree = NULL;
	}

	CSphereTreeNode* pNewNode	= m_pFreeNodes;
	m_pFreeNodes				= m_pFreeNodes->m_pNextFree;
	pNewNode->m_pNextAlloc		= m_pLastAllocNode;
	pNewNode->m_pPrevAlloc		= NULL;
	if( m_pLastAllocNode )
		m_pLastAllocNode->m_pPrevAlloc	= pNewNode;

	m_pLastAllocNode			= pNewNode;

	return pNewNode;
}

CSphereTreeNode* CSphereTree::Build( CProgressCtrl* pProgress )
{
	m_pProgress = pProgress;
	CPolygon* pLast = NULL;
	for ( DWORD i = 0; i < m_Polygons.size(); i++ )
	{
		CPolygon* pPolygon	= m_Polygons[i];
		CPolygon* pNext		= ( i < m_Polygons.size() - 1 ) ? m_Polygons[i + 1] : NULL;
		pPolygon->m_pNext	= pNext;
		pPolygon->m_pPrev	= pLast;
		pLast				= pPolygon;
	}
	m_dwPolysProcessed = 0;
	ComputeBounds();
	m_pRoot = BuildNode( m_Polygons[0], NULL, evcX, m_Bounds[0], m_Bounds[1], 1 );
	m_fAvgDepth /= m_dwLeaves;

	return m_pRoot;
}

CSphereTreeNode* CSphereTree::BuildNode(	CPolygon* pPolygons, CSphereTreeNode* pParentNode, eVectorComponents Axis, CVector vMin, CVector vMax,
											DWORD dwLevel )
{
	ASSERT( pPolygons );

	// Update stats
	if ( dwLevel > m_dwDepth )
		m_dwDepth = dwLevel;
	
	// Calculate sphere tree node bounding sphere - do this by calculating a bounding sphere that englobes all the polygons
	// in the polygon list passed as parameter
	// This bounding sphere is created by consecutively merging the individual polygons' bounding spheres
	// into a larger bounding sphere; inaccurate, but fast
	CPolygon*		pPolygon		= pPolygons;
	float			fMinimalExtent	= pPolygon->m_Bounds.m_fRadius;
	DWORD			dwPolyCount		= 1;							// Polygon count
	CSphere			Bounds			= pPolygon->m_Bounds;
					pPolygon		= (CPolygon*)pPolygon->m_pNext;
	//ASSERT( fMinimalExtent > EPSILON );

	// Compute actual sphere tree node bounding sphere (note that this is NOT the same as the subdivision area's bounding box)
	// The sphere tree nodes are very loose and actually overlap
	while( pPolygon )
	{
		Bounds.Merge( pPolygon->m_Bounds );

		// Minimal extent allowed for an sphere tree node is a box as small as the smallest polygon's bounding sphere
		// diameter
		if( pPolygon->m_Bounds.m_fRadius < fMinimalExtent )
			fMinimalExtent = pPolygon->m_Bounds.m_fRadius * 2.0f;

		dwPolyCount++;
		pPolygon = (CPolygon*)pPolygon->m_pNext;
	}
	if ( fMinimalExtent < 0.01f )
		fMinimalExtent = 0.01f;

	// Create sphere tree node
	CSphereTreeNode* pNode	= AllocNode();
	pNode->m_dwPolyCount	= dwPolyCount;
	pNode->m_Bounds			= Bounds;
	m_Nodes.push_back( pNode );
	m_uNodeCount++;
	//Trace( "SphereTree node 0x%.8X created\n", pNode );

	// Stop subdividing sphere tree when we have only one polygon left in that node or when the sphere tree node's area 
	// bounding box has an extent as small or smaller than the minimal allowed extent
	// Subdivide sphere tree

	// We will loop here several times if the sphere tree subdivision does not actually refine the sphere tree
	while( true )
	{
		CVector vExtents = vMax - vMin;
		// If we already are at the minimal allowed extent, nothing we could do could refine the sphere tree more
		// at this branch
		if(	!pPolygons->m_pNext || 
			( vExtents.fX <= fMinimalExtent && vExtents.fY <= fMinimalExtent && vExtents.fZ <= fMinimalExtent ) )
		{
			// SphereTree leaf
			pNode->m_pChildren[0] = NULL;
			pNode->m_pChildren[1] = NULL;
			
			// Link the polygons to the sphere tree leaf
			pNode->m_pPolygons	  = pPolygons;
			pPolygon = pPolygons;
			while( pPolygon )
			{
				pPolygon->m_pParentNode = pNode;
				pPolygon = pPolygon->m_pNext;
				m_dwPolysProcessed++;
			}
			m_fAvgDepth += dwLevel;
			m_dwLeaves++;
			m_pProgress->SetPos( m_dwPolysProcessed );
			return pNode;
		}
		else
		{
			// Find the threshold, minima & maxima of our subdivision boxes
			// Note that we are accessing the vector's components here, not accessing an array of vectors
			// This creates two subdivision boxes from the parent subdivison box
			// Box A has extents vMin, vBoxAMax
			// Box B has extents vBoxBMin, vMax
			float fThreshold = vMin[Axis] + ( vMax[Axis] - vMin[Axis] ) * 0.5f;
			CVector vBoxAMax = vMin;
			CVector vBoxBMin = vMax;
			vBoxAMax[Axis] = fThreshold;
			vBoxBMin[Axis] = fThreshold;
			static int temp = 0;
			temp++;

			// Optimization: Check if this sphere tree node actually refines the sphere tree
			
			pPolygon					= pPolygons;
			bool bHasFront				= false;
			bool bHasBack				= false;
			while( pPolygon )
			{
				// Classify the polygon's vertices as in front or behind the sphere tree node's splitting axial plane
				DWORD dwFrontCount = 0;
				CPolygonVertex* pVertex = pPolygon->m_pVertices;
				for( DWORD i = 0; i < pPolygon->m_uVertexCount; i++ )
				{
					if( pVertex->m_vPosition[ Axis ] < fThreshold )
						dwFrontCount++;
					pVertex = pVertex->m_pNext;
				}
				
				// See if we have polygons in both subdivision boxes
				// We want polygons classified into one, and only one of the sub-boxes
				// So just classify the polygon in the sub-box that contains the most of the polygon's vertices
				// See if we have more vertices on front side than on the back side
				if( dwFrontCount >= pPolygon->m_uVertexCount - dwFrontCount )
				{
					bHasFront = true;
					if( bHasBack )
						break;
				}
				else 
				{
					bHasBack = true;
					if( bHasFront )
						break;
				}

				pPolygon = (CPolygon*)pPolygon->m_pNext;
			}

			if( !( bHasBack && bHasFront ) )
			{
				// This sphere tree split does not refine the sphere tree (i.e. all polygons are on one side)
				// choose a finer subdivision box - this will eliminate quite a few redundant sphere tree branches

				// Choose next splitting axis: X ==> Y ==> Z ==> X...
				Axis = (eVectorComponents)(Axis + 1);
				if( Axis > evcZ )
					Axis = evcX;

				// Subdivide whichever sub-box had all the polygons
				if( bHasFront )
					vMax = vBoxBMin;
				else
					vMin = vBoxAMax;

				// Restart the subdivision process
				continue;
			}

			CPolygon* pNodeA	= NULL;
			CPolygon* pNodeB	= NULL;
			pPolygon			= pPolygons;
			while( pPolygon )
			{
				// Link the polygons to either the node A or node B
				CPolygon* pNext = pPolygon->m_pNext;

				// Classify the polygons
				BYTE dwFrontCount = 0;
				CPolygonVertex* pVertex = pPolygon->m_pVertices;
				for( BYTE i = 0; i < pPolygon->m_uVertexCount; i++ )
				{
					if( pVertex->m_vPosition[ Axis ] < fThreshold )
						dwFrontCount++;
					pVertex = pVertex->m_pNext;
				}
				
				// Link
				if( dwFrontCount >= pPolygon->m_uVertexCount - dwFrontCount ) 
					pPolygon->LinkAsPrev( pNodeA, pPolygons );
				else
					pPolygon->LinkAsPrev( pNodeB, pPolygons );

				pPolygon = pNext;
			}

			ASSERT( pNodeA );
			ASSERT( pNodeB );

			// Choose next splitting axis: X ==> Y ==> Z ==> X...
			Axis = (eVectorComponents)(Axis + 1);
			if( Axis > evcZ )
				Axis = evcX;

			// Build sphere tree children recursively
			pNode->m_pPolygons = NULL;
			pNode->m_pChildren[0] = BuildNode( pNodeA, pNode, Axis, vMin, vBoxBMin, dwLevel + 1 );
			pNode->m_pChildren[1] = BuildNode( pNodeB, pNode, Axis, vBoxAMax, vMax, dwLevel + 1 );

			// Set parent
			pNode->m_pChildren[0]->m_pParent = pNode;
			pNode->m_pChildren[1]->m_pParent = pNode;
			break;
		}
	}

	return pNode;
}

void CSphereTree::ComputeBounds()
{
	for ( DWORD i = 0; i < m_Polygons.size(); i++ )
	{
		// Update bounds
		CPolygon* pPolygon = m_Polygons[i];
		CPolygonVertex* pVertex = pPolygon->m_pVertices;
		for ( DWORD i = 0; i < pPolygon->m_uVertexCount; i++ )
		{
			if( pVertex->m_vPosition.fX < m_Bounds[0].fX )
				m_Bounds[0].fX = pVertex->m_vPosition.fX;
			if( pVertex->m_vPosition.fY < m_Bounds[0].fY )
				m_Bounds[0].fY = pVertex->m_vPosition.fY;
			if( pVertex->m_vPosition.fZ < m_Bounds[0].fZ )
				m_Bounds[0].fZ = pVertex->m_vPosition.fZ;
			if( pVertex->m_vPosition.fX > m_Bounds[1].fX )
				m_Bounds[1].fX = pVertex->m_vPosition.fX;
			if( pVertex->m_vPosition.fY > m_Bounds[1].fY )
				m_Bounds[1].fY = pVertex->m_vPosition.fY;
			if( pVertex->m_vPosition.fZ > m_Bounds[1].fZ )
				m_Bounds[1].fZ = pVertex->m_vPosition.fZ;
			pVertex = pVertex->m_pNext;
		}
		pPolygon = pPolygon->m_pNext;
	}
}

void CSphereTree::FreeNode( CSphereTreeNode* pNode )
{
	ASSERT( pNode );

	if( pNode == m_pLastAllocNode )
		m_pLastAllocNode = m_pLastAllocNode->m_pNextAlloc;
	if( pNode->m_pNextAlloc )
		pNode->m_pNextAlloc->m_pPrevAlloc = pNode->m_pPrevAlloc;
	if( pNode->m_pPrevAlloc )
		pNode->m_pPrevAlloc->m_pNextAlloc = pNode->m_pNextAlloc;
	pNode->m_pNextFree	= m_pFreeNodes;
	pNode->m_pPrevAlloc	= NULL;
	pNode->m_pNextAlloc	= NULL;
	m_pFreeNodes		= pNode;
	pNode->Reset();
}

DWORD CSphereTree::Split(	CPolygon* pSplitPolygon, CSphereTreeNode*& pFrontNode, CSphereTreeNode*& pBackNode )
{
	// Split the sphere tree into two separate sphere trees, (front & back sphere trees), using the pSplitPolygon's facet 
	// plane as the splitter
	DWORD uSplits = 0;

	float fDistance = pSplitPolygon->m_Plane.GetDistance( pFrontNode->m_Bounds.m_vPosition );
	if( fDistance > pFrontNode->m_Bounds.m_fRadius )
	{
		// Completely in front
		// Leave it as is, everything stays in the current (front) sphere tree
	}
	else if( fDistance < -pFrontNode->m_Bounds.m_fRadius )
	{
		// Completely in back - relink everything to back sphere tree
		BYTE ParentNodeIndex = pFrontNode->m_pParent ? pFrontNode->m_pParent->m_pChildren[1] == pFrontNode : NULL;
		pBackNode = pFrontNode;

		// :TRICKY: pFrontNode is a reference which will normally be a reference to its link from the parent, 
		// so setting it to null will clear the parent->child link of that node. Not exactly ideal, could
		// be reviewed
		// This is why we keep a backup
		CSphereTreeNode* pBackupNode = pFrontNode;
		pFrontNode = NULL;
		pBackupNode->m_pParent = pBackNode;
	}
	else
	{
		// Possibly straddles the plane
		if( pFrontNode->m_pPolygons )
		{
			// The sphere tree node straddles the plane and is a leaf
			// This is an sphere tree leaf, so we have to test the polygons individually
			CPolygon* pCurPolygon	= pFrontNode->m_pPolygons;
			DWORD dwFrontCount		= 0;
			DWORD dwBackCount		= 0;

			while( pCurPolygon )
			{
				CPolygon* pNextPoly = pCurPolygon->m_pNext;

				CPolygon::ePolyPlaneSides eSide = pCurPolygon->GetSide( pSplitPolygon->m_Plane, false, EPSILON );

				switch( eSide )
				{
					case CPolygon::epsFRONT:
						{
							// Front side
							dwFrontCount++;
						}
						break;
					case CPolygon::epsBACK:
						{
							// Back side
							// Allocate back sphere tree node if necessary
							if( !pBackNode )
							{
								pBackNode = AllocNode();
								pBackNode->m_Bounds			= pFrontNode->m_Bounds;
								pBackNode->m_pChildren[0]	= pBackNode->m_pChildren[1] = NULL;
								pBackNode->m_dwPolyCount	= 0;
								pBackNode->m_pPolygons		= NULL;
								//Trace( "Split: Back sphere tree node 0x%.8X created - polygon in back\n", pBackNode );
							}
							// Relink polygon to the back sphere tree node
							pCurPolygon->Unlink( pFrontNode->m_pPolygons );
							CPolygon* pTemp = NULL;
							pCurPolygon->LinkAsPrev( pBackNode->m_pPolygons, pTemp );
							pCurPolygon->m_pParentNode = pBackNode;
							dwBackCount++;
						}
						break;
					case CPolygon::epsBOTH:
						{
							// Straddles the plane, we need to split the polygon
							// Allocate back sphere tree node if necessary
							if ( !pBackNode )
							{
								pBackNode = AllocNode();
								pBackNode->m_Bounds			= pFrontNode->m_Bounds;
								pBackNode->m_pChildren[0]	= pBackNode->m_pChildren[1] = NULL;
								pBackNode->m_dwPolyCount	= 0;
								pBackNode->m_pPolygons		= NULL;
								//Trace( "Split: Back sphere tree node 0x%.8X created - polygon straddling plane\n", pBackNode );
							}

							// Allocate two new polygons
							CPolygon* pNewFront = new CPolygon;
							m_Polygons.push_back( pNewFront );
							CPolygon* pNewBack = new CPolygon;
							m_Polygons.push_back( pNewBack );
							pNewFront->m_Plane		= pCurPolygon->m_Plane;
							pNewBack->m_Plane		= pCurPolygon->m_Plane;
							uSplits++;

							pCurPolygon->Unlink( pFrontNode->m_pPolygons );

							// Split it
							pCurPolygon->Split( pSplitPolygon->m_Plane, pNewFront, pNewBack );
							CPolygon* pTemp = NULL;

							pNewFront->LinkAsPrev( pFrontNode->m_pPolygons, pTemp );
							pNewFront->m_pParentNode = pFrontNode;
							pNewFront->ComputeBounds();
							dwFrontCount++;

							pNewBack->LinkAsPrev( pBackNode->m_pPolygons, pTemp );
							pNewBack->m_pParentNode	 = pBackNode;
							pNewBack->ComputeBounds();
							dwBackCount++;
						}
						break;
					case CPolygon::epsALIGNED:
						{
							// This polygon is aligned with two planes at once... discard it, resplitting it wouldn't
							// add anything new to the tree
							// In theory, this should only happen with polygons so small that they can fit into a plane's
							// epsilon region in many different angles
							// :TODO: How will this affect solid/empty leaf classification later on ?
							// :TODO: Will this polygon leak ?
							//_ASSERT( pCurPolygon->IsTiny() );
							pCurPolygon->Unlink( pFrontNode->m_pPolygons );
						}
						break;
					default :
						{
							ASSERT( false );
						}
						break;
				}

				pCurPolygon = pNextPoly;
			}

			pFrontNode->m_dwPolyCount = dwFrontCount;
			if( pBackNode  )
				pBackNode->m_dwPolyCount = dwBackCount;
		}
		else
		{
			// The sphere tree node straddles the plane, but is not a leaf

			// Allocate back node
			pBackNode = AllocNode();
			pBackNode->m_Bounds			= pFrontNode->m_Bounds;
			pBackNode->m_pChildren[0]	= pBackNode->m_pChildren[1] = NULL;
			pBackNode->m_dwPolyCount	= 0;
			pBackNode->m_pPolygons		= NULL;
			//Trace( "Split: Back sphere tree node 0x%.8X created - not a leaf\n", pBackNode );

			// Split the sphere tree recursively
			// Split the A branch
			if( pFrontNode->m_pChildren[0] && pFrontNode->m_pChildren[0]->m_dwPolyCount > 0 ) 
			{
				CSphereTreeNode* pNewBackNode = NULL;
				uSplits += Split( pSplitPolygon, pFrontNode->m_pChildren[0], pNewBackNode );
				if ( pNewBackNode )
				{
					pNewBackNode->m_pParent		= pBackNode;
					pBackNode->m_pChildren[0]	= pNewBackNode;
				}
			}

			// Split the B branch
			if( pFrontNode->m_pChildren[1] && pFrontNode->m_pChildren[1]->m_dwPolyCount > 0 ) 
			{
				CSphereTreeNode* pNewBackNode = NULL;
				uSplits += Split( pSplitPolygon, pFrontNode->m_pChildren[1], pNewBackNode );
				if ( pNewBackNode )
				{
					pNewBackNode->m_pParent		= pBackNode;
					pBackNode->m_pChildren[1]	= pNewBackNode;
				}
			}

			// Update polygon counts
			pFrontNode->m_dwPolyCount =	( pFrontNode->m_pChildren[0] ? pFrontNode->m_pChildren[0]->m_dwPolyCount : 0 ) + 
										( pFrontNode->m_pChildren[1] ? pFrontNode->m_pChildren[1]->m_dwPolyCount : 0 );
			pBackNode->m_dwPolyCount =	( pBackNode->m_pChildren[0] ? pBackNode->m_pChildren[0]->m_dwPolyCount : 0 ) + 
										( pBackNode->m_pChildren[1] ? pBackNode->m_pChildren[1]->m_dwPolyCount : 0 );
		}
	}

	return uSplits;
}