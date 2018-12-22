//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "CGPIfGreaterThanZeroNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode *pGPCreateRandomSubtree(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPIfGreaterThanZeroNode IfGreaterThanZeroPrototype;

CGPIfGreaterThanZeroNode::CGPIfGreaterThanZeroNode()
{
	unsigned long i;
	nType=7;

	pChildren=NULL;
	ulNumberOfChildren=3;

	sprintf(psName,"IfGreaterThanZeroNode");

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
	dPrior=0.2;
}

CGPIfGreaterThanZeroNode::~CGPIfGreaterThanZeroNode()
{
}

CGPNode *CGPIfGreaterThanZeroNode::pGetCopy(CGP* pGP)
{
	unsigned long i;

	CGPIfGreaterThanZeroNode *pNewNode=new CGPIfGreaterThanZeroNode;
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

double CGPIfGreaterThanZeroNode::dEvaluate(void)
{
	//This node the subtree connected to its second child if the evaluation of the subtree connected
	//to its first child is positive, and evaluates the subtree connected to its third child otherwise. It
	//returns one if the second subtree was evaluated and zero otherwise
	if(pChildren[0]->dEvaluate()>0.0)
	{
		pChildren[1]->dEvaluate();
		return 1.0;
	}
	pChildren[2]->dEvaluate();
	return 0.0;
}

char* CGPIfGreaterThanZeroNode::psGetString(char*pString)
{
	char *pSubString=new char[50000];
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pSubString,"");
		pSubString=pChildren[0]->psGetString(pSubString);
		sprintf(pString,"%s If(%s)",pString,pSubString);
		sprintf(pSubString,"");
		pSubString=pChildren[1]->psGetString(pSubString);
		sprintf(pString,"%sThen(%s)",pString,pSubString);
		sprintf(pSubString,"");
		pSubString=pChildren[2]->psGetString(pSubString);
		sprintf(pString,"%sElse(%s)",pString,pSubString);
	}
	delete []pSubString;
	return pString;
}
