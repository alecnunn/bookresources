//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when comoiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "CGPANDNode.h"
#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

extern CGPNode *pGPCreateRandomSubtree(void);

extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

//All real classes (that is, noes that can actually form part of a program) have an automatic declaration
//like this, which causes the prortype list to be created and the class to be entered into it
CGPANDNode ANDPrototype;

CGPANDNode::CGPANDNode()
{
	unsigned long i;

	//Create the class name
	sprintf(psName,"ANDNode");

	//Define its type ID
	nType=100;

	//Set the children pointer to NULL so we know its uninitialsied
	pChildren=NULL;

	//AND has two arguments (and hence children)
	ulNumberOfChildren=2;

	//Try to add the node to the prortype list (and in so doing find out whether it is a prototype)
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

CGPANDNode::~CGPANDNode()
{
}

CGPNode *CGPANDNode::pGetCopy(CGP* pGP)
{
	unsigned long i;

	CGPANDNode *pNewNode=new CGPANDNode;
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

double CGPANDNode::dEvaluate(void)
{
	//Return the arithmetical-logical  AND of the return values of the node's children
	return pChildren[0]->dEvaluate()*pChildren[1]->dEvaluate();
}

char* CGPANDNode::psGetString(char* pString)
{
	//Append this node's function and that of its associated subtree to the string passed into the funciton
	char *pSubString=new char[50000];
	if(pString!=NULL && strlen(pString)<10000)
	{
		sprintf(pSubString,"");
		pSubString=pChildren[0]->psGetString(pSubString);
		sprintf(pString,"%s AND((%s),",pString,pSubString);
		sprintf(pSubString,"");
		pSubString=pChildren[1]->psGetString(pSubString);
		sprintf(pString,"%s(%s))",pString,pSubString);
	}
	delete []pSubString;
	return pString;
}

