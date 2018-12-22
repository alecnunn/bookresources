// Graphics Programming Methods
// An Effective kd-tree Implementation
// László Szécsi
// 2003.

// implementation of kd-tree methods

#include "KDTree.hpp"
#include <iostream>

bool KDTree::traverse (const Ray& ray, HitRec& hitRec, float minT, float maxT)
{
	static const float epsilonTolerance = 1e-4f;
	static const float epsilonDropBranch = 1e-3f;
	static const float epsilonDivisionByZero = 1e-5f;

	// handle dirs parallel to axis here in advance
	// will not need to check later for everyt node;
	Vector dirInverts;
	if(ray.dir.x > epsilonDivisionByZero || ray.dir.x < -epsilonDivisionByZero)
		dirInverts.x = 1.0f / ray.dir.x;
	else
		dirInverts.x = FLT_MAX;
	if(ray.dir.y > epsilonDivisionByZero || ray.dir.y < -epsilonDivisionByZero)
		dirInverts.y = 1.0f / ray.dir.y;
	else
		dirInverts.y = FLT_MAX;
	if(ray.dir.z > epsilonDivisionByZero || ray.dir.z < -epsilonDivisionByZero)
		dirInverts.z = 1.0f / ray.dir.z;
	else
		dirInverts.z = FLT_MAX;

	hitRec.object	= NULL;
	hitRec.isIntersect = false;
	hitRec.depth = FLT_MAX;

	float rayMin = minT;
	float rayMax = maxT;

	unsigned int tNode = 0;
	signed int stackPointer = -1;

	unsigned int leftChild;
	unsigned int rightChild;

	for(;;)
	{
		bool isLeaf = !followChildren(tNode, leftChild, rightChild);

		if(isLeaf) // node is a leaf
		{ 
			Intersectable** leafList = ((Intersectable***)nodeTable)[tNode];
			if(leafList) // leaf contains some objects
			{
				// first 4 bytes of leafList contains the length
				unsigned int leafListLength = (unsigned int)*leafList;
				Intersectable** leafListEnd = ++leafList + leafListLength;
				for (Intersectable** objectIterator = leafList; objectIterator < leafListEnd; objectIterator++) {
					float depth;
					// check if this test was already done
					if (ray.id != (*objectIterator)->lastTestedRayId) 
						(*objectIterator)->Intersect (ray, &depth, rayMin, rayMax);
					else
						depth = (*objectIterator)->lastTestedRayResult.depth;
					// intersection is acceptable only if it is within the node volume
					if ((*objectIterator)->lastTestedRayResult.isIntersect && 
							depth < hitRec.depth &&
							depth > rayMin - epsilonTolerance &&
							depth < rayMax + epsilonTolerance)
						hitRec = (*objectIterator)->lastTestedRayResult;
				}
			
				if(hitRec.isIntersect)
					return true;
				else
					if(stackPointer >= 0) 
					{ 
						//have not found anything on this branch, pop the other one from stack
						rayMin = traverseStack[stackPointer].rayMin;
						rayMax = traverseStack[stackPointer].rayMax;
						tNode = traverseStack[stackPointer].tNode;
						stackPointer--;
					}
					else
						return false;
			}
			else
			{
				//leaf is empty
				if(stackPointer >= 0)
				{
					rayMin = traverseStack[stackPointer].rayMin;
					rayMax = traverseStack[stackPointer].rayMax;
					tNode = traverseStack[stackPointer].tNode;
					stackPointer--;
				}
				else
					return false;
			}
		}
		else //not a leaf, go deeper
		{
			// gether cutting plane information from the nodeTable
			float cutPlane = nodeTable[tNode];
			unsigned char axis = *(unsigned int*)(&cutPlane) & 0x00000003;
			unsigned int bitfield = *(unsigned int*)(&nodeTable[tNode]) & 0xfffffffc;
			cutPlane = *(float*)(&bitfield);

			float origCoord;
			float dirCoord;
			switch(axis & 07)
			{
			case 0: origCoord = ray.origin.x;
					dirCoord = dirInverts.x;
					break;
			case 1: origCoord = ray.origin.y;
					dirCoord = dirInverts.y;
					break;
			case 2: origCoord = ray.origin.z;
					dirCoord = dirInverts.z;
			}

            // calculate cutting plane - ray intersection distance 't'
			register float t;
			t = (cutPlane - origCoord) * dirCoord;

			// find the child volume nearer to the origin
			unsigned int nearNode = rightChild;
			unsigned int farNode = leftChild;
			if(origCoord + epsilonDivisionByZero < cutPlane ||		// origin on the left
				!(cutPlane + epsilonDivisionByZero < origCoord) &&	// or on the plane
				dirCoord < 0)										// but dir goes left
			{
				nearNode = leftChild;
				farNode = rightChild;
			}

			if(t <= 0 || rayMax + epsilonDropBranch < t )
				// whole interval on near cell
				tNode = nearNode;
			else
			{
				if(t < rayMin - epsilonDropBranch)
					// whole interval on far cell
					tNode = farNode;
				else
				{
					// both cells
					// push far branch to stack
					stackPointer++;
					traverseStack[stackPointer].rayMin = t;
					traverseStack[stackPointer].rayMax = rayMax;
					traverseStack[stackPointer].tNode = farNode;
					// near branch is next to traverse
					tNode = nearNode;
					rayMax = t;
					rayMin;
				}
			}
		}
	}
	return false;
}

void KDTree::deleteLeaves(unsigned int nodeID)
{
	unsigned int rc;
	unsigned int lc;
	if(!followChildren(nodeID, lc, rc))
	{
		Intersectable** leafList = *(Intersectable***)&nodeTable[nodeID];
		if(leafList)
		{
			delete leafList;
			return;
		}
		return;
	}
	deleteLeaves(lc);
	deleteLeaves(rc);
}

bool KDTree::followChildren(unsigned int& parent, unsigned int &leftChild, unsigned int &rightChild)
{
	unsigned int subPos = parent & nCacheLineNodes;			// position within cache line
	unsigned int supPos = parent & ~nCacheLineNodes;		// cache line index
	unsigned int leafBit = ((*(unsigned int *)&nodeTable[parent | nCacheLineNodes]) >> subPos) & 0x1;
	if(leafBit)							// node is a leaf, has no children
		return false;					// leftChild & rightChild unchanged
	subPos = (subPos << 1) + 1;			// index of left child
	if(subPos >= nCacheLineNodes)		// out of cache line, node is pointer node
	{
		parent = ((unsigned int*)nodeTable)[parent];	// follow pointer
		subPos = parent & nCacheLineNodes;				// same procedure with new parent node
		supPos = parent & ~nCacheLineNodes;				// referenced node cannot be leaf
		subPos = (subPos << 1) + 1;						// or pointer node
	}
	leftChild = (parent & ~nCacheLineNodes) | subPos;	// recombine parent and node address
	rightChild = (parent & ~nCacheLineNodes) | subPos + 1;
	return true;
}

// retrieve would-have-been child nodes of a leaf node, if they are not in a last row
bool KDTree::getFreeNodes(unsigned int leaf, unsigned int& leftFree, unsigned int& rightFree)
{
	unsigned int subPos = leaf & nCacheLineNodes;			// position within cache line
	unsigned int supPos = leaf & ~nCacheLineNodes;			// cache line index
	subPos = (subPos << 1) + 1;			// index of left child position
	if(subPos < nCacheLineNodes)		// within cache line, may have free children positions
	{
		unsigned int grandSubPos = ((subPos + 1) << 1) + 2;	// rightmost grandchild
		if(grandSubPos >= nCacheLineNodes)					// children are on last row
			return false;									// not suitable for free nodes
		leftFree = (leaf & ~nCacheLineNodes) | subPos;
		rightFree = (leaf & ~nCacheLineNodes) | subPos + 1;
		return true;
	}
	else
		return false;					// leaf is on last row, has no free children
}

bool KDTree::isLeaf(unsigned int xnode)
{
	return ((*(unsigned int *)&nodeTable[xnode | nCacheLineNodes]) >> (xnode & nCacheLineNodes)) & 0x1;
}

float KDTree::getBoundValue(unsigned int * extremeIndex, unsigned char axis)
{
	//if Most Significant Bit of 'extremeIndex' is 1, take the maximum
	//if Most Significant Bit of 'extremeIndex' is 0, take the minimum
	unsigned int index = *extremeIndex;
	switch(axis){
	case 0:
	return (index & 0x80000000)?
		(objects[ index & 0x7fffffff]->bbox.maxPoint.x):
		(objects[ index & 0x7fffffff]->bbox.minPoint.x);
	case 1:
	return (index & 0x80000000)?
		(objects[ index & 0x7fffffff]->bbox.maxPoint.y):
		(objects[ index & 0x7fffffff]->bbox.minPoint.y);
	default:
	return (index & 0x80000000)?
		(objects[ index & 0x7fffffff]->bbox.maxPoint.z):
		(objects[ index & 0x7fffffff]->bbox.minPoint.z);
	}
}

void KDTree::build(unsigned int nodeID, unsigned int* boundsArray, unsigned int nObjects, BoundingBox& limits, unsigned char axisNmask)
{
	//separate the axisNmask parameter to axis and mask
	unsigned char axis = axisNmask & 07;
	unsigned char mask = axisNmask & 070;

	//calculate some values for the cost function
	float costBase;		// area of face perpendicular to axis
	float costSteep;	// half of circumference of face perpendicular to axis
						// surface area / 2 = costBase + costSteep * (length of edge parallel to axis)
						// cost = #(objects within volume) * surface area / 2
	switch(axis)
	{
	case 0:
		costBase = (limits.maxPoint.z - limits.minPoint.z)
					*	(limits.maxPoint.y - limits.minPoint.y);
		costSteep = (limits.maxPoint.z - limits.minPoint.z)
					+	(limits.maxPoint.y - limits.minPoint.y);
		break;
	case 1:
		costBase = (limits.maxPoint.x - limits.minPoint.x)
					*	(limits.maxPoint.z - limits.minPoint.z);
		costSteep = (limits.maxPoint.x - limits.minPoint.x)
					+	(limits.maxPoint.z - limits.minPoint.z);
		break;
	case 2:
		costBase = (limits.maxPoint.x - limits.minPoint.x)
					*	(limits.maxPoint.y - limits.minPoint.y);
		costSteep = (limits.maxPoint.x - limits.minPoint.x)
					+	(limits.maxPoint.y - limits.minPoint.y);
		break;
	}

	// references to extrema (corresponding to 'axis') of node volume
	float& limiMin = axis?((axis==1)?limits.minPoint.y:limits.minPoint.z):limits.minPoint.x;
	float& limiMax = axis?((axis==1)?limits.maxPoint.y:limits.maxPoint.z):limits.maxPoint.x;

	//sort the the extrema of the objects
	//we will need this to find the best splitting plane
	quickSort(boundsArray, boundsArray + (nObjects << 1) - 1, axis);

	// the following code section could be used to find the spatial and object medians
	// either to implement a simpler and less effective subdivision rule
	// or to restrict optimum search to the median interval (between two median)
	//***************************************************************************
	// find the value for the spatial median.... easy
	//	float spatialMedian = (limiMax + limiMin) / 2.0f;
	//find the position in the array corresponding to the spatial median value
	//	unsigned int* spatialMedianPosition = findBound(boundsArray, nObjects << 1, spatialMedian, axis);
	//find object median position (middle of array, surprise-surprise), and value
	//	unsigned int* objectMedianPosition = boundsArray + nObjects;
	//	float objectMedian = getBoundValue(objectMedianPosition, axis);
	//****************************************************************************

	// find the value for the left and right "cutting off empty space" (COES) planes 
	// remember the object extrema are already sorted
	float leftCoesCut;
	float rightCoesCut;
	switch(axis)
	{
	case 0:
		leftCoesCut = objects[boundsArray[0] & 0x7fffffff]->bbox.minPoint.x;
		rightCoesCut = objects[boundsArray[(nObjects << 1)-1] & 0x7fffffff]->bbox.maxPoint.x;
		break;
	case 1:
		leftCoesCut = objects[boundsArray[0] & 0x7fffffff]->bbox.minPoint.y;
		rightCoesCut = objects[boundsArray[(nObjects << 1)-1] & 0x7fffffff]->bbox.maxPoint.y;
		break;
	case 2:
		leftCoesCut = objects[boundsArray[0] & 0x7fffffff]->bbox.minPoint.z;
		rightCoesCut = objects[boundsArray[(nObjects << 1)-1] & 0x7fffffff]->bbox.maxPoint.z;
		break;
	default:
		printf("WARNING!, Should never happen");
	}
	if(rightCoesCut > limiMax)
		rightCoesCut = limiMax;
	if(leftCoesCut < limiMin)
		leftCoesCut = limiMin;

	// find the estimated cost of ray casting after COES
	float leftCoesCutCost = ((limiMax - leftCoesCut) * costSteep + costBase) * (nObjects);
	float rightCoesCutCost = ((rightCoesCut - limiMin) * costSteep + costBase) * (nObjects);
	// take care of numerical accuracy problems that arise when
	// limiMin == limiMax		(possible and allowed)
	if(leftCoesCutCost < 0) leftCoesCutCost = FLT_MAX;
	if(rightCoesCutCost < 0) rightCoesCutCost = FLT_MAX;

	// creep through array, evaluate cost for every possible cut,
	// maintaining the counters for intersected and completely passed objects
	int leftCount = 0;				// objects on the left of current cut
	int intersectedCount = 0;		// object intersected by current cut

	// we may limit the search to a partition of the array, eg. the median interval
	unsigned int* searchIntervalStart;
	unsigned int* searchIntervalEnd;

	// full range optimum search
	searchIntervalStart = boundsArray + 1;
	searchIntervalEnd = boundsArray + (nObjects << 1) - 1;

	// these three variable identify the best cut found so far:
	float minimumCostFound;
	float minimumCostCut;
	unsigned int* minimumCostPosition;

	// is left or right COES better? the better is our best candidate to start with
	int minimumIntersected = 0;
	int minimumLeft = 0;
	if(leftCoesCutCost < rightCoesCutCost)
	{
		minimumCostFound = leftCoesCutCost;
		minimumCostPosition = boundsArray;
		minimumCostCut = leftCoesCut;
		minimumIntersected = 0;
		minimumLeft = 0;
	}
	else
	{
		minimumCostFound = rightCoesCutCost;
		minimumCostPosition = boundsArray + (nObjects << 1);
		minimumCostCut = rightCoesCut;
		minimumIntersected = 0;
		minimumLeft = nObjects;
	}

	// creep up to the search interval... count objects
	unsigned int* trace = boundsArray;
	for(; trace< searchIntervalStart; trace++ )
	{
		if(*trace & 0x80000000)
		{
			intersectedCount--;
			leftCount++;
		}
		else
			intersectedCount++;
	}
	// evaluate the cost function, if better is found keep it
	for(; trace< searchIntervalEnd; trace++ )
	{
		float cut = getBoundValue(trace, axis);
		float costIfCutHere;
		if(*trace & 0x80000000)
		{
			intersectedCount--;
			leftCount++;
			costIfCutHere = 
				(leftCount+intersectedCount) * ((cut - limiMin) * costSteep + costBase) +
				(nObjects - leftCount) * ((limiMax - cut) * costSteep + costBase);
			if(leftCount == 0 || leftCount + intersectedCount == nObjects )
				costIfCutHere = FLT_MAX;
			if(costIfCutHere < minimumCostFound ){
				minimumCostFound = costIfCutHere;
				minimumCostPosition = trace + 1;
				minimumIntersected = intersectedCount;
				minimumLeft = leftCount;
				minimumCostCut = cut;
			}
		}
		else
		{
			costIfCutHere = 
				(leftCount+intersectedCount) * ((cut - limiMin) * costSteep + costBase) +
				(nObjects - leftCount) * ((limiMax - cut) * costSteep + costBase);
			if(leftCount == 0 || leftCount + intersectedCount == nObjects)
				costIfCutHere = FLT_MAX;
			if(costIfCutHere < minimumCostFound)
			{
				minimumCostFound = costIfCutHere;
				minimumCostPosition = trace;
				minimumIntersected = intersectedCount;
				minimumLeft = leftCount;
				minimumCostCut = cut;
			}
			intersectedCount++;
		}

	}

	// automatic termination, based on the cost estimation
	if(	minimumCostFound * 1.0001 >= nObjects * ((limiMax - limiMin) * costSteep + costBase))
	{
		// no appropriate splitting found, mark this axis as failed
		mask |= (010 << axis);
		if(mask != 070 && (nObjects != 1 || minimumCostFound < 10.0))
		{
			//if there is still a hopeful axis, do not split here, go on with another direction
			do{		axis = (axis + 1)%3;
			}while(mask & (010 << axis));
			build(nodeID, boundsArray, nObjects, limits, mask | axis);
			return;
		}
		// if all 3 have axes have already failed, stop here, make a leaf
		// save leaf length and objects list
		Intersectable** leafList = new Intersectable*[nObjects+1];
		Intersectable** leafLoader = leafList;
		*(unsigned int*)leafLoader = nObjects;
		leafLoader++;
		for(unsigned int* bubo = boundsArray;bubo < boundsArray + (nObjects << 1);bubo++)
			if(!(*bubo & 0x80000000))
			{
				*leafLoader = *(objects + *bubo);
				leafLoader++;
			}
		// make the node contain a pointer to the list
		((Intersectable ***)nodeTable)[nodeID] = leafList;
		delete boundsArray;
		//mark the node as a leaf
		((unsigned int*)nodeTable)[nodeID | nCacheLineNodes] |= (0x1 << (nodeID & nCacheLineNodes));
		//add orphaned nodes to free node list
		unsigned int lFree;
		unsigned int rFree;
		if(getFreeNodes(nodeID, lFree, rFree))
		{
			freeNodes->insert(lFree);
			freeNodes->insert(rFree);
		}
		return;
	}
	// grant another chance to previously failed directions:
	mask = 00;
	// insert a back-pointer if necessary
	nodeID = makePointer(nodeID);
	// store the splitting plane value in the node
	unsigned int bitfield = *(unsigned int*)(&minimumCostCut) & 0xfffffffc | axis;
	nodeTable[nodeID] =  *(float*)(&bitfield);
	// mark node as a non-leaf node
	*(unsigned int*)&nodeTable[nodeID | nCacheLineNodes] &= ~(0x1 << (nodeID & nCacheLineNodes));

	// allocate child nodes, pass the objects on to them
	unsigned int leftChildMade;
	unsigned int rightChildMade;

	// handle the two branches, the onr with less objects first
	bool firstBranch = false;
	bool rightBranchHasMoreObjects = (minimumLeft << 1) + minimumIntersected < nObjects;
	followChildren(nodeID, leftChildMade, rightChildMade);
	do
	{
		firstBranch = !firstBranch;
		if(firstBranch && rightBranchHasMoreObjects
			|| !firstBranch && !rightBranchHasMoreObjects)
		{
			// left branch
			if(minimumLeft + minimumIntersected)
			{
				// branch has objects
				// create new object extrema array
				BoundingBox diver = limits;
				limiMax = minimumCostCut;
				unsigned int * leftChildBounds = new unsigned int[(minimumLeft + minimumIntersected) << 1];
				unsigned int* copycatLeft = leftChildBounds;
				unsigned int* snoop = boundsArray;
				for(; snoop < minimumCostPosition; snoop++)
				{
					// for every mimimum left of cutting plane
					if(!(*snoop & 0x80000000))
					{
						// add one for the mimimum
						*copycatLeft = *snoop;
						copycatLeft++;
						// add one for the maximum
						*copycatLeft = *snoop | 0x80000000;
						copycatLeft++;
					}
				}
				unsigned char turnedAxis = axis;
				do{		turnedAxis = (turnedAxis + 1)%3;
				}while(mask & (010 << turnedAxis));
				build(leftChildMade, leftChildBounds, minimumLeft + minimumIntersected, limits, mask | turnedAxis);
				limits = diver;
			}
			else
			{
				// make empty leaf
				unsigned int empty = leftChildMade;
				(unsigned int *&)nodeTable[empty] = 0x0;
				*(unsigned int*)&nodeTable[empty | nCacheLineNodes] |= 0x1 << (empty & nCacheLineNodes);
				// add orphaned nodes to free list
				unsigned int eleLeft;
				unsigned int eleRight;
				if(getFreeNodes(empty, eleLeft, eleRight))
				{
					freeNodes->insert(eleLeft);
					freeNodes->insert(eleRight);
				}
			}
		}
		else
		{
			if(nObjects - minimumLeft)
			{
				BoundingBox diver = limits;
				limiMin = minimumCostCut;

				// create new bounds array
				unsigned int * rightChildBounds;
				rightChildBounds = new unsigned int[(nObjects - minimumLeft) << 1 ];

				unsigned int* copycatRight = rightChildBounds;
				unsigned int* snoop = minimumCostPosition;
				for(; snoop < boundsArray + (nObjects << 1); snoop++)
				{
					// for every maximum right of cutting plane
					if(*snoop & 0x80000000)
					{
						*copycatRight = *snoop & 0x7fffffff;
						copycatRight++;
						*copycatRight = *snoop;
						copycatRight++;
					}
				}

				unsigned int turnedAxis = axis;
				do{	turnedAxis = (turnedAxis + 1)%3;
				}while(mask & (010 << turnedAxis));
				build(rightChildMade, rightChildBounds, nObjects - minimumLeft, limits, mask | turnedAxis);
				limits = diver;
			}
			else
			{
				// empty leaf
				unsigned int empty = rightChildMade;
				(unsigned int *&)nodeTable[empty] = 0x0;
				*(unsigned int*)&nodeTable[empty | nCacheLineNodes] |= 0x1 << (empty & nCacheLineNodes);
				// orphaned nodes
				unsigned int eleLeft;
				unsigned int eleRight;
				if(getFreeNodes(empty, eleLeft, eleRight))
				{
					freeNodes->insert(eleLeft);
					freeNodes->insert(eleRight);
				}
			}
		}
	}while(firstBranch);
	
	delete boundsArray;
}

void KDTree::quickSort(unsigned int *lo0, unsigned int* hi0, unsigned char axis)
{
	if ( hi0 > lo0)
	{
		unsigned int* lo = lo0;
		unsigned int* hi = hi0;

         // establish partition element as the midpoint of the array.
		unsigned int* midPos =  
			lo0 + ( ((unsigned int)hi0 - (unsigned int)lo0) / sizeof(unsigned int) / 2);
		unsigned int midIndex = *midPos;

         // loop through the array until indices cross
		while( lo <= hi )
		{
            // find the first element that is greater than or equal to 
            // the partition element starting from the left Index.
			// getBoundValue(lo, axis) < mid ) && *lo != midIndex)
			while( ( lo < hi0 ) && ( compare(*lo, midIndex, axis)))
				++lo;

            // find an element that is smaller than or equal to 
            // the partition element starting from the right Index.
			//( getBoundValue(hi, axis) > mid ) && *hi != midIndex)
            while( ( hi > lo0 ) && ( compare(midIndex, *hi, axis)))
               --hi;            
			// if the indices have not crossed, swap
            if( lo <= hi )
			{
				unsigned int temp = *lo;
				*lo = *hi;
				*hi = temp;
				++lo;
				--hi;
			}         
		}
        // If the right index has not reached the left side of array
        // must now sort the left partition.
        if( lo0 < hi )
            quickSort( lo0, hi, axis);
		// If the left index has not reached the right side of array
        // must now sort the right partition.
		if( lo < hi0 )
            quickSort( lo, hi0, axis);
	}
}

KDTree::KDTree (Intersectable** iObjects, int nObjects){
	// take parameters
	this->nObjects		= nObjects;
	objects = iObjects;

	// determine bounding box
	createBoundingBox (iObjects, nObjects);

	// set up memory structure constants
	nCacheLineBytes = KDTREE_CACHE_LINE_BYTES;
	// a cache line has 2^n - 1 nodes, plus 4 bytes (1 node) for leaf bits
	nCacheLineNodes = nCacheLineBytes / sizeof(float) - 1;

	// size of cache line memory pool
	maxNCacheLines = nObjects * 15 / nCacheLineNodes + 1;
	// cache lines already allocated
	nCacheLines = 1;

	unsigned int maxPossibleDepth = maxNCacheLines * nCacheLineNodes;
	// allocate memory structures
	traverseStack = new TraverseStack[maxPossibleDepth];
	freeNodes = new	Heap<unsigned int>(maxPossibleDepth);

	// allocate cache line memory pool (large size, will be shrinked after the build)
	// nodes in cache line pool + offset correction
	poolNodeTable = (float*)malloc(sizeof(float) *
		( maxNCacheLines * (nCacheLineNodes + 1) + nCacheLineNodes + 1 ));
	nodeTable = (float*)(((unsigned int)poolNodeTable / nCacheLineBytes + 1) * nCacheLineBytes);

	// collect the initial object extremes to pass on to the build algorithm
	unsigned int* objectBoundaries = new unsigned int[nObjects << 1];
	for(unsigned int fill=0;fill<nObjects;fill++){
		objectBoundaries[fill << 1] = fill & 0x7fffffff;
		objectBoundaries[(fill << 1) + 1] = fill | 0x80000000;
	}

	// GO! GO! GO!
	build(0, objectBoundaries, nObjects, boundingBox, 0);

	// get rid of garbage
	// decrease cache line memory pool to fit actual memory need
	realloc(poolNodeTable, (nCacheLines + 1) * nCacheLineBytes);
	delete freeNodes;
}

unsigned int* KDTree::findBound(unsigned int *extremeArrayStart, unsigned int nBounds, float loc, unsigned char axis)
{
	while(nBounds > 1)
	{
		unsigned int wBounds = nBounds >> 1;
		if(getBoundValue(extremeArrayStart+wBounds, axis) < loc)
		{
			extremeArrayStart += wBounds;
			nBounds -= wBounds;
		}
		else
			nBounds = wBounds;
	}
	return extremeArrayStart;

}

/*!this is more complicated then one would predict. rules are:
- if values are significantly different, no problem
- the minimum of a patch is smaller than its maximum
- if a maximum and a minimum are near, the other extrema of the patches decide
- if all four extrema are near, the 2 ends of a patch have to be simultaneusly
smaller or bigger than the other two ends
*/
bool KDTree::compare(unsigned int aIndex, unsigned int bIndex, unsigned char axis)
{
	unsigned int caIndex = aIndex & 0x7fffffff;
	float aBegin;
	float aEnd;
	unsigned int cbIndex = bIndex & 0x7fffffff;
	float bBegin;
	float bEnd;

	switch(axis)
	{
	case 0:
		aBegin = objects[caIndex]->bbox.minPoint.x;
		aEnd = objects[caIndex]->bbox.maxPoint.x;
		bBegin = objects[cbIndex]->bbox.minPoint.x;
		bEnd = objects[cbIndex]->bbox.maxPoint.x;
		break;
	case 1:
		aBegin = objects[caIndex]->bbox.minPoint.y;
		aEnd = objects[caIndex]->bbox.maxPoint.y;
		bBegin = objects[cbIndex]->bbox.minPoint.y;
		bEnd = objects[cbIndex]->bbox.maxPoint.y;
		break;
	default:
		aBegin = objects[caIndex]->bbox.minPoint.z;
		aEnd = objects[caIndex]->bbox.maxPoint.z;
		bBegin = objects[cbIndex]->bbox.minPoint.z;
		bEnd = objects[cbIndex]->bbox.maxPoint.z;
	}

	if(aIndex & 0x80000000)
		if(bIndex & 0x80000000)
		{
			if(caIndex == cbIndex)
				return false;
			if(aEnd < bEnd)
				return true;
			if(bEnd < aEnd)
				return false;
			if(aBegin < bBegin)
				return true;
			if(bBegin < aBegin)
				return false;
			return caIndex < cbIndex;
		}
		else
		{
			if(caIndex == cbIndex)
				return false;

			if(aEnd < bBegin)
				return true;
			if(bBegin < aEnd)
				return false;
			if(aBegin < bEnd)
				return true;
			return caIndex < cbIndex;

		}
	else
		if(bIndex & 0x80000000)
		{
			if(caIndex == cbIndex)
				return true;

			if(bEnd < aBegin)
				return false;
			if(aBegin < bEnd)
				return true;
			if(bBegin < aEnd)
				return false;
			return caIndex < cbIndex;
		}
		else
		{
			if(caIndex == cbIndex)
				return false;

			if(aBegin < bBegin)
				return true;
			if(bBegin < aBegin)
				return false;
			if(aEnd < bEnd)
				return true;
			if(bEnd < aEnd)
				return false;
			return caIndex < cbIndex;

		}
}

inline unsigned int KDTree::makePointer(unsigned int originalNode)
{
	unsigned int subPos = originalNode & nCacheLineNodes;		// position within cache line
	unsigned int supPos = originalNode & ~nCacheLineNodes;		// index of cache line
	subPos = (subPos << 1) + 1;									// child position
	if(subPos < nCacheLineNodes)								// within cache line
	{
		return originalNode;
	}
	else														// find free node
	{
		unsigned int emptyNode;
		if(!freeNodes->isEmpty())
		{
			emptyNode = freeNodes->removeMin();
		}
		else
		{
			//allocate a new cache line
			emptyNode = nCacheLines * (nCacheLineNodes + 1);	// root of new cache line
			nCacheLines++;
			if(nCacheLines > maxNCacheLines)
			{
				maxNCacheLines *= 1.1;
				float * largerPoolNodeTable = (float*)malloc(
					sizeof(float) *	( maxNCacheLines * (nCacheLineNodes + 1) + nCacheLineNodes + 1 ));
				float* largerNodeTable = (float*)(((unsigned int)largerPoolNodeTable / nCacheLineBytes + 1) * nCacheLineBytes);
				memcpy(largerNodeTable, nodeTable, nCacheLines * nCacheLineBytes);
				nodeTable = largerNodeTable;
				free(poolNodeTable);
				poolNodeTable = largerPoolNodeTable;
			}
		}
		// mark as non-leaf (being last-row and non-leaf means pointer node)
		*(unsigned int*)&nodeTable[originalNode | nCacheLineNodes] &= ~(0x1 << (originalNode & nCacheLineNodes));
		// insert index of empty node
		((unsigned int*)nodeTable)[originalNode] = emptyNode;
		return emptyNode;
	}
}
