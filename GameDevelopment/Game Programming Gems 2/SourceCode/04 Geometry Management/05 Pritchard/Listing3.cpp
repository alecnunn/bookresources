/* Copyright (C) Matt Pritchard, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Matt Pritchard, 2001"
 */
QuadNode* QuadTree::GetNodelContaining(const rect &ObjBounds)
{
     
    int   x1 = (int) (ObjBounds.left * QuadXScale);
    int   y1 = (int) (ObjBounds.top  * QuadYScale);

    int   xResult = x1 ^ ((int) (ObjBounds.right * QuadXScale));
    int   yResult = y1 ^ ((int) (ObjBounds.bottom * QuadYScale));

    int   NodeLevel = NumberOfTreeLevels;
    int   shiftCount = 0;
  
    while (xResult + yResult != 0 )    //Count highest bit position
    {
        xResult >>= 1;
        yResult >>= 1;
        NodeLevel--;
        ShiftCount++;
    }

    // Now lookup the node pointer in a 2D array stored linearly

    x1 >>= shiftCount;			// Scale coordinates for
    y1 >>= shiftCount;              // quadtree level

    QuadNode** nodes = NodeLevelPointerArray[NodeLevel];

    return (nodes[y1<<(NodeLevel-1)+x1]);
}
