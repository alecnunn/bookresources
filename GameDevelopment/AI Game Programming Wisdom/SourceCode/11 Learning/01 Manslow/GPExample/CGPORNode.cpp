//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "CGPORNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode *pGPCreateRandomSubtree(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPORNode ORPrototype;

CGPORNode::CGPORNode()
{
	unsigned long i;
	nType=105;

	pChildren=NULL;
	ulNumberOfChildren=2;

	sprintf(psName,"ORNode");

	nIsPrototype=1;
	if(AddToPrototypeList())
	{
		nIsPrototype=0;
		pChildren=new CGPNode*[ulNumberOfChildren];
		for(i=0;i<ulNumberOfChildren;i++)
		{
			pChildren[i]=NULL;
		}
	}
	dPrior=0.1;
}

CGPORNode::~CGPORNode()
{
}

CGPNode *CGPORNode::pGetCopy(CGP* pGP)
{
	unsigned long i;
	CGPORNode *pNewNode=new CGPORNode;
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

double CGPORNode::dEvaluate(void)
{
	//This function returns the arithmetical-logical OR of the return values of the node's children
	double dReturn1=pChildren[0]->dEvaluate();
	double dReturn2=pChildren[1]->dEvaluate();
	return dReturn1+dReturn2-dReturn1*dReturn2;
}

char* CGPORNode::psGetString(char*pString)
{
	char *pSubString=new char[50000];
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pSubString,"");
		pSubString=pChildren[0]->psGetString(pSubString);
		sprintf(pString,"%s OR((%s),",pString,pSubString);
		sprintf(pSubString,"");
		pSubString=pChildren[1]->psGetString(pSubString);
		sprintf(pString,"%s(%s))",pString,pSubString);
	}
	delete []pSubString;
	return pString;
}
