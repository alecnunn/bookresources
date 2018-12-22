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


#ifndef VHDSCENECONVERTERABTTREE_H
#define VHDSCENECONVERTERABTTREE_H

class vhdABTTree;

#include <vector>
#include <map>
#include <string>
#include <list>
#include "vhdSingleton.h"
#include "vhdAxisAlignedBox.h"

/**
 * This class is provide functionalities for keeping a more classical tree view (e.g using pointers), helping for the creation of the cache-oblivious one
 *
 *
 */
class vhdSceneConverterABTTree  : public vhdSingleton<vhdSceneConverterABTTree>
{
public:
	/**
	 * Each child node struct keep is local AABB among a path/id to the data.
	 */
	class NodeChildStruct
	{
	public:
		NodeChildStruct(const vhdAxisAlignedBox & _bbox, const std::string & _strPath):bbox(_bbox),strPath(_strPath){}
		vhdAxisAlignedBox bbox;
		std::string	strPath;
	};

	typedef std::list<NodeChildStruct>  ListAABB;
	
	/*
	 * Binary Tree use for the construction of the cache-oblivious ABTTree as they are more pratical for finding the data.
	 * This class is use only during the compilation of the cache-oblivious ABTTree
	 */
	class vhdSceneABTTreeNode
	{
	public:
		vhdSceneABTTreeNode()
		{
			pLeft=pRight=NULL;
		}
		ListAABB listAABB; 
		vhdSceneABTTreeNode* pLeft;
		vhdSceneABTTreeNode* pRight;
	};

	typedef std::map<unsigned int, vhdSceneABTTreeNode> MapNodeChild;
	
	MapNodeChild mapNodeChild;


public:
	/**
	 * find the global AABB from a ListAABB
	 * 
	 * @param listAABB the list where we want to retrieve the global AABB
	 * @return the global AABB
	 */
	static vhdAxisAlignedBox findGlobalAABB(const ListAABB & listAABB);

	

public:
	/** 
	 * constructor
	 */
	vhdSceneConverterABTTree();
	/** 
	 * destructor
	 */
	~vhdSceneConverterABTTree();


private:
	/**
	 * Define the different Axis 
	 */
	static const unsigned int ABTTREE_AXISX;
	static const unsigned int ABTTREE_AXISY;
	static const unsigned int ABTTREE_AXISZ;
	static const unsigned int ABTTREE_AXISINVALID;

	

	/**
	 * find best splitLocation
	 * Real project will replace this template by a more robust and reliable routine
	 *
	 * @param pParentNode the parentNode
	 * @param axis the axis (x,y,z) where we want to find a splitter
	 * @param fSplitLoc the coordinate where we will split (return value)
	 * @param uiNodeIndex the node index
	 * @return the weight for the best splitter among the selected axis
	 */
	float _findBestSplitLocation (vhdSceneABTTreeNode *pParentNode ,int axis, float& fSplitLoc,unsigned int uiNodeIndex);

public:

	// ABTree Data Structures
	vhdSceneABTTreeNode *pDataRootNode;
	

	/** 
	 * get the children for the CurrentChildPosition
	 *
	 * @param pParentNode the parentNode
	 * @param nCurrentChildPosition the current child position
	 * @param boxLeft   AABB for the left  child (which will be retrieve from this routine)
	 * @param boxRightt AABB for the right child (which will be retrieve from this routine)
	 */
	void getChildren(vhdSceneABTTreeNode *pParentNode ,unsigned int nCurrentChildPosition,vhdAxisAlignedBox & boxLeft, vhdAxisAlignedBox & boxRight);

};

#endif
