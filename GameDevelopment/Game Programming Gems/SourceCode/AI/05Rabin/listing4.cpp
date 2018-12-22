/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
#include <queue>
#include <vector>
#include "node.h"
#include "priorityqueue.h"
#include "nodetotalgreater.h"

Node* PopPriorityQueue( PriorityQueue& pqueue )
{  //Total time = O(log n)
   
   //Get the node at the front - it has the lowest total cost
   Node * node = pqueue.heap.front();

   //pop_heap will move the node at the front to the position N
   //and then sort the heap to make positions 1 through N-1 correct
   //(STL makes no assumptions about your data and doesn't want to change
   //the size of the container.)
   std::pop_heap( pqueue.heap.begin(), pqueue.heap.end(), NodeTotalGreater() );

   //pop_back() will actually remove the last element from the heap
   //(now the heap is sorted for positions 1 through N)
   pqueue.heap.pop_back();

   return( node );
}

void PushPriorityQueue( PriorityQueue& pqueue, Node* node )
{  //Total time = O(log n)
  
   //Pushes the node onto the back of the vector (the heap is now unsorted)
   pqueue.heap.push_back( node );

   //Sorts the new element into the heap
   std::push_heap( pqueue.heap.begin(), pqueue.heap.end(), NodeTotalGreater() );
}

void UpdateNodeOnPriorityQueue( PriorityQueue& pqueue, Node* node )
{  //Total time = O(n+log n)
   
   //Loop through the heap and find the node to be updated
   std::vector<Node*>::iterator i;
   for( i=pqueue.heap.begin(); i!=pqueue.heap.end(); i++ )
   {
      if( (*i)->location == node->location )
      {  //Found node - resort from this position in the heap
         //(since its total value was changed before this function was called)
         std::push_heap( pqueue.heap.begin(), i+1, NodeTotalGreater() );
         return;
      }
   }
}

bool IsPriorityQueueEmpty( PriorityQueue& pqueue )
{
   //empty() is an STL function that determines if
   //the STL vector has no elements 
   return( pqueue.heap.empty() );
}
