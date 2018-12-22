//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "CGPNOTNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode *pGPCreateRandomSubtree(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPNOTNode NOTPrototype;

CGPNOTNode::CGPNOTNode()
{
	unsigned long i;
	nType=104;

	pChildren=NULL;
	ulNumberOfChildren=1;

	sprintf(psName,"NOTNode");

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
	dPrior=0.05;
}

CGPNOTNode::~CGPNOTNode()
{
}

CGPNode *CGPNOTNode::pGetCopy(CGP* pGP)
{
	unsigned long i;
	CGPNOTNode *pNewNode=new CGPNOTNode;
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

double CGPNOTNode::dEvaluate(void)
{
	//This function return the arithmetical-logical NOT of the return value of its child
	return 1.0-pChildren[0]->dEvaluate();
}

char* CGPNOTNode::psGetString(char*pString)
{
	char *pSubString=new char[50000];
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pSubString,"");
		pSubString=pChildren[0]->psGetString(pSubString);
		sprintf(pString,"%s NOT(%s)",pString,pSubString);
	}
	delete []pSubString;
	return pString;
}
