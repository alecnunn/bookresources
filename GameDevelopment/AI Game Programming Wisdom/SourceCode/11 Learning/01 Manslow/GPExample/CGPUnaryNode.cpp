//GPExample
//Copyright John Manslow
//29/09/2001

///////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
///////////////////////////////////////////////////////////

#include "CGPUnaryNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode *pGPCreateRandomSubtree(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPUnaryNode::CGPUnaryNode()
{
}

CGPUnaryNode::~CGPUnaryNode()
{
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

CGPNode *CGPUnaryNode::pGetCopy(CGP* pGP)
{
	unsigned long i;
	CGPUnaryNode *pNewNode=new CGPUnaryNode;
	assert(!(pNewNode==NULL));
	for(i=0;i<ulNumberOfChildren;i++)
	{
		if(pNewNode->pChildren[i]!=NULL)
		{
			delete pNewNode->pChildren[i];
		}
		if(!nIsPrototype)
		{
			pNewNode->pChildren[i]=pChildren[i]->pGetCopy(pGP);
		}
		else
		{
			pNewNode->pChildren[i]=pGP->pGetRandomSubtree();
		}
	}
	return (CGPNode*)pNewNode;
}

unsigned long CGPUnaryNode::ulGetNumberOfNodesInSubtree(unsigned long ulNodesFoundSoFar)
{
	return ulNodesFoundSoFar+1+pChildren[0]->ulGetNumberOfNodesInSubtree(0);
}

void CGPUnaryNode::GetnthNode(unsigned long& ulNodesSoFar,unsigned long ulTargetNode,CGPNode **&pTheNode)
{
	if(pTheNode==NULL)
	{
		if(ulTargetNode==ulNodesSoFar)
		{
			pTheNode=&pChildren[0];
		}
	}
	ulNodesSoFar+=1;
	pChildren[0]->GetnthNode(ulNodesSoFar,ulTargetNode,pTheNode);
}
