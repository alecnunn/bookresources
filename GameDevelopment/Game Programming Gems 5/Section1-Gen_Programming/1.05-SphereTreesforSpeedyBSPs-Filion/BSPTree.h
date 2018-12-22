//
// CBSPTree.h
//

#ifndef _BSPTREE_H_
#define _BSPTREE_H_

#include "SphereTree.h"

class CBSPTree;

class CBSPNode 
{

public :

	CBSPNode( bool bLeaf, CBSPTree* pTree = NULL );
	virtual	~CBSPNode();

	WORD				m_wID;				// Unique ID for this BSP leaf
	
	CPolygon*			m_pPolygons;		// Polygons inside the node

	CBSPNode*			m_pFrontChild;		// Front child
	CBSPNode*			m_pBackChild;		// Back child

	DWORD				m_dwNumCoplanars;	// Number of coplanar polygons

	CPlane				m_Plane;			// Split plane

};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CBSPTree
//
///////////////////////////////////////////////////////////////////////////////////////////////////


class CBSPTree
{

public:
	
	friend class CBSPNode;

	CBSPTree();
	virtual ~CBSPTree();

	DWORD			GetLeafCount() const							{ return m_dwLeafCount;			};

	// Returns the root of the generated BSP tree.
	CBSPNode*		GetRoot() const									{ return m_pRoot;				};
	void			SetRoot( CBSPNode* pRoot )						{ m_pRoot = pRoot;				};

protected :

	CBSPNode*					m_pRoot;					// Root of BSP tree
	std::vector<CBSPNode*>		m_BSPNodes;					// Linear array of the BSP nodes in the tree
	
	DWORD						m_dwLeafCount;				// Number of BSP leaves
	WORD						m_wNextLeafID;				// Next free leaf ID

	CVector						m_vBounds[2];				// Bounding box for all the polygons

	virtual	void	Finalize();

};

#endif // _BSPTREE_H_