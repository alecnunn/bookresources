/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
class NodeTotalGreater 
{
public:
   //This is required for STL to sort the priority queue
   //(its entered as an argument in the STL heap functions)
   bool operator()( Node * first, Node * second ) const {
      return( first->total > second->total );
   }
};
