//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPTurnLeftNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern void TurnLeft(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPTurnLeftNode TurnLeftPrototype;

CGPTurnLeftNode::CGPTurnLeftNode()
{
	nType=10;
	pChildren=NULL;
	ulNumberOfChildren=0;
	sprintf(psName,"TurnLeftNode");
	nIsPrototype=1-AddToPrototypeList();
	dPrior=0.1;
}

CGPTurnLeftNode::~CGPTurnLeftNode()
{
}

CGPNode *CGPTurnLeftNode::pGetCopy(CGP* pGP)
{
	CGPTurnLeftNode *pNewNode=new CGPTurnLeftNode;
	assert(!(pNewNode==NULL));
	return (CGPNode*)pNewNode;
}

double CGPTurnLeftNode::dEvaluate(void)
{
	//This function makes the agent turn left 90 degrees
	TurnLeft();
	return 0.0;
}

char* CGPTurnLeftNode::psGetString(char* pString)
{
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pString,"%s TurnLeft()",pString);
	}
	return pString;
}
