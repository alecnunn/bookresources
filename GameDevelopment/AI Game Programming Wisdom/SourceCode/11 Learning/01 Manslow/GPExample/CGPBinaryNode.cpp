//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "CGPBinaryNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

//This call is necessary to create the subtrees attached to this node
extern CGPNode *pGPCreateRandomSubtree(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPBinaryNode::CGPBinaryNode()
{
}

CGPBinaryNode::~CGPBinaryNode()
{
	//Destroy the subtrees
	unsigned long i;
	if(pChildren!=NULL)
	{
		for(i=0;i<ulNumberOfChildren;i++)
		{
			if(pChildren[i]!=NULL)
			{
				delete pChildren[i];
			}
		}
		delete []pChildren;
	}
}

CGPNode *CGPBinaryNode::pGetCopy(CGP* pGP)
{
	//Creates a copy of this node. If this node is in the prototype list, it does not have a valid subtree attached to it
	//so a random one is created. If it is not a prototype, the subtree attached to this node is copied by calls to the
	//copy functions of this node's children's children, etc.
	unsigned long i;

	//Create a copy
	CGPBinaryNode *pNewNode=new CGPBinaryNode;
	assert(!(pNewNode==NULL));

	//For each child,
	for(i=0;i<ulNumberOfChildren;i++)
	{
		//If this node has children, delete them because we're about to create new ones (children are usually
		//created automatically in the constructors even though we don't always want them)
		if(pNewNode->pChildren[i]!=NULL)
		{
			delete pNewNode->pChildren[i];
		}

		//If this is not a prototype,
		if(!nIsPrototype)
		{
			//Make the children of the new node copies of this node's children
			pNewNode->pChildren[i]=pChildren[i]->pGetCopy(pGP);
		}
		else
		{
			//Otherwise make the children of the new node random subprograms
			pNewNode->pChildren[i]=pGP->pGetRandomSubtree();
		}
	}

	//Return the new node and associated subtree
	return (CGPNode*)pNewNode;
}

unsigned long CGPBinaryNode::ulGetNumberOfNodesInSubtree(unsigned long ulNodesFoundSoFar)
{
	//Add the number of nodes in the subtree attached to this to the number of nodes found so far
	//Note that the +2 is necessary because node don't count themselves
	return ulNodesFoundSoFar+2+pChildren[0]->ulGetNumberOfNodesInSubtree(0)+pChildren[1]->ulGetNumberOfNodesInSubtree(0);
}

void CGPBinaryNode::GetnthNode(unsigned long& ulNodesSoFar,unsigned long ulTargetNode,CGPNode **&pTheNode)
{
	//If p TheNode is not equal to NULL, the node has already been found
	if(pTheNode==NULL)
	{
		//If the first child is the target node, get its address
		if(ulTargetNode==ulNodesSoFar)
		{
			pTheNode=&pChildren[0];
		}
		//If the second child is the target node, get its address
		if(ulTargetNode==ulNodesSoFar+1)
		{
			pTheNode=&pChildren[1];
		}
	}
	//Increment the nodes counter
	ulNodesSoFar+=2;

	//And check the nodes in this node's subtree
	pChildren[0]->GetnthNode(ulNodesSoFar,ulTargetNode,pTheNode);
	pChildren[1]->GetnthNode(ulNodesSoFar,ulTargetNode,pTheNode);
}
