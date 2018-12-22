/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

// A Star Node
struct AStarNode
{
	inline AStarNode(long inX = 0, long inY = 0)
	: mX(inX),mY(inY),mTotalCost(0), mCostOfNode(0),
	  mCostToDestination(0),mParent(NULL),mNext(NULL),
	  mListParent(NULL) { }

	AStarNode* mParent;      // who opened me?
	AStarNode* mNext;        // Next in my list
	AStarNode* mListParent;  // my list Parent
	long mTotalCost;         // f
	long mCostOfNode;        // g
	long mCostToDestination; // h (estimate)
	long mX;                 // Tile X
	long mY;                 // Tile Y
};
