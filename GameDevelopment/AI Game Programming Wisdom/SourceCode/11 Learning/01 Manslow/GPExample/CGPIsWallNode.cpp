//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "math.h"
#include "CGPIsWallNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

extern int nLookForward(void);

CGPIsWallNode IsWallPrototype;

CGPIsWallNode::CGPIsWallNode()
{
	nType=103;

	pChildren=NULL;
	ulNumberOfChildren=0;

	sprintf(psName,"IsWallNode");
	nIsPrototype=1-AddToPrototypeList();
	dPrior=0.05;
}

CGPIsWallNode::~CGPIsWallNode()
{
}

CGPNode *CGPIsWallNode::pGetCopy(CGP* pGP)
{
	CGPIsWallNode *pNewNode=new CGPIsWallNode;
	assert(!(pNewNode==NULL));
	return (CGPNode*)pNewNode;
}

double CGPIsWallNode::dEvaluate(void)
{
	//This function returns one if the object in front of the agent is a wall and zero otherwise
	if(nLookForward()==2)
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

char* CGPIsWallNode::psGetString(char* pString)
{
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pString,"%s IsWall()",pString);
	}
	return pString;
}
