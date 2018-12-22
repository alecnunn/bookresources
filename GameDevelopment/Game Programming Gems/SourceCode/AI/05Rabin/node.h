/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
typedef int NodeLocation;

class Node
{
public:
   NodeLocation location;   // location of node (some location representation)
   Node* parent;            // parent node (zero pointer represents starting node)
   float cost;              // cost to get to this node
   float total;             // total cost (cost + heuristic estimate)
   bool onOpen;             // on Open list
   bool onClosed;           // on Closed list
};

