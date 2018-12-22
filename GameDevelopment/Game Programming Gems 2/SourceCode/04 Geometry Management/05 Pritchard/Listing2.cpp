/* Copyright (C) Matt Pritchard, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Matt Pritchard, 2001"
 */
int QuadTree::GetNodeLevelContaining(const rect &ObjBounds)
{
    int   xResult = ((int) (ObjBounds.left * QuadXScale)) ^ 
                    ((int) (ObjBounds.right * QuadXScale));
 
    int   yResult = ((int) (ObjBounds.top * QuadYScale)) ^ 
                    ((int) (ObjBounds.bottom * QuadYScale));

    int   NodeLevel = NumberOfTreeLevels;
  
    while (xResult + yResult != 0 )    //Count highest bit position
    {
        xResult >>= 1;
        yResult >>= 1;
        NodeLevel--;
    }

    return (NodeLevel);
}
