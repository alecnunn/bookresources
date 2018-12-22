//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPRandomNumberNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPRandomNumberNode RandomNumberPrototype;

CGPRandomNumberNode::CGPRandomNumberNode()
{
	nType=12;

	pChildren=NULL;
	ulNumberOfChildren=0;

	sprintf(psName,"RandomNumberNode");
	nIsPrototype=1-AddToPrototypeList();
	dPrior=0.1;
}

CGPRandomNumberNode::~CGPRandomNumberNode()
{
}

CGPNode *CGPRandomNumberNode::pGetCopy(CGP* pGP)
{
	CGPRandomNumberNode *pNewNode=new CGPRandomNumberNode;
	assert(!(pNewNode==NULL));
	return (CGPNode*)pNewNode;
}

double CGPRandomNumberNode::dEvaluate(void)
{
	//This funciton returns zero of one, each  with probability 0.5
	return rand()%2;
}

char* CGPRandomNumberNode::psGetString(char* pString)
{
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pString,"%s Random()",pString);
	}
	return pString;
}
