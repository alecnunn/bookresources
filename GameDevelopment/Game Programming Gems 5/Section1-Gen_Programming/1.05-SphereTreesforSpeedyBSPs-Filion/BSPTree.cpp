//
// CBSPTree.cpp
//

#include "stdafx.h"
#include "BSPTree.h"

CBSPNode::CBSPNode( bool bLeaf, CBSPTree* pTree ) :
	m_pFrontChild		( NULL			),
	m_pBackChild		( NULL			),
	m_pPolygons			( NULL			),
	m_dwNumCoplanars	( 0				)
{
	static WORD	s_wID = 0;
	if( bLeaf )
	{
		ASSERT( pTree );
		m_wID = pTree->m_wNextLeafID++;
		pTree->m_dwLeafCount++;
		pTree->m_BSPNodes.push_back( this );
	}
	else 
	{
		m_wID = 0;
	}
}

CBSPNode::~CBSPNode()
{
}

CBSPTree::CBSPTree() :
 m_dwLeafCount		( 0		),
 m_wNextLeafID		( 0		)
{
}

CBSPTree::~CBSPTree()
{
	Finalize();
}

void CBSPTree::Finalize()
{
	for( DWORD i = 0; i < m_BSPNodes.size(); i++ ) 
		delete m_BSPNodes[i];
	m_BSPNodes.clear();
}