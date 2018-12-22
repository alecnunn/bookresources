//GPExample
//Copyright John Manslow
//29/09/2001

/////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
/////////////////////////////////////////////////////////////

#include "CGPTernaryNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode *pGPCreateRandomSubtree(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPTernaryNode::CGPTernaryNode()
{
}

CGPTernaryNode::~CGPTernaryNode()
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

CGPNode *CGPTernaryNode::pGetCopy(CGP* pGP)
{
	unsigned long i;
	CGPTernaryNode *pNewNode=new CGPTernaryNode;
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

unsigned long CGPTernaryNode::ulGetNumberOfNodesInSubtree(unsigned long ulNodesFoundSoFar)
{
	return ulNodesFoundSoFar+3+pChildren[0]->ulGetNumberOfNodesInSubtree(0)+pChildren[1]->ulGetNumberOfNodesInSubtree(0)+pChildren[2]->ulGetNumberOfNodesInSubtree(0);
}

void CGPTernaryNode::GetnthNode(unsigned long& ulNodesSoFar,unsigned long ulTargetNode,CGPNode **&pTheNode)
{
	if(pTheNode==NULL)
	{
		if(ulTargetNode==ulNodesSoFar)
		{
			pTheNode=&pChildren[0];
		}
		if(ulTargetNode==ulNodesSoFar+1)
		{
			pTheNode=&pChildren[1];
		}
		if(ulTargetNode==ulNodesSoFar+2)
		{
			pTheNode=&pChildren[2];
		}
	}
	ulNodesSoFar+=3;
	pChildren[0]->GetnthNode(ulNodesSoFar,ulTargetNode,pTheNode);
	pChildren[1]->GetnthNode(ulNodesSoFar,ulTargetNode,pTheNode);
	pChildren[2]->GetnthNode(ulNodesSoFar,ulTargetNode,pTheNode);
}
