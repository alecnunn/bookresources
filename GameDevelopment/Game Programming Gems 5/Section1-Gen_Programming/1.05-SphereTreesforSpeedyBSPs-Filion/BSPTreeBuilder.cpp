//
// CBSPTreeBuilder.cpp
//

#include "stdafx.h"

#include "BSPTreeBuilder.h"
#include "Polygon.h"
#include "Trace.h"

const DWORD SPHERETREE_STACK_MAX_DEPTH = 1024;

CBSPTreeBuilder::CBSPTreeBuilder() :
 m_pSphereTree			( NULL	),
 m_pCurCandidate	( NULL	),
 m_dwBestSplit		( 0		),
 m_dwSplits			( 0		),
 m_dwBackCount		( 0		),
 m_dwFrontCount		( 0		),
 m_dwBSPDepth		( 0		),
 m_dwTotalNodes		( 0		),
 m_dwPolyProcessed	( 0		),
 m_dwBranchNodes	( 0		),
 m_dwFrontNodes		( 0		),
 m_dwBackNodes		( 0		),
 m_dwLeafNodes		( 0		),
 m_dwCoplanarCount	( 0		),
 m_fAvgDepth		( 0.0f	)
{
}

bool CBSPTreeBuilder::Build( CSphereTree* pSphereTree, CProgressCtrl* pProgress )
{
	m_dwBSPDepth		= 0;
	m_dwTotalNodes		= 0;
	m_dwPolyProcessed	= 0;
	m_dwBranchNodes		= 0;
	m_dwFrontNodes		= 0;
	m_dwBackNodes		= 0;
	m_dwLeafNodes		= 0;
	m_dwCoplanarCount	= 0;
	m_fAvgDepth			= 0.0f;
	m_dwBestSplit		= 0;
	m_dwSplits			= 0;
	m_dwBackCount		= 0;
	m_dwFrontCount		= 0;
	m_pProgress			= pProgress;
	m_dwLeafCount		= 0;
	m_wNextLeafID		= 0;

	m_dwPolyLeft = pSphereTree->GetPolyCount();
	m_vBounds[0] = pSphereTree->GetMinBounds();
	m_vBounds[1] = pSphereTree->GetMaxBounds();
	m_pSphereTree	= pSphereTree;
	CBSPTree::Finalize();

	m_pProgress->SetRange32( 0, m_dwPolyLeft );
	m_pProgress->SetPos( 0 );
	CBSPNode* pStub = NULL;
	m_pRoot = BuildNode( pSphereTree, pSphereTree->GetRoot(), 0, NULL, pStub );

	if( m_dwLeafNodes )
		m_fAvgDepth /= (float)m_dwLeafCount;
	else
		m_fAvgDepth = 0.0f;

	// PRINTF( "Total : %d BSP nodes, %d leafs.\n", m_uTotalNodes, m_uLeafCount );

	return ( m_pRoot != NULL );
}

char szBuffer[255];


CBSPNode* CBSPTreeBuilder::BuildNode(	CSphereTree* pSphereTree, CSphereTreeNode* pSphereTreeNode, DWORD dwLevel, 
										CBSPNode* pParent, CBSPNode*& pParentChild )
{	
	// Okay, so we got a bunch of polygons to split in two halves. We will split along one of the polygons,
	// as most BSPs do, we just need to choose along which polygons to split.
	// A good BSP tree should split as few triangles as possible, and maintain good balance on each side of the tree

	CSphereTreeNode*	pLastAlloc		= pSphereTree->GetLastAllocNode();
	
	dwLevel++;
	if( dwLevel > m_dwBSPDepth )
		m_dwBSPDepth = dwLevel;

	// Create new BSP branch node
	CBSPNode* pBSPNode = new CBSPNode( false );

	m_BSPNodes.push_back( pBSPNode );
	pParentChild = pBSPNode;
	m_dwTotalNodes++;

	//Trace( "Building BSP node %d: ", m_dwTotalNodes );

	// Choose a splitting plane
	DWORD	dwNumCoplanars	= 0;
	CPolygon* pBestCandidate;
	ChooseSplitPlane( pSphereTreeNode, pBestCandidate, dwNumCoplanars );
	//Trace( "Splitter found " );

	pBSPNode->m_Plane			= pBestCandidate->m_Plane;
	pBSPNode->m_dwNumCoplanars	= dwNumCoplanars;
	m_dwPolyLeft				-= dwNumCoplanars;
	m_dwPolyProcessed			+= dwNumCoplanars;
	m_pProgress->SetPos( m_dwPolyProcessed );

	// Okay, so we chose our splitting plane; let's do the actual splitting
	// One of the main ideas of the algorithm is that each BSP branch get its own sub-space in the form
	// of a separate sphere tree - so at each BSP branch you will have a front sphere tree and a back sphere tree
	CSphereTreeNode* pBackSphereTree	= pSphereTree->AllocNode();
	pBackSphereTree->m_Bounds			= pSphereTreeNode->m_Bounds;

	// Upon returning from Split(), pSphereTree will contain the front space sphere tree and pBackSphereTree will contain the
	// back space sphere tree
	DWORD dwSplits = pSphereTree->Split( pBestCandidate, pSphereTreeNode, pBackSphereTree );	// 0
	m_dwPolyLeft += dwSplits;
	//Trace( "SphereTree split " );

	if( !pBackSphereTree->m_pPolygons )
	{
		pBackSphereTree->m_dwPolyCount =	( pBackSphereTree->m_pChildren[0] ? pBackSphereTree->m_pChildren[0]->m_dwPolyCount : 0 ) + 
										( pBackSphereTree->m_pChildren[1] ? pBackSphereTree->m_pChildren[1]->m_dwPolyCount : 0 );
	}

	// Categorize the BSP nodes
	if( pSphereTreeNode->m_dwPolyCount > 0 )
	{
		if( pBackSphereTree->m_dwPolyCount > 0 )
		{
			// A branch node has split candidates on both sides
			m_dwBranchNodes++;
			//Trace( "Adding branch node.\n" );
		}
		else
		{
			// A front node has split candidates  only on the front side
			m_dwFrontNodes++;
			//m_fAvgDepth += dwLevel;
			//Trace( "Adding front node.\n" );
		}
	}
	else
	{
		if( pBackSphereTree->m_dwPolyCount > 0 )
		{
			// A back node has split candidates only on the back side
			m_dwBackNodes++;
			//m_fAvgDepth += dwLevel;
			//Trace( "Adding back node.\n" );
		}
		else
		{
			// A leaf node has no split candidates on either side
			m_dwLeafNodes++;
			m_fAvgDepth += dwLevel; // * 2;
			//Trace( "Adding leaf node.\n" );
		}
	}

	// Build BSP node children
	if( pSphereTreeNode->m_dwPolyCount > 0 )
	{
		if( !BuildNode( pSphereTree, pSphereTreeNode, dwLevel, pBSPNode, pBSPNode->m_pFrontChild ) )
			return false;
	}
	else
	{
		// Front leaf
		CBSPNode* pBSPLeaf = new CBSPNode( true, this );
		pBSPNode->m_pFrontChild = pBSPLeaf;
	}

	if( pBackSphereTree->m_dwPolyCount > 0 )
	{
		if ( !BuildNode( pSphereTree, pBackSphereTree, dwLevel, pBSPNode, pBSPNode->m_pBackChild ) )
			return false;
	}
	else
	{
		// Back leaf
		CBSPNode* pBSPLeaf = new CBSPNode( true, this );
		pBSPNode->m_pBackChild = pBSPLeaf;
	}

	// Free memory that's no longer needed at this point (i.e. the whole back tree)
	// Delete everything that has been allocated inside this iteration of the routine
	CSphereTreeNode* pCurSphereTreeNode = pSphereTree->GetLastAllocNode();
	while( pCurSphereTreeNode && pCurSphereTreeNode != pLastAlloc )
	{
		CSphereTreeNode* pNext = pCurSphereTreeNode->m_pNextAlloc;
		pSphereTree->FreeNode( pCurSphereTreeNode );
		pCurSphereTreeNode = pNext;
	}

	return pBSPNode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CBSPTreeBuilder::ChooseSplitPlane()
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void CBSPTreeBuilder::ChooseSplitPlane( CSphereTreeNode* pSphereTreeRoot, CPolygon*& pBestCandidate, DWORD& dwNumCoplanars  )
{
	ASSERT( pSphereTreeRoot );

	DWORD		SphereTreeStackIndex	= 0;
	DWORD		dwBestBalance		= INT_MAX;
				m_dwBestSplit		= INT_MAX;
				pBestCandidate		= NULL;
				dwNumCoplanars		= 0;

	CSphereTreeNode* pCurSphereTreeNode = pSphereTreeRoot;
	DWORD dwCandidatesTested = 0;

	// Iterate through all polygons and test them as candidates
	while( true )
	{		
		// SphereTree leaves will contain polygons - sphere tree branches will not
		m_pCurCandidate	= pCurSphereTreeNode->m_pPolygons;
		while( m_pCurCandidate )
		{
			// We've hit an sphere tree leaf, test the polygons
			CPolygon* pNextCandidate = m_pCurCandidate->m_pNext;

			// Select this polygon's splitting plane & test it
			m_dwSplits		= 0;
			m_dwBackCount	= 0;
			m_dwFrontCount	= 0;
			bool bTerminateEarly	= false;
			// TestSplitCandidate will return its results in m_dwSplits, m_dwBackCount & m_dwFrontCount for
			// the current candidate polygon
			TestSplitCandidate( pSphereTreeRoot, bTerminateEarly );

			// Compute balance
			DWORD dwBalance = abs( (int)( m_dwFrontCount - m_dwBackCount ) );
			if(		m_dwSplits == m_dwBestSplit && pBestCandidate && 
					(	m_pCurCandidate->m_Plane == pBestCandidate->m_Plane ||
						m_pCurCandidate->m_Plane.EqualToReverse( pBestCandidate->m_Plane ) ) )
			{
				// This polygon is coplanar with our current winning candidate
				// We shouldn't split on the same plane twice, so link the coplanar polygon into the
				// coplanar list, we'll remove all the coplanars at once later on
				dwNumCoplanars++;
				m_pCurCandidate->m_pNextCoplanar	= pBestCandidate;
				pBestCandidate						= m_pCurCandidate;
			}
			else
			{
				if ( m_dwSplits < m_dwBestSplit || ( m_dwSplits == m_dwBestSplit && dwBalance < dwBestBalance ) )
				{
					// We have a winner! This our best candidate so far
					m_dwBestSplit						= m_dwSplits;
					dwBestBalance						= dwBalance;
					pBestCandidate						= m_pCurCandidate;
					m_pCurCandidate->m_pNextCoplanar	= NULL;
					dwNumCoplanars						= 1;
					ASSERT( pBestCandidate->m_pParentNode == pCurSphereTreeNode );
				}
			}

			dwCandidatesTested++;
			m_pCurCandidate = pNextCandidate;
		}

		// Traverse the sphere tree to iterate the polygons in that sphere tree
		// Why don't we iterate through the global liste of polygons? Because we only want to use
		// as split candidates the polygons that are classified in this branch of the BSP tree
		// pSphereTreeRoot contains the sphere tree that holds all polygons for this branch of the BSP
		// Remember, sphere trees get successively split until each BSP branch has its own mini-sphere tree
		if ( pCurSphereTreeNode->m_pChildren[0] )
		{
			if ( pCurSphereTreeNode->m_pChildren[1] ) 
			{
				// Push back child on stack
				m_SphereTreeStack[SphereTreeStackIndex++] = pCurSphereTreeNode->m_pChildren[1];
				ASSERT( SphereTreeStackIndex < SPHERETREE_STACK_MAX_DEPTH );
			}
			// Process front child
			pCurSphereTreeNode = pCurSphereTreeNode->m_pChildren[0];
		}
		else
		{
			if ( pCurSphereTreeNode->m_pChildren[1] ) 
				// Process back child
				pCurSphereTreeNode = pCurSphereTreeNode->m_pChildren[1];
			else
			{
				// This is an sphere tree leaf - pop the stack
				if ( SphereTreeStackIndex == 0 ) 
					// Stack is empty - nothing left to process
					break;
				else 
				{
					pCurSphereTreeNode = m_SphereTreeStack[SphereTreeStackIndex - 1];
					SphereTreeStackIndex--;
				}
			}
		}
	}
	
	ASSERT( pBestCandidate );

	// Decrement the polygon count up the sphere tree, as the splitter polygon will no longer be part of the
	// list of polgons to compare with
	// Do the same for the coplanars as well
	CPolygon* pPolygon = pBestCandidate;
	while ( pPolygon )
	{
		CSphereTreeNode* pNode = pPolygon->m_pParentNode;
		while ( pNode )
		{
			pNode->m_dwPolyCount--;
			pNode = pNode->m_pParent;
		}

		// Remove the splitter from the list of polygons
		pPolygon->Unlink( pPolygon->m_pParentNode->m_pPolygons );
		pPolygon = pPolygon->m_pNextCoplanar;
	}

	// Total coplanar polygon count
	m_dwCoplanarCount	+= dwNumCoplanars - 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CBSPTreeBuilder::TestSplitCandidate()
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void CBSPTreeBuilder::TestSplitCandidate( CSphereTreeNode* pSphereTreeNode, bool& bTerminateEarly )
{
	// Test sphere tree node with plane
	float fDistance = m_pCurCandidate->m_Plane.GetDistance( pSphereTreeNode->m_Bounds.m_vPosition );
	if ( fDistance < -pSphereTreeNode->m_Bounds.m_fRadius )
	{
		// SphereTree node completely in back
		m_dwBackCount += pSphereTreeNode->m_dwPolyCount;
	}
	else if ( fDistance > pSphereTreeNode->m_Bounds.m_fRadius )
	{
		// SphereTree node completely in front
		m_dwFrontCount += pSphereTreeNode->m_dwPolyCount;
	}
	else
	{
		// SphereTree node possibly straddles the plane
		if ( pSphereTreeNode->m_pPolygons )
		{
			// This is an sphere tree leaf, so we have to test the polygons individually
			CPolygon* pCurPoly = (CPolygon*)pSphereTreeNode->m_pPolygons;
			while ( pCurPoly )
			{
				// Check for splits with the current candidate
				if ( pCurPoly != m_pCurCandidate )
				{
					switch ( pCurPoly->GetSide( m_pCurCandidate->m_Plane ) )
					{
					case CPolygon::epsFRONT :
						m_dwFrontCount++;
						break;
					case CPolygon::epsBACK :
						m_dwBackCount++;
						break;
					case CPolygon::epsBOTH :
						// Ya, this one causes a split
						m_dwSplits++;
						if ( m_dwSplits > m_dwBestSplit ) 
						{
							bTerminateEarly = true;
							break;		// Too many splits - This candidates's a loser, discard it early, seeya
						}
						break;
					}
				}
				pCurPoly = pCurPoly->m_pNext;
			}
		}
		else
		{
			// sphere tree node, so test both children
			if ( pSphereTreeNode->m_pChildren[0] && pSphereTreeNode->m_pChildren[0]->m_dwPolyCount > 0 ) 
				TestSplitCandidate( pSphereTreeNode->m_pChildren[0], bTerminateEarly );
			if ( !bTerminateEarly && pSphereTreeNode->m_pChildren[1] && pSphereTreeNode->m_pChildren[1]->m_dwPolyCount > 0 ) 
				TestSplitCandidate( pSphereTreeNode->m_pChildren[1], bTerminateEarly );
		}
	}
}