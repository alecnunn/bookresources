/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

// Excerpt from FastStorage's ResetPathfinder method: 
// (Note: 1. theBox is a 2D integer rectangle that represents 
// the bounds of what we searched LAST path with this pathfinder. 
// In a sense, what is DIRTY. 2. theBoxes' Left is the lowest X 
// and Top returns is the lowest Y coordinate. 3. theBoxes' Right 
// is the highest X and Bottom is the highest Y.) 

// Get the X offset where we will begin to memset from.
theXOffset = ((theBox.GetLeft() * mMaxX)+ theBox.GetTop());

// Get the amount we will need to clear.
theAmountToClear = (((theBox.GetRight() * mMaxX) +
                      theBox.GetBottom()) - theXOffset);

// memset NODE array using the Xoffset and AmountToClear
::memset(this->mNodes + theXOffset, NULL,
         theAmountToClear * sizeof(AStarNode*));

// Reset the flags array using the same dirty rectangle
::memset(this->mFlags + theXOffset, kClear,
         theAmountToClear * sizeof(AStarNodeStatusFlags));
