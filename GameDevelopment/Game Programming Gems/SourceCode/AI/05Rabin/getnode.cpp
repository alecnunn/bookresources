/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
Node* GetNode( MasterNodeList nodelist, NodeLocation node_location )
{
   //GetNodeFromMasterNodeList accesses the hash table of nodes
   Node* node = GetNodeFromMasterNodeList( nodelist, node_location );
   if( node ) {
      return( node );
   }
   else
   {   //Not in the Master Node List - get a new one from the Node Bank
      Node* newNode = GetFreeNodeFromNodeBank();
      newNode->location = node_location;
      newNode->onOpen = false;
      newNode->onClosed = false;

      //StoreNodeInMasterNodeList places the node into the hash table
      StoreNodeInMasterNodeList( nodelist, newNode );
      return( newNode ); 
   }
}
