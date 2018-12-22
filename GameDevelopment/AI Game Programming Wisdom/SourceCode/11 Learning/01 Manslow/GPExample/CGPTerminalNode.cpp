//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPTerminalNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPTerminalNode::CGPTerminalNode()
{
}

CGPTerminalNode::~CGPTerminalNode()
{
}

CGPNode *CGPTerminalNode::pGetCopy(CGP* pGP)
{
	CGPTerminalNode *pNewNode=new CGPTerminalNode;
	assert(!(pNewNode==NULL));
	if(!nIsPrototype) 
	{
		pNewNode->dTerminal=dTerminal;
	}
	return (CGPNode*)pNewNode;
}

unsigned long CGPTerminalNode::ulGetNumberOfNodesInSubtree(unsigned long ulNodesFoundSoFar)
{
	return ulNodesFoundSoFar;
}

void CGPTerminalNode::GetnthNode(unsigned long&,unsigned long,CGPNode**&)
{
}
