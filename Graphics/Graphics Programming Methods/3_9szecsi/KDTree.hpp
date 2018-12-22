// KDTree.h: interface for the KDTree class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __KDTREE_HPP__
#define __KDTREE_HPP__

#include "Patch.hpp"

#define KDTREE_EPSILON						1e-4
#define KDTREE_EPSILON_NEIGHBOUR_PARTITION		1e-8
#define KDTREE_CACHE_LINE_BYTES				64
#define KDTREE_BOUNDS_EPSILON				0.0

inline void GetBoundingBox (BoundingBox& bbox, Intersectable** iObjects, int nObjects) 
{
	if(!nObjects)
		return;
	bbox.minPoint = iObjects [0]->bbox.minPoint;
	bbox.maxPoint = iObjects [0]->bbox.maxPoint;
	for (int i=1; i<nObjects; i++) {
		bbox.minPoint <= iObjects [i]->bbox.minPoint;
		bbox.maxPoint >= iObjects [i]->bbox.maxPoint;
	}
}

/*! This class implements a kd-tree. Splitting, termination and cutting of empty space is
based on cost estimation. Memory representation uses cache-line sized sub-trees and
mapping of an unbalanced tree into an array. */
class KDTree{
	unsigned int nCacheLines;		// #cache line sized memory segments used
	unsigned int nCacheLineNodes; 	// #nodes per cache line
	unsigned int nCacheLineBytes; 	// bytes per cache line
	unsigned int depth;				// depth of preallocated balanced tree

	BoundingBox	boundingBox;		// bounding box containing all objects

	float* eNodeTable;				// memory allocated for nodes
	float* nodeTable;				// array aligned on cache lines

	unsigned int		nObjects;	// #objects
	Intersectable**		objects;	// array objects

	// FIFO to store indices of free nodes during the build
	unsigned int* freeNodes;
	unsigned int nFreeNodes;
	unsigned int maxFreeNodes;
	unsigned int lastFreeNodeIndex;
	unsigned int firstFreeNodeIndex;

	//! stack to implement the recursive traversal algorithm
	struct TraverseStack{
		float rayMin;
		float rayMax;
		unsigned int tNode;
	} *traverseStack;

	// Traces the tree, and frees the memory occupied by the object lists
	void deleteLeaves(unsigned int nodeID);

	// Recursive algorithm to build the tree.
	void build(unsigned int nodeID, unsigned int* boundsArray, unsigned int nPatches, BoundingBox& limits, unsigned char axisNmask);

	//qSort an array segment of object extremes
	void medianSort(unsigned int* lo0, unsigned int* hi0, unsigned char axis);
	// Compare two object extremes according to the coordinate specified
	bool inline compare(unsigned int aIndex, unsigned int bIndex, unsigned char axis);
	// return the value of the object extreme referenced
	float inline getBoundValue(unsigned int * bes, unsigned char axis);
	// binary search: finds the position of a value in a sorted array partition
	unsigned int* findBound(unsigned int* bar, unsigned int nBounds, float loc, unsigned char axis);
	
	// Tells if a node is a leaf.
	bool inline isLeaf(unsigned int xnode);
	/* Retrieves the node indices corresponding to the children of the specified node.
	Shall those exceed the limits of the pre-allocated balanced tree, allocates two free
	nodes and sets the pointers appropriately. Returns 'false' if there was no space left.*/
	bool inline makeChildren(unsigned int parent, unsigned int& lefty, unsigned int& righty);
	/* Retrieves the node indices corresponding to the children of the specified node.
	Returns false if the node is a leaf.*/
	bool inline followChildren(unsigned int& parent, unsigned int &lefty, unsigned int &righty);

	/* Retrieves the node indices corresponding to the children of the specified node.
	Shall those exceed the limits of the pre-allocated balanced tree, 'false' is returned.*/
	bool inline getFreeNodes(unsigned int parent, unsigned int& lefty, unsigned int& righty);

public:
	/* If the node referenced by oNode is on the last level of the pre-allocated
	balanced tree, a pointer to a free node is placed, and the index of this free
	node is returned. Else, oNode is returned. */
	inline unsigned int makePointer(unsigned int oNode);

	KDTree (Intersectable** objects, int nObjects);
	~KDTree () {
		deleteLeaves(0);
		delete traverseStack;
		delete eNodeTable;		
	}

	bool traverse (const Ray& ray, HitRec& hitRec, float minT, float maxT);
	BoundingBox& getBoundingBox();
};


#endif //__KDTREE_HPP__