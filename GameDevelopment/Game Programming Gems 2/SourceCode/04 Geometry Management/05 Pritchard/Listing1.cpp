/* Copyright (C) Matt Pritchard, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Matt Pritchard, 2001"
 */
// Two C++ classes, one to represent the QuadTree and one to
// Represent each individual node.
// all variables are class members unless defined 

QuadNode* QuadTree::GetNodeContaining(const rect &ObjBounds)
{
    if (RootNode->Contains(ObjBounds)
	 return(RootNode->GetNodeContaining(ObjBounds);
    else
       return(NULL);
}

QuadNode* QuadNode::GetNodeContaining(const rect &ObjBounds)
{
   if (!isLeafNode)
   {
   	if (ULNode->Contains(ObjBounds)
	    return(ULNode->GetNodeContaining(ObjBound));

   	if (URNode->Contains(ObjBounds)
	    return(URNode->GetNodeContaining(ObjBound));

   	if (LLNode->Contains(ObjBounds)
	    return(LLNode->GetNodeContaining(ObjBound));

   	if (LRNode->Contains(ObjBounds)
	    return(LRNode->GetNodeContaining(ObjBound));
   }
   return(this);

}

bool QuadNode::Contains(const rect &ObjBounds)
{
   return(ObjBounds.top >= y1 && ObjBounds.left >= x1 &&
          ObjBounds.bottom <= y2 && ObjBounds.right <= x2);
}


