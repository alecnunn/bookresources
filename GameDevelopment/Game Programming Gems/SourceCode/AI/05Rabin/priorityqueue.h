/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
class PriorityQueue
{
public:
   //Heap implementation using an STL vector
   //Note: the vector is an STL container, but the
   //operations done on the container cause it to
   //be a priority queue organized as a heap
   std::vector<Node*> heap;
};
