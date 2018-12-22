// Graphics Programming Methods
// An Effective kd-tree Implementation
// László Szécsi
// 2003.

// declaration of kd-tree members

#ifndef GRAPHICS_PROGRAMMING_METHODS_KDTREE
#define GRAPHICS_PROGRAMMING_METHODS_KDTREE

#include "malloc.h"
#include "Heap.hpp"
#include "Scene.hpp"

#define KDTREE_CACHE_LINE_BYTES					64

/*! This class implements a kd-tree. Splitting, termination and cutting of empty space is
based on cost estimation. Memory representation uses cache-line sized sub-trees and
mapping of an unbalanced tree into an array. */
class KDTree {
	unsigned int nCacheLines;		// #cache line sized memory segments used
	unsigned int maxNCacheLines; 	// #nodes per cache line
	unsigned int nCacheLineNodes; 	// #nodes per cache line
	unsigned int nCacheLineBytes; 	// bytes per cache line
	unsigned int depth;				// depth of preallocated balanced tree

	BoundingBox	boundingBox;		// bounding box containing all objects

	float* poolNodeTable;				// memory allocated for nodes
	float* nodeTable;				// array aligned on cache lines

	unsigned int		nObjects;	// #objects
	Intersectable**		objects;	// array objects

	// minimum heap to store indices of free nodes during the build
	Heap<unsigned int>* freeNodes;

	// stack to implement the recursive traversal algorithm
	struct TraverseStack{
		float rayMin;				// minimum distance from origin
		float rayMax;				// maximum distance from origin
		unsigned int tNode;			// node index of kd-tree node to be traced
	} *traverseStack;

	// traces the tree defined by root node index 'nodeID',
	// and frees the memory occupied by the object lists
	void deleteLeaves(unsigned int nodeID);

	// recursive algorithm to build the tree
	void build(unsigned int nodeID,	// node index of kd-tree node to be created
		unsigned int* boundsArray,	// array of duplicated object indices,
									// first bit tells mimima and maxima apart
		unsigned int nObjects,		// #objects (boundsArray's size is 2*nObjects)
		BoundingBox& limits,		// kd-tree node volume
		unsigned char axisNmask);	// bits 5,4,3 indicate z,y,x failed cut attempts
									// bits 1,0 identify cut axis to be used

	// qSort an array segment of object extremes
	void quickSort(unsigned int* lo0, unsigned int* hi0, unsigned char axis);
	// compare two object extrema according to the coordinate specified
	bool inline compare(unsigned int aIndex, unsigned int bIndex, unsigned char axis);
	// return the value of the object extreme referenced
	float inline getBoundValue(unsigned int * extremeIndex, unsigned char axis);
	// binary search: finds the position of value 'loc' in a sorted array partition
	unsigned int* findBound(unsigned int* extremeArrayStart, unsigned int nBounds, float loc, unsigned char axis);
	
	// tell if a node is a leaf
	bool inline isLeaf(unsigned int xnode);
	// retrieve the node indices corresponding to the children of the specified node
	// return false if the node is a leaf
	bool inline followChildren(unsigned int& parent, unsigned int &leftChild, unsigned int &rightChild);

	// retrieve the node indices corresponding to the children of the specified node
	// if they are not suitable to be the root of a free sub-tree, return false
	bool inline getFreeNodes(unsigned int parent, unsigned int& leftFree, unsigned int& rightFree);

	// accumulate object extrema
	inline void createBoundingBox (Intersectable** iObjects, int nObjects) 
	{
		if(!nObjects)
			return;
		boundingBox.minPoint = iObjects [0]->bbox.minPoint;
		boundingBox.maxPoint = iObjects [0]->bbox.maxPoint;
		for (int i=1; i<nObjects; i++) {
			// check accumulate operators of Vector class
			boundingBox.minPoint <= iObjects [i]->bbox.minPoint;
			boundingBox.maxPoint >= iObjects [i]->bbox.maxPoint;
		}
	}

	/* If the node referenced by oNode is on the last level of a cache-line sized sub-tree,
	a pointer to a free node is placed, and the index of this free
	node is returned. Else, originalNode is returned. */
	inline unsigned int makePointer(unsigned int originalNode);

public:
	KDTree (Intersectable** objects, int nObjects);
	~KDTree () {
		deleteLeaves(0);
		delete traverseStack;
		free(poolNodeTable);
	}

	bool traverse (const Ray& ray, HitRec& hitRec, float minT, float maxT);
	BoundingBox& getBoundingBox();
};


#endif //GRAPHICS_PROGRAMMING_METHODS_KDTREE