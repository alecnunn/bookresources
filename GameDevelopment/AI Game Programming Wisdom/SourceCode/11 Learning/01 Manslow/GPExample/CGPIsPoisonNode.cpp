//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPIsPoisonNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

extern int nLookForward(void);

CGPIsPoisonNode IsPoisonPrototype;

CGPIsPoisonNode::CGPIsPoisonNode()
{
	nType=102;

	pChildren=NULL;
	ulNumberOfChildren=0;

	sprintf(psName,"IsPoisonNode");
	nIsPrototype=1-AddToPrototypeList();
	dPrior=0.2;
}

CGPIsPoisonNode::~CGPIsPoisonNode()
{
}

CGPNode *CGPIsPoisonNode::pGetCopy(CGP* pGP)
{
	CGPIsPoisonNode *pNewNode=new CGPIsPoisonNode;
	assert(!(pNewNode==NULL));
	return (CGPNode*)pNewNode;
}

double CGPIsPoisonNode::dEvaluate(void)
{
	//This function returns one if the object directly in front of the agent is poison and zero otherwise
	if(nLookForward()==3)
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

char* CGPIsPoisonNode::psGetString(char* pString)
{
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pString,"%s IsPoison()",pString);
	}
	return pString;
}
