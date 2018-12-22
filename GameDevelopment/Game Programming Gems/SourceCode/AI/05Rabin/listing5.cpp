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

MasterNodeList g_nodelist;

bool FindPath( GameObject* gameobject, WorldLocation goal )
{
   //Get a path in progress if it exists for this game object with this goal
   //A path may have been started and not finished from last game tick
   //If no path in progress, it returns an empty path structure
   Path* path = GetPathInProgress( gameobject, goal );

   if( !path->initialized )
   {  //The InitializePath function fills out the path structure for this path request
      //It initializes a clean MasterNodeList and a clean Open list
      InitializePath( path, gameobject, goal );
      
      //Create the very first node and put it on the Open list
      Node* startnode = GetNode( g_nodelist, GetNodeLocation( gameobject->pos ) );
      startnode->onOpen = true;      //This node is going on the Open list
      startnode->onClosed = false;   //This node is not on the Closed list
      startnode->parent = 0;         //This node has no parent
      startnode->cost = 0;           //This node has no cost to get to
      startnode->total = GetNodeHeuristic( startnode->location, path.goal );
      PushPriorityQueue( path.open, startnode );
   }

   while( !IsPriorityQueueEmpty( path->open ) )
   {
       //Get the best candidate node to search next
       Node* bestnode = PopPriorityQueue( path.open );

       if( AtGoal( bestnode, goal ) )
       {  //Found the goal node - construct a path and exit
          //The complete path will be stored inside the game object
          ConstructPathToGoal( gameobject, path );
          return( true );   //return with success
       }

       while( /*loop through all connecting nodes of bestnode*/ )
       {
          Node newnode;
          newnode.location = /*whatever the new location is*/;

          //This avoids searching the node we just came from
          if( bestnode->parent == 0 || 
              bestnode->parent->location != newnode.location )
          {   
             newnode.parent = bestnode;
             newnode.cost = bestnode->cost + CostFromNodeToNode( &newnode, bestnode );
             newnode.total = newnode.cost;

             //Get the preallocated node for this location
             //Both newnode and actualnode represent the same node location,
             //but the search at this point may not want to clobber over the
             //data from a more promising route - thus the duplicate nodes for now
             Node* actualnode = GetNode( g_nodelist, newnode.location );

             //Note: the following test takes O(1) time (no searching through lists)
             if( !( actualnode->onOpen && newnode.total > actualnode->total ) &&
                 !( actualnode->onClosed && newnode.total > actualnode->total ) )
             {  //This node is very promising
                //Take it off the Open and Closed lists (in theory) and push on Open
                actualnode->onClosed = false;   //effectively removing it from Closed
                actualnode->parent = newnode.parent;
                actualnode->cost = newnode.cost;
                actualnode->total = newnode.total;

                if( actualnode->onOpen )
                {  //Since this node is already on the Open list, update it's position
                   UpdateNodeOnPriorityQueue( path.open, actualnode );
                }
                else
                {  //Put the node on the Open list
                   PushPriorityQueue( path.open, actualnode );
                   actualnode->onOpen = true;
                }
             }
          }
       }

       //Now that we've explored bestnode, put it on the Closed list
       bestnode->onClosed = true;

       //Use some method to determine if we've taken too much time
       //this tick and should abort the search until next tick
       if( ShouldAbortSearch() ) {
          return( false );
       }
   }

   //If we got here, all nodes have been searched without finding the goal
   return( false );
}

