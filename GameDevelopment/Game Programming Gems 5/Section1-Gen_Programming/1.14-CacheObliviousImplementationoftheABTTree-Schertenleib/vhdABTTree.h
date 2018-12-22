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


#ifndef VHDABTTREE_H
#define VHDABTTREE_H

class vhdSceneEntity;

#include <string>
#include <list>
#include "vhdAxisAlignedBox.h"


// Define data chunk
typedef unsigned int  uint32;
typedef unsigned char byte;


/**
 * Describe an ABTTreeLeafData
 */
class vhdABTTreeLeafData
{
public:
	uint32 index;
	uint32 callback;
};

/**
 * Describe an ABTTreeNodeData
 */
class vhdABTTreeNodeData
{
public:
	byte data[8];
};


/**
 * Made the union between a node and a leaf sharing the same memory size
 */
union vhdABTTreeNode
{
	vhdABTTreeLeafData leaf;
	vhdABTTreeNodeData node;
};

/**
 * an ABTTreeLeaf. In this case, we keep only a pointer to an entity
 */
class vhdABTTreeLeaf
{
public:
	vhdSceneEntity* pEntity;
};


/** @brief A cache-oblivious ABT Tree implementation
 *
 * This class provided in a single instance the creation and execution routine of an effective cache-oblivious implementation of the ABT Tree
 * The user have to choose the proper cache line size among the end-user platform. Most hardware provide 32/64 bytes (see manufaturer docs)
 * modify the ABTTREE_CACHE_LINE_BYTES accordingly
 * 
 *
 * @author Sebastien Schertenleib
 * Sebastien.Schertenleib@epfl.ch 
 * @version 0.0.1
 */
class vhdABTTree
{
	/**
	 * Utility class for building the AABB Tree
	 */
	class Element
	{
	public:
		Element (uint32 _id, vhdAxisAlignedBox _bbox):bbox(_bbox),id(_id){}
		uint32 id;
		vhdAxisAlignedBox bbox;
	};
	typedef std::list<Element> ListElement;

public:
	/**
	 * callback for catching the is visible test
	 */
	typedef bool (vhdABTTreeIsVisibleVisitFunc) (const vhdAxisAlignedBox &);
	/**
	* A callback function for visiting a ABT-tree node. If this function
	* returns true the traversal will continue.
	*
	* @param box the AxisAlignedBox to test!
	* @return true if the current axix aligned box is visible or not
	*/
	vhdABTTreeIsVisibleVisitFunc *funcIsVisible;
	typedef void (vhdABTTreeVisitFunc) (unsigned int uiCurrentPos, unsigned int uiIndex, const vhdAxisAlignedBox &bbox);
	/**
	* A callback function informing that a particular leaf is visible. 
	*/
	vhdABTTreeVisitFunc *funcVisit;
private:
	/**
	 * comput the sub tree depth
	 *
	 * @return the sub tree depth
	 */
	static uint32 _computeSubTreeDepth();


	static uint32 nCacheLineNodes; 	// #nodes per cache line
	static uint32 nCacheLineBytes; 	// bytes per cache line
	static uint32 nDepth; //depth of the sub tree
	static uint32  nSubTreeId; // first id for a member of the last line.
	static const uint32 nArrayDeep; //max theorical multiplier ratio.
	
	uint32 _nFirstFreeCacheLineIndex; //first free cache line index available
	uint32 _nLeafIndex;		// leaf index
	uint32 nCacheLines;		// #cache line sized memory segments used

	vhdAxisAlignedBox _bbox;	//global aabbox

private:
	public:
	vhdABTTreeNode *_pNode; //node array
	vhdABTTreeLeaf *_pLeaf; // leaf aray

public:
	
	/**
	 * constructor 
	 */
	vhdABTTree();

private:
	/** 
	 * default callback
	 *
	 * @param box AABB of this node
	 * @return true if visible
	 */
	static bool _isVisible(const vhdAxisAlignedBox & box);
	/**
	 * default callback
	 *
	 * @param uiCurrentPos current position
	 * @param uiIndex leaf index
	 * @param bbox leaf bbox
	 */
	static void visit(unsigned int uiCurrentPos, unsigned int uiIndex, const vhdAxisAlignedBox &bbox);
		

private:
	/**
	 * make the node flags 
	 *
	 * @param nCurrentChildPosition current child position
	 * @param box parent AABB
	 * @param boxLeft left child AABB
	 * @param boxRight right child AABB
	 */
	void _makeNodeDataFlags(uint32 nCurrentChildPosition, const vhdAxisAlignedBox &box, const vhdAxisAlignedBox &boxLeft, const vhdAxisAlignedBox & boxRight); 
	/**
	 * check if we need to continue the building recursion
	 * 
	 * @param box current parent box
	 * @param nCurrentPos current position
	 * @param bLeft true if we look to the left child, false for the right
	 * @return true if we need to continue the recursion (splitting), false otherwise
	 */
	bool _needToSplit(const vhdAxisAlignedBox &box, uint32 nCurrentPos, bool bLeft);
	/**
	 * cast to 8-bits value
	 *
	 * @param fMinExtend min extend
	 * @param fMinParent min Parent extend
	 * @param fMaxParent max Parent extend
	 * @return the 8-bits cast
	 */
	byte _extendLeft(float fMinExtend, float fMinParent, float fMaxParent);
	/**
	 * cast to 8-bits value
	 *
	 * @param fMaxExtend max extend
	 * @param fMinParent min Parent extend
	 * @param fMaxParent max Parent extend
	 * @return the 8-bits cast
	 */
	byte _extendRight(float fMaxExtend, float fMinParent, float fMaxParent);
	/**
	 * make the children 
	 * 
	 * @param listElement utility list for the current cache line
	 * @param box current node AABB
	 * @param nCurrentPosition current position
	 * @param nCurrentChildPosition current child position
	 * @param index
	 * @param bLeft true for left child, flase for right
	 */
	void _makeChildren(ListElement & listElement, const vhdAxisAlignedBox &box, uint32 nCurrentPosition, uint32 nCurrentChildPosition, uint32 index, bool bLeft);
	/**
	 * make a leaf
	 *
	 * @param nCurrentPosition current position
	 * @param nCurrentChildPosition current child position
	 * @param bLeft true for left child, flase for right
	 */
	void _makeLeaf(uint32 nCurrentPosition, uint32 nCurrentChildPosition, bool bLeft);
	/**
	 * conpute child box
	 *
	 * @param boxParent parent's AABB
	 * @param boxLeft left child's AABB
	 * @param boxRight right child's AABB
	 * @param nCurrentPos current position
	 */
	void _computeChildBox(const vhdAxisAlignedBox & boxParent,vhdAxisAlignedBox &boxLeft,vhdAxisAlignedBox &boxRight, uint32 nCurrentPos);
	/**
	 * build the next sub-tree
	 *
	 * @param nCurrentCacheLineIndex cache line index
	 * @param box sub-root AABB
	 */
	void subTreeBuild(uint32 nCurrentCacheLineIndex, const vhdAxisAlignedBox &box);
	/**
	 * traversal
	 *
	 * @param nCurrentCacheLineIndex current cache line index
	 * @param box sub-root AABB
	 */
	void _traversal(uint32 nCurrentCacheLineIndex,const vhdAxisAlignedBox & box);
public:	
	/**
	 * tree traversal
	 */
	void traversal();
	/**
	 * create
	 * 
	 * @param nObjects nb object in the tree
	 */
	void create(uint32 nObjects);
	/**
	 * create (can be used when size are already known)
	 *
	 * nCacheLineSize cache line size
	 * nLeafSize leaves's number
	 */
	void create(unsigned int nCacheLineSize, unsigned int nLeafSize );
	/**
	 * destroy
	 */
	void destroy();


};

#endif