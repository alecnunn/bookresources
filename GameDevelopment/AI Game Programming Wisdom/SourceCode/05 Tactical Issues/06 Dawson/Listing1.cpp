/* Copyright (C) Chad Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chad Dawson, 2001"
 */

// The offsets in the formation positions can be relative 
// to the center of the formation or the first (lead) position 
// in the formation.

struct FormationPosition
{
	float	mXoffset;   // horizontal offset
	float	mYoffset;   // vertical offset
	float	mDirection; // the facing angle
}

struct Formation 
{
	vector<FormationPosition> mPositions;
}
