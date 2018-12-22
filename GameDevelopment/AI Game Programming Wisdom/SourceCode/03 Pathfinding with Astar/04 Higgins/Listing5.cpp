/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

// An example of some of the FastStorage methods that were overloaded are:

inline void AddNodeToOpenList(AStarNode* inNode)
{ // call base class version.
	AStarStorage::AddNodeToOpenList(inNode);
	// Add it to our own node sliding window array.
	this->AddNodeToNodeTable(inNode->mX, inNode->mY, inNode);
}

inline AStarNode* FindNodeInClosedList(long inX, long inY)
{ // the flags array will tell us if we should return it
	if(this->GetIsClosed(this->GetFlagForTile(inX,inY))
		return this->GetNodeFromNodeTable(inX, inY);
	else
		return NULL;
}
