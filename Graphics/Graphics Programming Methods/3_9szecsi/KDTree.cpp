// KDTree.cpp: implementation of the KDTree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KDTree.hpp"
#include <iostream>

bool KDTree::traverse (const Ray& ray, HitRec& hitRec, float minT, float maxT)
{
	static const float epsilonTolerance = 1e-10f;
	static const float epsilonDropBranch = 1e-4f;
	static const float epsilonDivisionByZero = 1e-10f;

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

		if(isLeaf)
		{ //leaf
			Intersectable** leafList = *(Intersectable***)&nodeTable[tNode];
			if(leafList)
			{//leaf contains some objects
				unsigned int leafListLength = (unsigned int)*leafList;
				for (Intersectable** cpc = ++leafList; cpc < leafList + leafListLength; cpc++) {
					float depth;
					if (ray.id != (*cpc)->lastTestedRayId) // if this triangle was not tested for this ray before
						(*cpc)->intersect (ray, depth, rayMin, rayMax);
					else
						depth = (*cpc)->lastTestedRayResult->depth;
		
					if ((*cpc)->lastTestedRayResult->isIntersect && 
							depth < hitRec.depth)
						if (depth > rayMin - epsilonTolerance && depth < rayMax + epsilonTolerance) 
							hitRec = *((*cpc)->lastTestedRayResult);
				}
			
				if(hitRec.isIntersect)
					return true;
				else
					if(stackPointer >= 0)
					{ //have not found anything on this branch, pop the other one from stack
						rayMin = traverseStack[stackPointer].rayMin;
						rayMax = traverseStack[stackPointer].rayMax;
						tNode = traverseStack[stackPointer].tNode;
						stackPointer--;
					}
					else
						return false;
			}
			else
			{//leaf is empty
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
		else
		{//not a leaf, go deeper
			float cutPlane = nodeTable[tNode];
			unsigned char axis = *(unsigned int*)(&cutPlane) & 0x00000003;
			unsigned int bitfield = *(unsigned int*)(&nodeTable[tNode]) & 0xfffffffc;
			cutPlane = *(float*)(&bitfield);

			float origCoord;
			float dirCoord;
			switch(axis & 07)
			{
			case 0: origCoord = ray.origin.x;
					dirCoord = ray.dir.x;
					break;
			case 1: origCoord = ray.origin.y;
					dirCoord = ray.dir.y;
					break;
			case 2: origCoord = ray.origin.z;
					dirCoord = ray.dir.z;
			}

			register float t;
			if(dirCoord <= epsilonDivisionByZero && dirCoord >= -epsilonDivisionByZero)
				t = (cutPlane - origCoord) * FLT_MAX;
			else
				t =	(cutPlane - origCoord) / dirCoord;
	
			unsigned int nearNode, farNode;
			if(origCoord + epsilonDivisionByZero < cutPlane)
			{
				nearNode = leftChild;
				farNode = rightChild;			
			}
			else if(cutPlane + epsilonDivisionByZero < origCoord)
			{
				nearNode = rightChild;
				farNode = leftChild;
			}
			else if(dirCoord < 0)
			{
				nearNode = leftChild;
				farNode = rightChild;			
			}
			else
			{
				nearNode = rightChild;
				farNode = leftChild;
			}

			if(t <= 0 || rayMax + epsilonDropBranch < t )
				//whole interval on near cell
				tNode = nearNode;
			else
			{
				if(t < rayMin - epsilonDropBranch)
					//whole interval on far cell
					tNode = farNode;
				else
				{//intersection
					//push far branch to stack
					stackPointer++;
					traverseStack[stackPointer].rayMin = t;
					traverseStack[stackPointer].rayMax = rayMax;
					traverseStack[stackPointer].tNode = farNode;
					//near branch is next to traverse
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
	if(nodeTable[nodeID] <  FLT_MAX - KDTREE_EPSILON)
		deleteLeaves(rc);
}

bool KDTree::followChildren(unsigned int& parent, unsigned int &lefty, unsigned int &righty)
{
	unsigned int subPos = parent & nCacheLineNodes;
	unsigned int supPos = parent & ~nCacheLineNodes;
	unsigned int leafBit = ((*(unsigned int *)&nodeTable[parent | nCacheLineNodes]) >> subPos) & 0x1;
	if(leafBit)
		return false;
	subPos = (subPos << 1) + 1;
	if(subPos < nCacheLineNodes)
	{
		lefty = (parent & ~nCacheLineNodes) | subPos;
		righty = (parent & ~nCacheLineNodes) | subPos + 1;
		return true;
	}
	else
	{
		unsigned int sps = supPos + subPos - nCacheLineNodes + 1;
		lefty = sps * (nCacheLineNodes + 1);
		righty = (sps + 1) * (nCacheLineNodes + 1);
		if(sps + 1 < nCacheLines)
			return true;
		else
		{
			parent = *(int*)(nodeTable + parent);

			unsigned int wsubPos = parent & nCacheLineNodes;
			unsigned int wsupPos = parent & ~nCacheLineNodes;
			wsubPos = (wsubPos << 1) + 1;
			if(wsubPos < nCacheLineNodes)
			{
				lefty = wsupPos | wsubPos;
				righty = wsupPos | wsubPos + 1;
				return true;
			}
			else
			{
				unsigned int wsps = wsupPos + wsubPos - nCacheLineNodes + 1;
				lefty = wsps * (nCacheLineNodes + 1);
				righty = (wsps + 1) * (nCacheLineNodes + 1);
				return true;
			}
		}
	}
}

bool KDTree::makeChildren(unsigned int parent, unsigned int& lefty, unsigned int& righty)
{
	unsigned int subPos = parent & nCacheLineNodes;
	unsigned int supPos = parent & ~nCacheLineNodes;
	subPos = (subPos << 1) + 1;
	if(subPos < nCacheLineNodes)
	{
		lefty = (parent & ~nCacheLineNodes) | subPos;
		righty = (parent & ~nCacheLineNodes) | subPos + 1;
		return true;
	}
	else
	{
		unsigned int sps = supPos + subPos - nCacheLineNodes + 1;
		lefty = sps * (nCacheLineNodes + 1);
		righty = (sps + 1) * (nCacheLineNodes + 1);
		if(sps + 1 < nCacheLines)
			return true;
		else
		{
			if(nFreeNodes < 2)
				return false;
			nFreeNodes -= 2;
			lefty = freeNodes[firstFreeNodeIndex];
			firstFreeNodeIndex = (firstFreeNodeIndex+1) % maxFreeNodes;
			righty = freeNodes[firstFreeNodeIndex];
			firstFreeNodeIndex = (firstFreeNodeIndex+1) % maxFreeNodes;
			return true;
		}
	}
}

bool KDTree::getFreeNodes(unsigned int parent, unsigned int& lefty, unsigned int& righty)
{
	unsigned int subPos = parent & nCacheLineNodes;
	unsigned int supPos = parent & ~nCacheLineNodes;
	subPos = (subPos << 1) + 1;
	if(subPos < nCacheLineNodes)
	{
		unsigned int grandSubPos = ((subPos + 1) << 1) + 2;
		if(grandSubPos >= nCacheLineNodes)
		{
			unsigned int sps = supPos + grandSubPos - nCacheLineNodes + 1;
			if(sps + 1 >= nCacheLines)
			return false;
		}
		lefty = (parent & ~nCacheLineNodes) | subPos;
		righty = (parent & ~nCacheLineNodes) | subPos + 1;;
		return true;
	}
	else
	{
		unsigned int sps = supPos + subPos - nCacheLineNodes + 1;
		lefty = sps * (nCacheLineNodes + 1);
		righty = (sps + 1) * (nCacheLineNodes + 1);
		if(sps + 1 < nCacheLines)
			return true;
		else
			return false;
	}
}

bool KDTree::isLeaf(unsigned int xnode)
{
	return ((*(unsigned int *)&nodeTable[xnode | nCacheLineNodes]) >> (xnode & nCacheLineNodes)) & 0x1;
}

float KDTree::getBoundValue(unsigned int * bies, unsigned char axis)
{
	//if MSB of bies is 1, take the maximum
	//if MSB of bies is 0, take the minimum
	unsigned int index = *bies;
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
	//separate the axis parameter to axis and mask
	unsigned char axis = axisNmask & 07;
	unsigned char mask = axisNmask & 070;

	//calculate some values for the cost function
	float costBase;
	float costSteep;

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
	default:
		printf("WARNING!, Should never happen");
	}

	//some shorthand for this long-long thingie
	float& limiMin = axis?((axis==1)?limits.minPoint.y:limits.minPoint.z):limits.minPoint.x;
	float& limiMax = axis?((axis==1)?limits.maxPoint.y:limits.maxPoint.z):limits.maxPoint.x;

	//find the value for the spatial median.... easy
	float spatialMedian = (limiMax + limiMin) / 2.0f;

	//sort the references to the extrema of the patches
	//we will need this to find the best splitting plane
	medianSort(boundsArray, boundsArray + (nObjects << 1) - 1, axis);

	//find the positionin the array corresponding to the spatial median value
	unsigned int* spatialMedianPosition = findBound(boundsArray, nObjects << 1, spatialMedian, axis);

	//find object median position (middle of array, surprise-surprise), and value
	unsigned int* objectMedianPosition = boundsArray + nObjects;
	float objectMedian = getBoundValue(objectMedianPosition, axis);

	//find the value for the left and right "cutting off empty space" (COES) planes 
	float minCut;
	float maxCut;

	switch(axis)
	{
	case 0:
		minCut = objects[boundsArray[0] & 0x7fffffff]->bbox.minPoint.x;
		maxCut = objects[boundsArray[(nObjects << 1)-1] & 0x7fffffff]->bbox.maxPoint.x;
		break;
	case 1:
		minCut = objects[boundsArray[0] & 0x7fffffff]->bbox.minPoint.y;
		maxCut = objects[boundsArray[(nObjects << 1)-1] & 0x7fffffff]->bbox.maxPoint.y;
		break;
	case 2:
		minCut = objects[boundsArray[0] & 0x7fffffff]->bbox.minPoint.z;
		maxCut = objects[boundsArray[(nObjects << 1)-1] & 0x7fffffff]->bbox.maxPoint.z;
		break;
	default:
		printf("WARNING!, Should never happen");
	}
	if(maxCut > limiMax)
		maxCut = limiMax;
	if(minCut < limiMin)
		minCut = limiMin;

	// find the estimated cost of ray casting after COES
	float minCutCost = ((limiMax - minCut) * costSteep + costBase) * (nObjects);
	float maxCutCost = ((maxCut - limiMin) * costSteep + costBase) * (nObjects);

	if(minCutCost < 0) minCutCost = FLT_MAX;
	if(maxCutCost < 0) maxCutCost = FLT_MAX;

	//creep through array, maintaining the number of intersected and completely passed pathes
	int leftCount = 0;
	int intersectedCount = 0;

	//we may limit the search to a partition of the array or not
	//'not' is slower to evaluate, but a bit faster at ray casting
	unsigned int* medianIntervalStart;
	unsigned int* medianIntervalEnd;

	//full range optimum search
	medianIntervalStart = boundsArray + 1;
	medianIntervalEnd = boundsArray + (nObjects << 1) - 1;

	//is left or right COES better? the winner is our best candidate so far
	float minimumCostFound;
	float minimumCostCut;
	unsigned int* minimumCostPosition;
	int minimumIntersected = 0;
	int minimumLeft = 0;
	if(minCutCost < maxCutCost)
	{
		minimumCostFound = minCutCost;
		minimumCostPosition = boundsArray;
		minimumCostCut = minCut;
		minimumIntersected = 0;
		minimumLeft = 0;
	}
	else
	{
		minimumCostFound = maxCutCost;
		minimumCostPosition = boundsArray + (nObjects << 1);
		minimumCostCut = maxCut;
		minimumIntersected = 0;
		minimumLeft = nObjects;
	}

	//start creeping... count patches
	unsigned int* trace = boundsArray;
	for(; trace< medianIntervalStart; trace++ )
	{
		if(*trace & 0x80000000)
		{
			intersectedCount--;
			leftCount++;
		}
		else
			intersectedCount++;
	}
	//here comes the section where we estimate the cost to find the best cut
	for(; trace< medianIntervalEnd; trace++ )
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

	//termination. cost-based
	if(	minimumCostFound * 1.0001 >= nObjects * ((limiMax - limiMin) * costSteep + costBase))
	{
		//no split found, mark this axis as failed
		mask |= (010 << axis);

		if(mask != 070 && (nObjects != 1 || minimumCostFound < 10.0))
		{//if there is still a hopeful axis, do not split here, go on with the other direction
			do{		axis = (axis + 1)%3;
			}while(mask & (010 << axis));
			build(nodeID, boundsArray, nObjects, limits, mask | axis);
			return;
		}
			//if all 3 have abandoned us, stop here, make a leaf

			//save leaf length and patch list
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

			//make the node contain a pointer to the list
			(Intersectable **&)nodeTable[nodeID] = leafList;
			delete boundsArray;

			//mark the node as a leaf
			*(unsigned int*)&nodeTable[nodeID | nCacheLineNodes] |= (0x1 << (nodeID & nCacheLineNodes));
			//add orphaned nodes to free node list
			unsigned int lFree;
			unsigned int rFree;
			if(getFreeNodes(nodeID, lFree, rFree))
			{
				nFreeNodes += 2;
				freeNodes[lastFreeNodeIndex] = lFree;
				lastFreeNodeIndex = (lastFreeNodeIndex + 1) % maxFreeNodes;
				freeNodes[lastFreeNodeIndex] = rFree;
				lastFreeNodeIndex = (lastFreeNodeIndex + 1) % maxFreeNodes;
			}
			return;

	}
	//grant another chance to previously failed directions:
	mask = 00;
	//insert a back-pointer if necessary
	nodeID = makePointer(nodeID);

	//store the splitting plane value in the node
	unsigned int bitfield = *(unsigned int*)(&minimumCostCut) & 0xfffffffc | axis;
	nodeTable[nodeID] =  *(float*)(&bitfield);
	//mark node as a non-leaf node
	*(unsigned int*)&nodeTable[nodeID | nCacheLineNodes] &= ~(0x1 << (nodeID & nCacheLineNodes));

	//allocate child nodes, pass the objects on to them
	unsigned int leftChildMade;
	unsigned int rightChildMade;
	if(makeChildren(nodeID, leftChildMade, rightChildMade))
	{
		if(minimumLeft + minimumIntersected)
		{
			BoundingBox diver = limits;
			limiMax = minimumCostCut;
			unsigned int * leftChildBounds = new unsigned int[(minimumLeft + minimumIntersected) << 1];
			unsigned int* copycatLeft = leftChildBounds;
			unsigned int* snoop = boundsArray;
			for(; snoop < minimumCostPosition; snoop++)
			{
				if(!(*snoop & 0x80000000))
				{
					*copycatLeft = *snoop;
					copycatLeft++;
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
			unsigned int empty = leftChildMade;
			(unsigned int *&)nodeTable[empty] = 0x0;
			*(unsigned int*)&nodeTable[empty | nCacheLineNodes] |= 0x1 << (empty & nCacheLineNodes);
			unsigned int eleLeft;
			unsigned int eleRight;
			if(getFreeNodes(empty, eleLeft, eleRight))
			{
				nFreeNodes += 2;
				freeNodes[lastFreeNodeIndex] = eleLeft;
				lastFreeNodeIndex = (lastFreeNodeIndex + 1) % maxFreeNodes;
				freeNodes[lastFreeNodeIndex] = eleRight;
				lastFreeNodeIndex = (lastFreeNodeIndex + 1) % maxFreeNodes;
			}
		}

		if(nObjects - minimumLeft)
		{
			BoundingBox diver = limits;
			limiMin = minimumCostCut;

			unsigned int * rightChildBounds;
			rightChildBounds = new unsigned int[(nObjects - minimumLeft) << 1 ];

			unsigned int* copycatRight = rightChildBounds;
			unsigned int* snoop = minimumCostPosition;
			for(; snoop < boundsArray + (nObjects << 1); snoop++)
			{
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
			unsigned int empty = rightChildMade;
			(unsigned int *&)nodeTable[empty] = 0x0;
			*(unsigned int*)&nodeTable[empty | nCacheLineNodes] |= 0x1 << (empty & nCacheLineNodes);
			unsigned int eleLeft;
			unsigned int eleRight;
			if(getFreeNodes(empty, eleLeft, eleRight))
			{
				nFreeNodes += 2;
				freeNodes[lastFreeNodeIndex] = eleLeft;
				lastFreeNodeIndex = (lastFreeNodeIndex + 1) % maxFreeNodes;
				freeNodes[lastFreeNodeIndex] = eleRight;
				lastFreeNodeIndex = (lastFreeNodeIndex + 1) % maxFreeNodes;
			}
		}
	}
	else //terminate
	{
		// "Node table overflow";
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
		(Intersectable **&)nodeTable[nodeID] = leafList;
		*(unsigned int*)&nodeTable[nodeID | nCacheLineNodes] |= (0x1 << (nodeID & nCacheLineNodes));
	}
	
	delete boundsArray;
}

void KDTree::medianSort(unsigned int *lo0, unsigned int* hi0, unsigned char axis)
{

	if ( hi0 > lo0)
	{
		unsigned int* lo = lo0;
		unsigned int* hi = hi0;

         // establishing partition element as the midpoint of the array.
		unsigned int* midPos =  
			lo0 + ( ((unsigned int)hi0 - (unsigned int)lo0) / sizeof(unsigned int) / 2);
		unsigned int midIndex = *midPos;
		float mid = getBoundValue(midPos, axis);

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
            medianSort( lo0, hi, axis);
		// If the left index has not reached the right side of array
        // must now sort the right partition.
		if( lo < hi0 )
            medianSort( lo, hi0, axis);
	}
}

KDTree::KDTree (Intersectable** iObjects, int nObjects){
	//take parameters
	this->nObjects		= nObjects;
	objects = iObjects;

	//determine bounding box
	GetBoundingBox (boundingBox, iObjects, nObjects);

	//set up memory structure constants
	nCacheLineBytes = KDTREE_CACHE_LINE_BYTES;
	nCacheLineNodes = nCacheLineBytes / sizeof(float) - 1;

	nCacheLines = nObjects * 18 / nCacheLineNodes + 1;

	//allocate memory structures
	unsigned int tnnodes = nCacheLines * nCacheLineNodes;
	for(depth=1; tnnodes; depth++)
		tnnodes = tnnodes >> 1;
	traverseStack = new TraverseStack[0x1 << depth];

	lastFreeNodeIndex = 0;
	firstFreeNodeIndex = 0;
	maxFreeNodes = nCacheLines * (nCacheLineNodes + 1);
	freeNodes = new unsigned int[maxFreeNodes];

	// nodes in cachelines + offset correction
	eNodeTable = new float[nCacheLines * (nCacheLineNodes + 1)
									+ nCacheLineNodes + 1];
	nodeTable = (float*)(((unsigned int)eNodeTable / nCacheLineBytes + 1) * nCacheLineBytes);

	//collect the initial object extremes to pass on to the build algorithm
	unsigned int* objectBoundaries = new unsigned int[nObjects << 1];
	for(unsigned int fill=0;fill<nObjects;fill++){
		objectBoundaries[fill << 1] = fill & 0x7fffffff;
		objectBoundaries[(fill << 1) + 1] = fill | 0x80000000;
	}

	//GO! GO! GO!
	build(0, objectBoundaries, nObjects, boundingBox, 0);
	//get rid of garbage
	delete freeNodes;
}

unsigned int* KDTree::findBound(unsigned int *bar, unsigned int nBounds, float loc, unsigned char axis)
{
	while(nBounds > 1)
	{
		unsigned int wBounds = nBounds >> 1;
		if(getBoundValue(bar+wBounds, axis) < loc)
		{
			bar += wBounds;
			nBounds -= wBounds;
		}
		else
			nBounds = wBounds;
	}
	return bar;

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
			if(aEnd < bEnd - KDTREE_BOUNDS_EPSILON)
				return true;
			if(bEnd < aEnd - KDTREE_BOUNDS_EPSILON)
				return false;
			if(aBegin < bBegin - KDTREE_BOUNDS_EPSILON)
				return true;
			if(bBegin < aBegin - KDTREE_BOUNDS_EPSILON)
				return false;
			return caIndex < cbIndex;
		}
		else
		{
			if(caIndex == cbIndex)
				return false;

			if(aEnd < bBegin - KDTREE_BOUNDS_EPSILON)
				return true;
			if(bBegin < aEnd - KDTREE_BOUNDS_EPSILON)
				return false;
			if(aBegin < bEnd - KDTREE_BOUNDS_EPSILON)
				return true;
			return caIndex < cbIndex;

		}
	else
		if(bIndex & 0x80000000)
		{
			if(caIndex == cbIndex)
				return true;

			if(bEnd < aBegin - KDTREE_BOUNDS_EPSILON)
				return false;
			if(aBegin < bEnd - KDTREE_BOUNDS_EPSILON)
				return true;
			if(bBegin < aEnd - KDTREE_BOUNDS_EPSILON)
				return false;
			return caIndex < cbIndex;
		}
		else
		{
			if(caIndex == cbIndex)
				return false;

			if(aBegin < bBegin - KDTREE_BOUNDS_EPSILON)
				return true;
			if(bBegin < aBegin - KDTREE_BOUNDS_EPSILON)
				return false;
			if(aEnd < bEnd - KDTREE_BOUNDS_EPSILON)
				return true;
			if(bEnd < aEnd - KDTREE_BOUNDS_EPSILON)
				return false;
			return caIndex < cbIndex;

		}
}

inline unsigned int KDTree::makePointer(unsigned int oNode)
{
	unsigned int subPos = oNode & nCacheLineNodes;
	unsigned int supPos = oNode & ~nCacheLineNodes;
	subPos = (subPos << 1) + 1;
	if(subPos < nCacheLineNodes)
	{
		return oNode;
	}
	else
	{
		unsigned int sps = supPos + subPos - nCacheLineNodes + 1;
		if(sps + 1 < nCacheLines)
			return oNode;
		else
		{
			if(nFreeNodes < 1)
				exit(-1);
			*(unsigned int*)&nodeTable[oNode | nCacheLineNodes] &= ~(0x1 << (oNode & nCacheLineNodes));
			unsigned int cNode = freeNodes[firstFreeNodeIndex];
			nFreeNodes--;
			firstFreeNodeIndex = (firstFreeNodeIndex+1) % maxFreeNodes;
			nodeTable[oNode] = *(float*)(&cNode);
			return cNode;
		}
	}
}

