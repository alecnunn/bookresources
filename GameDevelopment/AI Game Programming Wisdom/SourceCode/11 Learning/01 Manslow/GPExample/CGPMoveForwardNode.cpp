//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPMoveForwardNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern void MoveForward(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPMoveForwardNode MoveForwardPrototype;

CGPMoveForwardNode::CGPMoveForwardNode()
{
	nType=9;

	pChildren=NULL;
	ulNumberOfChildren=0;

	sprintf(psName,"MoveForwardNode");
	nIsPrototype=1-AddToPrototypeList();
	dPrior=0.3;
}

CGPMoveForwardNode::~CGPMoveForwardNode()
{
}

CGPNode *CGPMoveForwardNode::pGetCopy(CGP* pGP)
{
	CGPMoveForwardNode *pNewNode=new CGPMoveForwardNode;
	assert(!(pNewNode==NULL));
	return (CGPNode*)pNewNode;
}

double CGPMoveForwardNode::dEvaluate(void)
{
	//This function moves (or rather tries to move) the agent forward one square. It returns zero
	//regardless of whether the operation was successful or not
	MoveForward();
	return 0.0;
}

char* CGPMoveForwardNode::psGetString(char*pString)
{
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pString,"%s MoveForward()",pString);
	}
	return pString;
}
