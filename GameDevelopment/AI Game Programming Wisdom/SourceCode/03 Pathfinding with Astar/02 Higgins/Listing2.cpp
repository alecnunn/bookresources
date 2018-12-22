/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */
class AStarBase
{
    // normal stuff including the virtual destructor
    virtual AStarGoalBase* GetGoalBase(void) = 0;
    virtual void GetPath(vector<Waypoint>& outPath) = 0;	
    virtual void RunAStar(void)  = 0;
    virtual void SetBailoutCap(long inMaxRevolutions);
	
    // etc.
};

