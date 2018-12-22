////////////////////////////////////////////////////////////////////////////
//	Copyright : Sebastien Schertenleib
//	          : VRLab-EPFL
//
//	Email :Sebastien.Schertenleib@epfl.ch
//
//	This source code is a part of the Game Programming Gems 5 Source Code
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
////////////////////////////////////////////////////////////////////////////



#include "vhdABTTree.h"
#include "vhdSceneConverterABTTree.h"

#include <iostream>
#include <math.h>

#define BITLEAF 0
#define LX 1
#define LY 2
#define LZ 3
#define RX 4
#define RY 5
#define RZ 6
#define BITFLAG 7

#define MASK_ISLEAF 0x01
#define MASK_NULL	0x00
#define MASK_LX		0x80 
#define MASK_LY		0x40 
#define MASK_LZ		0x20 
#define MASK_RX		0x10 
#define MASK_RY		0x08 
#define MASK_RZ		0x04 
#define MASK_LL		0x02 
#define MASK_RL		0x01 




uint32 
vhdABTTree::_computeSubTreeDepth()
{
	unsigned int nResult(0);
	unsigned int nCur(vhdABTTree::nCacheLineBytes);
	nCur/=4 ;
	while (nCur > 2)
	{
		//we divide in two
		nCur/=2 ;
		++nResult;
	}
	return nResult;
}

//set up memory structure constants
#define ABTTREE_CACHE_LINE_BYTES 64

uint32 vhdABTTree::nCacheLineBytes = ABTTREE_CACHE_LINE_BYTES;
uint32 vhdABTTree::nCacheLineNodes = vhdABTTree::nCacheLineBytes / sizeof(vhdABTTreeNode);
uint32 vhdABTTree::nDepth=vhdABTTree::_computeSubTreeDepth();
uint32 vhdABTTree::nSubTreeId = pow(2,vhdABTTree::nDepth-1) -1;
const uint32 vhdABTTree::nArrayDeep = 180;




void 
vhdABTTree::visit(unsigned int uiCurrentPos, unsigned int uiIndex, const vhdAxisAlignedBox &bbox)
{
	return;
}


vhdABTTree::vhdABTTree()
{
	
	_nFirstFreeCacheLineIndex = 0;
	_nLeafIndex = 0;
	funcIsVisible= vhdABTTree::_isVisible;
	funcVisit = vhdABTTree::visit;
}


void
vhdABTTree::create(uint32 nObjects)
{
	_nFirstFreeCacheLineIndex = 0;
	nCacheLines = nObjects * vhdABTTree::nArrayDeep / nCacheLineNodes + 1;
	
	uint32 maxFreeNodes = vhdABTTree::nCacheLines * (vhdABTTree::nCacheLineNodes + 1);
	_pNode= new vhdABTTreeNode[maxFreeNodes];
	memset(_pNode,0,maxFreeNodes*sizeof(vhdABTTreeNode));
	
	_pLeaf = new vhdABTTreeLeaf[nObjects];
	_nLeafIndex = 0;

	vhdAxisAlignedBox boxLeft;
	vhdAxisAlignedBox boxRight;
	
	//we are looking for getting the absolute bounding box.
	//id 0 contains all geometry
	vhdSceneConverterABTTree::getSingleton().getChildren(vhdSceneConverterABTTree::getSingleton().pDataRootNode ,0, boxLeft, boxRight);
	_bbox = vhdSceneConverterABTTree::getSingleton().findGlobalAABB(vhdSceneConverterABTTree::getSingleton().pDataRootNode->listAABB);
	
	_makeNodeDataFlags(0,_bbox,boxLeft, boxRight);
	
	//go with the build
	subTreeBuild(0, _bbox);
}

void
vhdABTTree::destroy()
{
	delete[] _pLeaf;
	delete[] _pNode;
}

void
vhdABTTree::create(unsigned int nCacheLineSize, unsigned int nLeafSize  )
{
	_nFirstFreeCacheLineIndex = 0;
	nCacheLines = nCacheLineSize;
	
	_pNode= new vhdABTTreeNode[nCacheLineSize];
	
	_pLeaf = new vhdABTTreeLeaf[nLeafSize];
	_nLeafIndex = nLeafSize;
}


byte
vhdABTTree::_extendLeft(float fMinExtend, float fMinParent, float fMaxParent)
{
	return (byte) (255* (   (fMinExtend - fMinParent) / ( fMaxParent - fMinParent ) ) );
}
byte
vhdABTTree::_extendRight(float fMaxExtend, float fMinParent, float fMaxParent)
{
	return (byte) (255* (   ( fMaxParent - fMaxExtend ) / ( fMaxParent - fMinParent ) ) );
}

void 
vhdABTTree::_makeNodeDataFlags(uint32 nCurrentChildPosition, const vhdAxisAlignedBox &box, const vhdAxisAlignedBox &boxLeft, const vhdAxisAlignedBox & boxRight)
{
	vhdVector3 minBox  = box.getMinimum();
	vhdVector3 maxBox  = box.getMaximum();
	
	vhdVector3 minBoxLeft = boxLeft.getMinimum();
	vhdVector3 maxBoxLeft = boxLeft.getMaximum();
	vhdVector3 minBoxRight = boxRight.getMinimum();
	vhdVector3 maxBoxRight = boxRight.getMaximum();

	#ifdef TREE_DEBUG
	std::cout << "vhdABTTree::_makeNodeDataFlags " << box << "  " << boxLeft << "  " << boxRight << std::endl;
	#endif

	if (minBox.x == minBoxLeft.x)
	{
		_pNode[nCurrentChildPosition].node.data[BITFLAG] = _pNode[nCurrentChildPosition].node.data[BITFLAG] |  MASK_LX;
		_pNode[nCurrentChildPosition].node.data[LX] = _extendLeft(minBoxRight.x, minBox.x, maxBox.x);
	}
	else
	{
		_pNode[nCurrentChildPosition].node.data[LX] = _extendLeft(minBoxLeft.x, minBox.x, maxBox.x);
	}
	if (minBox.y ==  minBoxLeft.y)
	{
		_pNode[nCurrentChildPosition].node.data[BITFLAG] = _pNode[nCurrentChildPosition].node.data[BITFLAG] |  MASK_LY;
		_pNode[nCurrentChildPosition].node.data[LY] = _extendLeft(minBoxRight.y, minBox.y, maxBox.y);
	}
	else
	{
		_pNode[nCurrentChildPosition].node.data[LY] = _extendLeft(minBoxLeft.y, minBox.y, maxBox.y);
	}
	if (minBox.z ==  minBoxLeft.z)
	{
		_pNode[nCurrentChildPosition].node.data[BITFLAG] = _pNode[nCurrentChildPosition].node.data[BITFLAG] |  MASK_LZ;
		_pNode[nCurrentChildPosition].node.data[LZ] = _extendLeft(minBoxRight.z, minBox.z, maxBox.z);
	}
	else
	{
		_pNode[nCurrentChildPosition].node.data[LZ] = _extendLeft(minBoxLeft.z, minBox.z, maxBox.z);
	}
	if (maxBox.x ==  maxBoxLeft.x)
	{
		_pNode[nCurrentChildPosition].node.data[BITFLAG] = _pNode[nCurrentChildPosition].node.data[BITFLAG] |  MASK_RX;
		_pNode[nCurrentChildPosition].node.data[RX] = _extendRight(maxBoxRight.x,  minBox.x, maxBox.x);
	}
	else
	{
		_pNode[nCurrentChildPosition].node.data[RX] =  _extendRight(maxBoxLeft.x,  minBox.x, maxBox.x);
	}
	if (maxBox.y ==  maxBoxLeft.y)
	{
		_pNode[nCurrentChildPosition].node.data[BITFLAG] = _pNode[nCurrentChildPosition].node.data[BITFLAG] |  MASK_RY;
		_pNode[nCurrentChildPosition].node.data[RY] = _extendRight(maxBoxRight.y,  minBox.y, maxBox.y);
	}
	else
	{
		_pNode[nCurrentChildPosition].node.data[RY] = _extendRight(maxBoxLeft.y,  minBox.y, maxBox.y);
	}
	if (maxBox.z ==  maxBoxLeft.z)
	{
		_pNode[nCurrentChildPosition].node.data[BITFLAG] = _pNode[nCurrentChildPosition].node.data[BITFLAG] |  MASK_RZ;
		_pNode[nCurrentChildPosition].node.data[RZ] = _extendRight(maxBoxRight.z,  minBox.z, maxBox.z);
	}
	else
	{
		_pNode[nCurrentChildPosition].node.data[RZ] = _extendRight(maxBoxLeft.z,  minBox.z, maxBox.z);
	}

	_pNode[nCurrentChildPosition].node.data[BITLEAF] = MASK_NULL;
}
		


void
vhdABTTree::_makeChildren(ListElement & listElement, const vhdAxisAlignedBox &box, uint32 nCurrentPosition,uint32 nCurrentChildPosition, uint32 index, bool bLeft )
{
	//get the two box bbox.
	vhdAxisAlignedBox boxLeft,boxRight; 
	
	vhdSceneConverterABTTree::vhdSceneABTTreeNode *pCurNode= NULL;
	if (bLeft) 
	{
		pCurNode=vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentPosition].pLeft;
	}
	else
	{
		pCurNode=vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentPosition].pRight;
	}
	pCurNode->pLeft=pCurNode->pRight=NULL;
	
	std::cout << "Make Children               : " << nCurrentChildPosition << std::endl;
	
	vhdSceneConverterABTTree::getSingleton().getChildren(pCurNode ,nCurrentChildPosition,boxLeft, boxRight);
	
	
	_makeNodeDataFlags(nCurrentChildPosition, box, boxLeft,boxRight); 
	listElement.push_back(Element(index, box));	
}



void
vhdABTTree::_makeLeaf(uint32 nCurrentPosition,uint32 nCurrentChildPosition ,bool bLeft)
{
	_pNode[nCurrentChildPosition].leaf.index = _nLeafIndex << MASK_ISLEAF;
	_pNode[nCurrentChildPosition].leaf.index = _pNode[nCurrentChildPosition].leaf.index | MASK_ISLEAF;
	_pNode[nCurrentChildPosition].leaf.callback = NULL;
	++_nLeafIndex;


	//we insert the current status in our map set to null so means leaf!!!
	vhdSceneConverterABTTree::vhdSceneABTTreeNode data;
	if (bLeft)
	{
		data.listAABB = vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentPosition].pLeft->listAABB;
	}
	else
	{
		data.listAABB = vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentPosition].pRight->listAABB;
	}
	data.pLeft   = NULL;
	data.pRight  = NULL;

	std::cout << "Make Leaf" << std::endl;
	std::cout << "Current Child Position      : " << nCurrentChildPosition <<  std::endl;
	std::cout << "Leaf Index                  : " << _nLeafIndex << std::endl;
	std::cout << "Lead ID                     : "; 
	//loop
	vhdSceneConverterABTTree::ListAABB::iterator it = data.listAABB.begin();
	std::cout << it->strPath;
	++it;
	for ( it; it!= data.listAABB.end(); ++it)
	{
		std::cout << " , " << it->strPath;
	}
	std::cout << std::endl << std::endl;
	
	vhdSceneConverterABTTree::getSingleton().mapNodeChild.insert(vhdSceneConverterABTTree::MapNodeChild::value_type(nCurrentChildPosition,data));
}


bool 
vhdABTTree::_needToSplit(const vhdAxisAlignedBox &box, uint32 nCurrentPos, bool bLeft)
{
	//we check with this companion methods
	vhdSceneConverterABTTree::vhdSceneABTTreeNode *pCurNode= NULL;
	if (bLeft) 
	{
		pCurNode=vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentPos].pLeft;
	}
	else
	{
		pCurNode=vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentPos].pRight;
	}
	
	if (pCurNode )
	{
		//2 for each sub-node (left/right)
		if(pCurNode->listAABB.size()>4)	
		{
			return true;
		}
	}	
	return false;
}



void
vhdABTTree::_computeChildBox(const vhdAxisAlignedBox & boxParent, vhdAxisAlignedBox &boxLeft,vhdAxisAlignedBox &boxRight, uint32 nCurrentPos )
{
	vhdVector3 vecMinParent = boxParent.getMinimum();
	vhdVector3 vecMaxParent = boxParent.getMaximum();
	vhdVector3 vecMinLeft, vecMinRight, vecMaxLeft, vecMaxRight;

	
	if ( (_pNode[nCurrentPos].node.data[BITFLAG] &  MASK_LX) == MASK_LX)
	{
		 //mean that the left children take the parent value
		vecMinLeft.x = vecMinParent.x;
		vecMinRight.x = ((float)(_pNode[nCurrentPos].node.data[LX])/255.0f) * (vecMaxParent.x-vecMinParent.x) + vecMinParent.x;
	}
	else
	{
		//left is parent
		vecMinRight.x = vecMinParent.x;
		vecMinLeft.x = ((float)(_pNode[nCurrentPos].node.data[LX])/255.0f) * (vecMaxParent.x-vecMinParent.x) + vecMinParent.x;
	}

	if ((_pNode[nCurrentPos].node.data[BITFLAG] &  MASK_LY) == MASK_LY)
	{
		//mean that the left children take the parent value
		vecMinLeft.y = vecMinParent.y;
		vecMinRight.y = ((float)(_pNode[nCurrentPos].node.data[LY])/255.0f) * (vecMaxParent.y-vecMinParent.y) + vecMinParent.y;
	}
	else
	{
		//left is parent
		vecMinRight.y = vecMinParent.y;
		vecMinLeft.y = ((float)(_pNode[nCurrentPos].node.data[LY])/255.0f) * (vecMaxParent.y-vecMinParent.y) + vecMinParent.y;
	}

	if ((_pNode[nCurrentPos].node.data[BITFLAG] &  MASK_LZ) == MASK_LZ)
	{
		 //mean that the left children take the parent value
		vecMinLeft.z = vecMinParent.z;
		vecMinRight.z = ((float)(_pNode[nCurrentPos].node.data[LZ])/255.0f) * (vecMaxParent.z-vecMinParent.z) + vecMinParent.z;
	}
	else
	{
		//left is parent
		vecMinRight.z = vecMinParent.z;
		vecMinLeft.z = ((float)(_pNode[nCurrentPos].node.data[LZ])/255.0f) * (vecMaxParent.z-vecMinParent.z) + vecMinParent.z;
	}

	if ((_pNode[nCurrentPos].node.data[BITFLAG] &  MASK_RX) == MASK_RX)
	{
		//mean that the left children take the parent value
		vecMaxLeft.x = vecMaxParent.x;
		vecMaxRight.x = vecMaxParent.x- ((float)(_pNode[nCurrentPos].node.data[RX])/255.0f) * (vecMaxParent.x-vecMinParent.x);
	}
	else
	{
		//left is parent
		vecMaxRight.x = vecMaxParent.x;
		vecMaxLeft.x = vecMaxParent.x -((float)(_pNode[nCurrentPos].node.data[RX])/255.0f) * (vecMaxParent.x-vecMinParent.x);
	}

	if ((_pNode[nCurrentPos].node.data[BITFLAG] &  MASK_RY)  == MASK_RY)
	{
		//mean that the left children take the parent value
		vecMaxLeft.y = vecMaxParent.y;
		vecMaxRight.y =  vecMaxParent.y-((float)(_pNode[nCurrentPos].node.data[RY])/255.0f) * (vecMaxParent.y-vecMinParent.y);
	}
	else
	{
		//left is parent
		vecMaxRight.y = vecMaxParent.y;
		vecMaxLeft.y =  vecMaxParent.y - ((float)(_pNode[nCurrentPos].node.data[RY])/255.0f) * (vecMaxParent.y-vecMinParent.y);
	}

	if ((_pNode[nCurrentPos].node.data[BITFLAG] &  MASK_RZ) == MASK_RZ )
	{
		//mean that the left children take the parent value
		vecMaxLeft.z = vecMaxParent.z;
		vecMaxRight.z = vecMaxParent.z - ((float)(_pNode[nCurrentPos].node.data[RZ])/255.0f) * (vecMaxParent.z-vecMinParent.z);
	}
	else
	{
		//left is parent
		vecMaxRight.z = vecMaxParent.z;
		vecMaxLeft.z = vecMaxParent.z - ((float)(_pNode[nCurrentPos].node.data[RZ])/255.0f) * (vecMaxParent.z-vecMinParent.z) ;
	}

	//at this time we have both vhdAxisAlignedBox
	if (vecMinLeft.x>vecMaxLeft.x)
	{
		std::swap(vecMinLeft.x,vecMaxLeft.x);
	}
	if (vecMinLeft.y>vecMaxLeft.y)
	{
		std::swap(vecMinLeft.y,vecMaxLeft.y);
	}
	if (vecMinLeft.z>vecMaxLeft.z)
	{
		std::swap(vecMinLeft.z,vecMaxLeft.z);
	}
	if (vecMinRight.x>vecMaxRight.x)
	{
		std::swap(vecMinRight.x,vecMaxRight.x);
	}
	if (vecMinRight.y>vecMaxRight.y)
	{
		std::swap(vecMinRight.y,vecMaxRight.y);
	}
	if (vecMinRight.z>vecMaxRight.z)
	{
		std::swap(vecMinRight.z,vecMaxRight.z);
	}
	boxLeft.setExtents(vecMinLeft.x, vecMinLeft.y, vecMinLeft.z, vecMaxLeft.x, vecMaxLeft.y, vecMaxLeft.z);
	boxRight.setExtents(vecMinRight.x, vecMinRight.y, vecMinRight.z, vecMaxRight.x, vecMaxRight.y, vecMaxRight.z);
		
}



void
vhdABTTree::subTreeBuild(uint32 nCurrentCacheLineIndex, const vhdAxisAlignedBox &box)
{
	std::cout << "Current Cache Line Index    : " << nCurrentCacheLineIndex << std::endl;
	std::cout << "First Free Cache Line Index : " << _nFirstFreeCacheLineIndex << std::endl;
	std::cout << std::endl;
	
	//sub-tree must be like this, their are reference in their local index in the current cache lines
	//					1|2
	//			3|4				5|6
	//		7|8      9|10    11|12   13|14
	
	// 1 solution
	//
	// List = [1]
	//ask for split
	//for each el in list, pop_front() , [ push_children]
	//
	// ex:
	// 1|2                        --> 0
	// 3|4  5|6					  --> 1 2
	// 5|6 7|8 9|10				---> 2 3 4
	// 7|8 9|10 11|12 13|14      --> 3 4 5 6
	// 9|10 11|12 13|14          --> 4 5 6 
	// 11|12 13|14				 --> 5 6
	// 13|14					--> 6


	ListElement listElement;
	listElement.push_back(Element(0,box));

	while (listElement.size()>0)
	{
		//check if we are not at the last line of the sub-tree
		if ( listElement.begin()->id >= nSubTreeId)
		{
			//we reach the end line
			break;
		}
		uint32 nCurrentPos = nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id; 

		if ((_pNode[nCurrentPos].node.data[BITLEAF] & MASK_ISLEAF) == MASK_ISLEAF)
		{
			//this is a leaf,
			//skip it (ony if local root is leaf, could be done once only)
			listElement.pop_front();
			continue;
		}
		
		vhdAxisAlignedBox boxLeft,boxRight;
		_computeChildBox(listElement.begin()->bbox,boxLeft,boxRight, nCurrentPos);
		
		
		//check if left  need to be split
		if (_needToSplit(boxLeft,nCurrentPos,true))
		{
			_makeChildren(listElement,boxLeft,nCurrentPos,nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id*2 + 1, listElement.begin()->id*2 + 1,true);	
		}
		else
		{
			_makeLeaf(nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id,nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id*2 + 1,true);
		}
		//right
		if (_needToSplit(boxRight,nCurrentPos,false))
		{
			_makeChildren(listElement,boxRight,nCurrentPos,nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id*2 + 2,listElement.begin()->id*2 + 2,false);	
		}
		else
		{
			_makeLeaf(nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id,nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id*2 + 2,false);
		}
		listElement.pop_front();	
	}

	if (listElement.size()>0)

	{
		//at this stage, we need to handle the final line of this sub-tree
		bool bFirst(true);
		uint32 i;
		ListElement::iterator it(listElement.begin());
		for ( i = nSubTreeId ; i < vhdABTTree::nCacheLineNodes-1; ++i)
		{
			if ( it->id != i)
			{
				//this potential node is not existing, so skipt it
				continue;
			}
			uint32 nCurrentPos = nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + it->id; 
			if ( (_pNode[nCurrentPos].node.data[BITLEAF] & MASK_ISLEAF) == MASK_ISLEAF)
			{
				//this is a leaf, so we don't care	
				++it;
				continue;
			}
			_nFirstFreeCacheLineIndex+=2; //for left and right
			#ifdef TREE_DEBUG
			std::cout << "_nFirstFreeCacheLineIndex internal "  << _nFirstFreeCacheLineIndex << std::endl;
			#endif
			if (bFirst)
			{
				//setup the next children into the line
				bFirst = false;
				_pNode[(nCurrentCacheLineIndex+1) * vhdABTTree::nCacheLineNodes -1 ].leaf.index = _nFirstFreeCacheLineIndex-1; //point to the left
			}
			//set this node as a non-leaf for continuity, use during traversal
			uint32 mask = 0x1 << (i-nSubTreeId);
			_pNode[(nCurrentCacheLineIndex+1) * vhdABTTree::nCacheLineNodes -1 ].leaf.callback = _pNode[(nCurrentCacheLineIndex+1) * vhdABTTree::nCacheLineNodes -1 ].leaf.callback | mask;
			
			++it;
		}

		uint32 nChildIndex=_pNode[(nCurrentCacheLineIndex+1) * vhdABTTree::nCacheLineNodes -1 ].leaf.index;
		for ( it = listElement.begin(); it != listElement.end(); ++it)
		{
			//recursive call
			vhdAxisAlignedBox boxLeft,boxRight;
			_computeChildBox(listElement.begin()->bbox,boxLeft,boxRight, nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id);
			//left
			//first el is not a leaf
			_pNode[nChildIndex*vhdABTTree::nCacheLineNodes].node.data[BITLEAF] = MASK_NULL;
			//we need to provide the next right and left here, so lets do it!
			vhdAxisAlignedBox boxLeftL,boxRightL;
			
			vhdSceneConverterABTTree::getSingleton().getChildren(vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + it->id].pLeft ,nChildIndex*vhdABTTree::nCacheLineNodes, boxLeftL, boxRightL);
			_makeNodeDataFlags(nChildIndex*vhdABTTree::nCacheLineNodes,it->bbox,boxLeftL, boxRightL);
			subTreeBuild(nChildIndex++,boxLeft);
			
			vhdSceneConverterABTTree::getSingleton().getChildren(vhdSceneConverterABTTree::getSingleton().mapNodeChild[nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + it->id].pRight ,nChildIndex*vhdABTTree::nCacheLineNodes, boxLeftL, boxRightL);
			_makeNodeDataFlags(nChildIndex*vhdABTTree::nCacheLineNodes,it->bbox,boxLeftL, boxRightL);
			subTreeBuild(nChildIndex++,boxRight);
			
		}
	}
}




bool 
vhdABTTree::_isVisible(const vhdAxisAlignedBox & box)
{
	//default callback -> all node are visile
	return true;
}


void
vhdABTTree::traversal()
{
	//we start from the root -> so _box
	_traversal(0,_bbox);
}

void
vhdABTTree::_traversal(uint32 nCurrentCacheLineIndex,const vhdAxisAlignedBox & box)
{
	
	#ifdef TREE_DEBUG
		std::cout << "_traversal " << nCurrentCacheLineIndex << std::endl;
	#endif
	//1) we need to try to traverse a complete sub-tree
	ListElement listElement;
	listElement.push_back(Element(0,box));

	while (listElement.size()>0)
	{
		//check if we are not at the last line of the sub-tree
		if ( listElement.begin()->id >= nSubTreeId)
		{
			//we reach the end line
			break;
		}

		uint32 nCurrentPos = nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + listElement.begin()->id; 

		if ( (_pNode[nCurrentPos].node.data[BITLEAF] & MASK_ISLEAF)  == MASK_ISLEAF)
		{
			//this is a leaf,
			//TO DO NOTIFY THAT WE NEED TO RENDER THIS LEAF
			uint32 index = _pNode[nCurrentPos].leaf.index >> MASK_ISLEAF;
			//we notify that this leaf is visible.
			#ifdef TREE_DEBUG
				std::cout << "_pNode[nCurrentPos] need to be render " << nCurrentPos  << " and leaf is " << index << std::endl;
			#endif
			funcVisit(nCurrentPos, index,listElement.begin()->bbox);
			listElement.pop_front();
			continue;
		}
		
		vhdAxisAlignedBox boxLeft,boxRight;
		_computeChildBox(listElement.begin()->bbox,boxLeft,boxRight, nCurrentPos);
		
		
		//check if left  need to be split
		if (funcIsVisible(boxLeft))
		{
			//add the new leaf  with the number 
			listElement.push_back(Element(listElement.begin()->id*2 + 1,boxLeft));
		}
		//right
		if (funcIsVisible(boxRight))
		{
			//add the new leaf  with the number 
			listElement.push_back(Element(listElement.begin()->id*2 + 2,boxRight));
		}
		listElement.pop_front();	
	}
	
	//now we reach the end line of this sub-tree
	
	//take the index to the first children
	uint32 nNextChildren = _pNode[(nCurrentCacheLineIndex+1) * vhdABTTree::nCacheLineNodes - 1].leaf.index;

	uint32 i;
	ListElement::iterator it(listElement.begin());
	for ( i = nSubTreeId ; i < vhdABTTree::nCacheLineNodes; ++i)
	{
		if (it == listElement.end())
		{
			break;
		}
		uint32 nCurrentPos = nCurrentCacheLineIndex * vhdABTTree::nCacheLineNodes + it->id; //nNextChildren * vhdABTTree::nCacheLineNodes + it->id; 
		if (nNextChildren==0)
		{
			//mean a leaf here
			//this is a leaf, so we render it
			//TO DO NOTIFY THAT WE NEED TO RENDER THIS LEAF
			uint32 index = _pNode[nCurrentPos].leaf.index >> MASK_ISLEAF;
			#ifdef TREE_DEBUG
			std::cout << "_pNode[nCurrentPos] need to be render " << nCurrentPos <<  " and leaf is " << index << std::endl;
			#endif
			funcVisit(nCurrentPos, index,it->bbox);
			//listElement.pop_front();
			++it;
			continue;
		}
		
		
	

		if ( it->id != i)
		{
			//this potential node is not existing, so skipt it
			continue;
		}
		if ( (_pNode[nCurrentPos].node.data[BITLEAF] & MASK_ISLEAF) == MASK_ISLEAF)
		{
			//this is a leaf, so we render it
			uint32 index = _pNode[nCurrentPos].leaf.index >> MASK_ISLEAF;
			#ifdef TREE_DEBUG
			std::cout << "_pNode[nCurrentPos] need to be render " << nCurrentPos  << " and leaf is " << index << std::endl;
			#endif
			funcVisit(nCurrentPos, index,it->bbox);
			//listElement.pop_front();
			++it;
			continue;
		}
		//check the flags 
		uint32 mask = 0x1 << (i-nSubTreeId);
		if ((_pNode[(nCurrentCacheLineIndex+1) * vhdABTTree::nCacheLineNodes -1 ].leaf.callback  & mask) == mask)
		{
			//this node would have a two child
			nNextChildren+=2; //for left and right
		}
		
		//we need to check for the children from here,so as we have already increment the index
		//recursive call.
		vhdAxisAlignedBox boxLeft,boxRight;
		_computeChildBox(it->bbox,boxLeft,boxRight, nCurrentPos);
		//left
		//check if left  need to be split
		if (funcIsVisible(boxLeft))
		{
			_traversal(nNextChildren-2, boxLeft); 
		}
		//right
		if (funcIsVisible(boxRight))
		{
			_traversal(nNextChildren-1,boxRight);
		}


		++it;
	}

}













