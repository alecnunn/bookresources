//
// CSphereTree.h
//

#ifndef _SPHERETREE_H_
#define _SPHERETREE_H_

#include <vector>
#include "MathStructs.h"

class CPolygon;

class CSphereTreeNode
{

public :

	CSphereTreeNode();

	void	Reset();

	CSphere				m_Bounds;			// SphereTree bounding sphere

	CPolygon*			m_pPolygons;		// Polygons in sphere tree node (leafs only)
	DWORD				m_dwPolyCount;		// Polygon count for this sphere tree node

	CSphereTreeNode*		m_pChildren[2];		// Front and back child
	CSphereTreeNode*		m_pParent;			// sphere tree node parent

	CSphereTreeNode*		m_pNextFree;		// Used to link to free sphere tree nodes
	CSphereTreeNode*		m_pPrevAlloc;		// Link to previously allocated node
	CSphereTreeNode*		m_pNextAlloc;		// Used to link to allocated sphere tree nodes

	DWORD				m_dwID;				// Unique sphere tree node ID

};


class CSphereTree
{

public:

	CSphereTree();
	virtual ~CSphereTree();

	CSphereTreeNode*				AllocNode();
	void						FreeNode( CSphereTreeNode* pNode );
	
	void						AddTriangles(	DWORD dwTriCount, DWORD dwVertexCount, DWORD* pIndices, 
												CVector* pVertices );
	CSphereTreeNode*				Build( CProgressCtrl* pProgress );
	DWORD						Split(	CPolygon* pSplitPolygon, CSphereTreeNode*& pFrontNode, 
										CSphereTreeNode*& pBackNode );

	inline	CSphereTreeNode*		GetLastAllocNode() const;

	inline	CSphereTreeNode*		GetRoot() const;

	inline	DWORD				GetPolyCount() const;

	inline	CVector				GetMinBounds() const;

	inline	CVector				GetMaxBounds() const;

	inline	DWORD				GetNodeCount() const;

	inline	DWORD				GetLeafCount() const;

	inline	float				GetAverageDepth() const;

	inline	DWORD				GetDepth() const;

private:

	CSphereTreeNode*				BuildNode(	CPolygon* pPolygons, CSphereTreeNode* pParentNode, eVectorComponents Axis, CVector vMin, 
											CVector vMax, DWORD dwLevel );

	void						Cleanup();

	void						ComputeBounds();

	std::vector<CPolygon*>		m_Polygons;
	CVector						m_Bounds[2];			// Bounding box for all the polygons


	CSphereTreeNode*				m_pRoot;				// Root of sphere tree
	std::vector<CSphereTreeNode*>	m_Nodes;				// Linear array of sphere tree nodes
	DWORD							m_uNodeCount;			// Number of sphere tree nodes

	std::vector<CSphereTreeNode*>	m_FreeBatches;			// Free batches of sphere tree nodes
	CSphereTreeNode*				m_pFreeNodes;			// Free sphere tree node list
	CSphereTreeNode*				m_pLastAllocNode;		// Last allocated sphere tree node list

	DWORD						m_dwLeaves;				// Number of sphere tree leaves
	DWORD						m_dwDepth;				// Depth of sphere tree
	float						m_fAvgDepth;			// Average sphere tree depth

	CProgressCtrl*				m_pProgress;

	DWORD						m_dwPolysProcessed;

	void	Finalize();

};

inline DWORD CSphereTree::GetPolyCount() const
{
	return (DWORD)m_Polygons.size();
}

inline CVector CSphereTree::GetMinBounds() const
{
	return m_Bounds[0];
}

inline CVector CSphereTree::GetMaxBounds() const
{
	return m_Bounds[1];
}

inline CSphereTreeNode* CSphereTree::GetLastAllocNode() const
{
	return m_pLastAllocNode;
}

inline DWORD CSphereTree::GetNodeCount() const
{
	return m_uNodeCount;
}

inline CSphereTreeNode* CSphereTree::GetRoot() const
{
	return m_pRoot;
}

inline DWORD CSphereTree::GetLeafCount() const
{
	return m_dwLeaves;
}

inline float CSphereTree::GetAverageDepth() const
{
	return m_fAvgDepth;
}

inline DWORD CSphereTree::GetDepth() const
{
	return m_dwDepth;
}


#endif // _SPHERETREE_H_