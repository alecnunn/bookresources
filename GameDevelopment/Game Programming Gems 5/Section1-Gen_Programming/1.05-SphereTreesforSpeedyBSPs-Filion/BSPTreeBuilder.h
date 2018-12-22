//
// CBSPTreeBuilder.h
//

#ifndef _SPBSPTREEBUILDER_
#define _SPBSPTREEBUILDER_

#include "BSPTree.h"

const DWORD MAX_SPHERETREE_STACK	= 1024;

class CBSPTreeBuilder : public CBSPTree
{

public:

//	========================================== LIFECYCLE ==========================================
	CBSPTreeBuilder();

//	========================================== OPERATIONS =========================================

	// Builds the BSP tree, taking an sphere tree of polygons as input.
			bool	Build( CSphereTree* pSphereTree, CProgressCtrl* pProgress );
	
//	========================================== ACCESS =============================================
	// Returns the sphere tree used to generate the BSP tree.
	CSphereTree*		GetSphereTree() const									{ return m_pSphereTree;				};
	// STATS
	DWORD			GetPolyProcessed() const							{ return m_dwPolyProcessed;		};
	DWORD			GetCoplanarPolyCount() const						{ return m_dwCoplanarCount;		};

	DWORD			GetBSPDepth() const									{ return m_dwBSPDepth;			};
	float			GetAvgDepth() const									{ return m_fAvgDepth;			};
	DWORD			GetTotalNodeCount() const							{ return m_dwTotalNodes;		};
	DWORD			GetBranchNodeCount() const							{ return m_dwBranchNodes;		};
	DWORD			GetBackNodeCount() const							{ return m_dwBackNodes;			};
	DWORD			GetFrontNodeCount() const							{ return m_dwFrontNodes;		};
	DWORD			GetLeafNodeCount() const							{ return m_dwLeafNodes;			};

protected :

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Builds a single node of the BSP tree.
	//
	// Parameters:
	//		pSphereTree				SphereTree used to generate the BSP tree
	//		pSphereTreeNode			Current sphere tree node to work with
	//		dwLevel				BSP node level
	//		pParentChild		Pointer to the parent node's child variable for the new child node
	CBSPNode*	BuildNode(	CSphereTree* pSphereTree, CSphereTreeNode* pSphereTreeNode, DWORD dwLevel, CBSPNode* pParent,
							CBSPNode*& pParentChild );

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Chooses the best split plane for BSP tree generation.
	//
	// Parameters:
	//		pSphereTree				SphereTree used to generate the BSP tree
	//		pBestCandidate		Returns the split polygon that will be chosen
	//		dwCoplanars			Returns the number of polygons coplanar with the split polygon; these coplanar
	//							polygons will be relinked right after the split polygon
	void		ChooseSplitPlane( CSphereTreeNode* pSphereTreeRoot, CPolygon*& pBestCandidate, DWORD& uCoplanars );

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Tests a certain split candidate to see if it is a suitable splitter.
	//
	// Parameters:
	//		pSphereTree				SphereTree used to generate the BSP tree
	//		bTerminateEarly			Used internally for recursive calls; set to false
	void		TestSplitCandidate( CSphereTreeNode* pSphereTree, bool& bTerminateEarly );

	CPolygon*				m_pCurCandidate;		// Current splitting candidate
	DWORD					m_dwBestSplit;			// Best (lowest) split count
	DWORD					m_dwSplits;				// Current number of splits
	DWORD					m_dwBackCount;			// Number of polygons on back side of current plane
	DWORD					m_dwFrontCount;			// Number of polygons on front side of current plane

	CSphereTreeNode*			m_SphereTreeStack[MAX_SPHERETREE_STACK];	// SphereTree stack, used for sphere tree traversals

	CSphereTree*				m_pSphereTree;							// SphereTree used to generate the BSP tree

	DWORD					m_dwPolyLeft;						// Polygons left for processing
	DWORD					m_dwBSPDepth;						// Depth of BSP tree
	DWORD					m_dwTotalNodes;						// Total BSP nodes
	DWORD					m_dwPolyProcessed;					// Polygons processed
	DWORD					m_dwBranchNodes;					// Branch nodes (front & back children)
	DWORD					m_dwFrontNodes;						// Front nodes (front child only)
	DWORD					m_dwBackNodes;						// Back nodes (back child only)
	DWORD					m_dwLeafNodes;						// Leaf nodes
	DWORD					m_dwCoplanarCount;					// Number of coplanar polygons
	float					m_fAvgDepth;						// Average BSP Depth

	CProgressCtrl*			m_pProgress;
};

#endif	// _SPBSPTREEBUILDER_