/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

AStarStorage::AddNodeToClosedList(AStarNode* inNode)
{
	// Generate a hash code.
	long theHash = this->Hash(inNode->mX, inNode->mY);

	// Drop it into that list, and setup list pointers.
	if(this->mClosedLists[theHash] != NULL)
		this->mClosedLists[theHash]->mListParent = inNode;

	// the current bucket head is now our next node.
	inNode->mNext = this->mClosedLists[theHash];

	// This is now the head of this bucket's list.
	this->mClosedLists[theHash] = inNode;

	// set the closed node flag
	this->SetClosedFlag(this->GetFlag(inNode->mX, inNode->mY));
}
