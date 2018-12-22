/* Copyright (C) Chad Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chad Dawson, 2001"
 */

// Extending our Formation structure, we can set up an offset 
// for the next rank. When all of the original FormationPosition 
// structures have been filled, a new rank can be created at the 
// specified offset from the current one.

struct Formation
{
	float mRankOffsetX;
	float mRankOffsetY;
	vector<FormationPosition> mPositions;
}
