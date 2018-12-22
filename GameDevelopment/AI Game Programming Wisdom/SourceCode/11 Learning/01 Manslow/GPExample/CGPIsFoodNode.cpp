//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPIsFoodNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

extern int nLookForward(void);

CGPIsFoodNode IsFoodPrototype;

CGPIsFoodNode::CGPIsFoodNode()
{
	nType=101;

	pChildren=NULL;
	ulNumberOfChildren=0;

	sprintf(psName,"IsFoodNode");
	nIsPrototype=1-AddToPrototypeList();
	dPrior=0.2;
}

CGPIsFoodNode::~CGPIsFoodNode()
{
}

CGPNode *CGPIsFoodNode::pGetCopy(CGP* pGP)
{
	CGPIsFoodNode *pNewNode=new CGPIsFoodNode;
	assert(!(pNewNode==NULL));
	return (CGPNode*)pNewNode;
}

double CGPIsFoodNode::dEvaluate(void)
{
	//This node tests to see whether the object direction in front of the agent is food. If it is, it returns
	//one and returns zero otherwise
	if(nLookForward()==1)
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

char* CGPIsFoodNode::psGetString(char* pString)
{
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pString,"%s IsFood()",pString);
	}
	return pString;
}
