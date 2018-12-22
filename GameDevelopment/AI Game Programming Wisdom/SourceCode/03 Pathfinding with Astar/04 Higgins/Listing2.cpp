/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

typedef unsigned char AStarNodeStatusFlags;
enum
{
	kClear    = 0x00, //empty, unexamined.
	KPassable = 0x01, //examined, node is not blocked
	KBlocked  = 0x02, //examined, node is blocked
	KOpen     = 0x04, //node is on the open list
	KClosed   = 0x08  //node is on the closed list
};
