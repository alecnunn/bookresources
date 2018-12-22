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


#include "vhdSceneConverterABTTree.h"
#include "vhdABTTree.h"
#include <float.h>
#include <iostream>

//for singleton
template <typename vhdSceneConverterABTTree> vhdSceneConverterABTTree* vhdSingleton <vhdSceneConverterABTTree>::ms_Singleton = 0;

const unsigned int vhdSceneConverterABTTree::ABTTREE_AXISX = 0;
const unsigned int vhdSceneConverterABTTree::ABTTREE_AXISY = 1;
const unsigned int vhdSceneConverterABTTree::ABTTREE_AXISZ = 2;
const unsigned int vhdSceneConverterABTTree::ABTTREE_AXISINVALID = 3;



vhdSceneConverterABTTree::vhdSceneConverterABTTree()
{

	pDataRootNode = NULL;
}


vhdSceneConverterABTTree::~vhdSceneConverterABTTree()
{
}




vhdAxisAlignedBox 
vhdSceneConverterABTTree::findGlobalAABB(const ListAABB & listAABB)
{
	
	ListAABB::const_iterator it;
	vhdVector3 vecMin, vecMax, vecGlobalMin(FLT_MAX,FLT_MAX,FLT_MAX), vecGlobalMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	for (it=listAABB.begin(); it!=listAABB.end(); ++it)
	{
		vecMin = it->bbox.getMinimum();
		vecMax = it->bbox.getMaximum();
		if (vecMin.x<vecGlobalMin.x) vecGlobalMin.x=vecMin.x;
		if (vecMin.y<vecGlobalMin.y) vecGlobalMin.y=vecMin.y;
		if (vecMin.z<vecGlobalMin.z) vecGlobalMin.z=vecMin.z;
		if (vecMax.x>vecGlobalMax.x) vecGlobalMax.x=vecMax.x;
		if (vecMax.y>vecGlobalMax.y) vecGlobalMax.y=vecMax.y;
		if (vecMax.z>vecGlobalMax.z) vecGlobalMax.z=vecMax.z;
	}
	return vhdAxisAlignedBox(vecGlobalMin.x,vecGlobalMin.y,vecGlobalMin.z, vecGlobalMax.x,vecGlobalMax.y,vecGlobalMax.z);
}


#define TRIAL 20

float 
vhdSceneConverterABTTree::_findBestSplitLocation (vhdSceneABTTreeNode *pParentNode ,int axis, float& fSplitLoc,unsigned int uiNodeIndex)
{
	//get the AABB
	vhdAxisAlignedBox aabb = findGlobalAABB(pParentNode->listAABB);
	unsigned int uiListSize = (unsigned int) pParentNode->listAABB.size();

	int i;
	float a;
	float fBestQual = -2.0;
	float fInvNumObjects = 1.0 / float (uiListSize);
	for (i = 0 ; i < TRIAL ; i++)
	{
		// Calculate a possible split location.
		a = aabb.getMinimum()[axis] + float (i+1)*(aabb.getMaximum()[axis]-aabb.getMinimum()[axis])/float (TRIAL+1.0);
		
		
		// Now count the number of poly that are on the left and the number of objects completely on the right
		// Normally the split function is much complex taking care of object complexity as well (rendering cost,....)
		
		unsigned int uiLeft = 0;
		unsigned int uiRight = 0;
		
		//we go through each instance to find the proper index, so here we go
		ListAABB::iterator it;
		vhdVector3 vecMin, vecMax;
		for (it=pParentNode->listAABB.begin(); it!=pParentNode->listAABB.end(); ++it)
		{
			vecMin = it->bbox.getMinimum();
			vecMax = it->bbox.getMaximum();

			if ((vecMin[axis] + (vecMax[axis]-vecMin[axis]) / 2.0f) < a)
			{
				++uiLeft;	
			}
			else
			{
				++uiRight;
			}
		}
		
		// If we have no object on the left or right then this is a bad
		// split which we should never take.
		float fQual;
		if (uiLeft == 0 || uiRight == 0)
		{
			fQual = -1.0;
		}
		else
		{
			fQual = 1.0 - (float (abs ((int)(uiLeft-uiRight))) * fInvNumObjects);
		}
		if (fQual > fBestQual)
		{
			fBestQual = fQual;
			fSplitLoc = a;
		}
	}
	
	return fBestQual;
}




void 
vhdSceneConverterABTTree::getChildren(vhdSceneABTTreeNode *pParentNode ,unsigned int nCurrentChildPosition,vhdAxisAlignedBox & boxLeft, vhdAxisAlignedBox & boxRight)
{
	// Here we have 2 or more objects.
	// We use FindBestSplitLocation() to see how we can best split this
	// node.
	float split_loc_x, split_loc_y, split_loc_z;
	float qual_x = _findBestSplitLocation (pParentNode,vhdSceneConverterABTTree::ABTTREE_AXISX, split_loc_x,nCurrentChildPosition);
	float qual_y = _findBestSplitLocation (pParentNode,vhdSceneConverterABTTree::ABTTREE_AXISY, split_loc_y,nCurrentChildPosition);
	float qual_z = _findBestSplitLocation (pParentNode,vhdSceneConverterABTTree::ABTTREE_AXISZ, split_loc_z,nCurrentChildPosition);

	float fSplitLocation;
	int iSplitAxis;
	if (qual_x >= 0 && qual_x >= qual_y && qual_x >= qual_z)
	{
		iSplitAxis = vhdSceneConverterABTTree::ABTTREE_AXISX;
		fSplitLocation = split_loc_x;
	}
	else if (qual_y >= 0 && qual_y >= qual_x && qual_y >= qual_z)
	{
		iSplitAxis = vhdSceneConverterABTTree::ABTTREE_AXISY;
		fSplitLocation = split_loc_y;
	}
	else ///if (qual_z >= 0)
	{
		iSplitAxis = vhdSceneConverterABTTree::ABTTREE_AXISZ;
		fSplitLocation = split_loc_z;
	}

	//we have our splitter so we do the real collection then
	
	//we go through each instance to find the proper index, so here we go
	ListAABB::iterator it;
	vhdVector3 vecMin, vecMax;
	
	
	pParentNode->pLeft = new vhdSceneABTTreeNode();
	pParentNode->pRight = new vhdSceneABTTreeNode();

	for (it=pParentNode->listAABB.begin(); it!=pParentNode->listAABB.end(); ++it)
	{
		vecMin = it->bbox.getMinimum();
		vecMax = it->bbox.getMaximum();

		if ((vecMin[iSplitAxis] + (vecMax[iSplitAxis]-vecMin[iSplitAxis]) / 2.0f) < fSplitLocation)
		{
			pParentNode->pLeft->listAABB.push_back(*it);
			#ifdef TREE_DEBUG
			std::cout << "name left " << it->strPath << std::endl;
			#endif
		}	
		else
		{
			pParentNode->pRight->listAABB.push_back(*it);
			#ifdef TREE_DEBUG
			std::cout << "name right " << it->strPath << std::endl;
			#endif
		}
	}

	//avoid that sub-child become empty
	while (pParentNode->pLeft->listAABB.size()<2)
	{
		//we transfer one
		pParentNode->pLeft->listAABB.push_back(*pParentNode->pRight->listAABB.begin());
		pParentNode->pRight->listAABB.pop_front();
	}
	while  (pParentNode->pRight->listAABB.size()<2)
	{
		//we transfer one
		pParentNode->pRight->listAABB.push_back(*pParentNode->pLeft->listAABB.begin());
		pParentNode->pLeft->listAABB.pop_front();
	}
	
	//we insert the current status in our map
	vhdSceneConverterABTTree::getSingleton().mapNodeChild.insert(vhdSceneConverterABTTree::MapNodeChild::value_type(nCurrentChildPosition,*pParentNode));
	
	//compute bbox
	boxLeft = findGlobalAABB(pParentNode->pLeft->listAABB);
	boxRight = findGlobalAABB(pParentNode->pRight->listAABB);
}





