/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

// Methods on the A* storage class that use these flags would include:

// Gets the flag from the array
inline AStarNodeStatusFlags* GetFlag(long inX, long inY)	
{ //add your own ASSERT check to ensure no array overruns.
  return this->mFlags + ((inX * this->mArrayMax) + inY); }

// returns true if the node is closed
inline bool GetIsClosed(AStarNodeStatusFlags* inFlag) const
{ return ((*inFlag & kClosed) != kClear); }

// clears the 'passable status' flag
inline void ClearPassableFlag(AStarNodeStatusFlags* inFlag)
{ *inFlag &= ~kPassable; }

// sets the 'open status' flag
inline void SetOpenFlag(AStarNodeStatusFlags* inFlag)
{ *inFlag |= kOpen; }
