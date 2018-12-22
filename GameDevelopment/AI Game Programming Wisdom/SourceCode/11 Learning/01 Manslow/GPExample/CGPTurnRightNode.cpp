//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPTurnRightNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern void TurnRight(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPTurnRightNode TurnRightPrototype;

CGPTurnRightNode::CGPTurnRightNode()
{
	nType=11;
	pChildren=NULL;
	ulNumberOfChildren=0;
	sprintf(psName,"TurnRightNode");
	nIsPrototype=1-AddToPrototypeList();
	dPrior=0.1;
}

CGPTurnRightNode::~CGPTurnRightNode()
{
}

CGPNode *CGPTurnRightNode::pGetCopy(CGP* pGP)
{
	CGPTurnRightNode *pNewNode=new CGPTurnRightNode;
	assert(!(pNewNode==NULL));
	return (CGPNode*)pNewNode;
}

double CGPTurnRightNode::dEvaluate(void)
{
	//This function makes the agent turn right 90 degrees
	TurnRight();
	return 0.0;
}

char* CGPTurnRightNode::psGetString(char* pString)
{
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pString,"%s TurnRight()",pString);
	}
	return pString;
}
