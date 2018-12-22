//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
#define false FALSE
////////////////////////////////////////////////////////////

#include "CGPNode.h"
#include "CGP.h"
#include "assert.h"

//Information about the prototype list to enable nodes to register themselves
extern CGPNode **pPrototypeList;
extern unsigned long ulNumberOfPrototypes;

CGPNode::CGPNode()
{
	//Initialise the node's name
	psName=new char[25];
	sprintf(psName,"BaseClassNode");

	//Default properties
	ulNumberOfChildren=0;
	pChildren=NULL;
	nType=3;
}

CGPNode::~CGPNode()
{
	delete []psName;
}

int CGPNode::AddToPrototypeList()
{
	//This function is called wheever a node is constrcuted. First, a check is made to see if a node
	//of the same type is already present in the prototype list. If so, the new node is not added. If this
	//type is not yet represented in the prototype list, this node must be a prototype, so we add it
	unsigned long i;

	//Check to see if a node of this type is already listed
	int nIsIn=0;
	for(i=0;i<ulNumberOfPrototypes;i++)
	{
		if(pPrototypeList[i]->nType==nType)
		{
			nIsIn=1;
		}
	}

	//If not,
	if(!nIsIn)
	{
		//Create a new prototype list that does contain it
		CGPNode **pOldPrototypeList=pPrototypeList;
		ulNumberOfPrototypes++;
		pPrototypeList=new CGPNode*[ulNumberOfPrototypes];
		for(i=0;i<ulNumberOfPrototypes-1;i++)
		{
			pPrototypeList[i]=pOldPrototypeList[i];
		}
		pPrototypeList[i]=this;

		//Remeber to get rid of the old one!
		if(pOldPrototypeList!=NULL)
		{
			delete []pOldPrototypeList;
		}
	}
	return nIsIn;
}

CGPNode *CGPNode::pGetCopy(CGP*)
{
	//In real nodes, this function creates and returns an exact copy of of the class and its contents
	assert(false);
	return NULL;
}

double CGPNode::dEvaluate(void)
{
	//Forces the node to evaluate itself (see the header file for more detail, and one of the real nodes for a 
	//specific example)
	assert(false);
	return NULL;
}

char* CGPNode::psGetString(char*)
{
	//This function appends a string representing the function of the subprogram attached to this node to the string
	//passed to this function. See one of the real nodes for a concrete example
	assert(false);
	return NULL;
}

unsigned long CGPNode::ulGetNumberOfNodesInSubtree(unsigned long ulNodesFoundSoFar)
{
	//This function allows the calling function to count the number of nodes in the subprogram attached to this
	//node. Used mainly when mutation and crossover points.
	return ulNodesFoundSoFar;
}

void CGPNode::GetnthNode(unsigned long&,unsigned long,CGPNode**&)
{
	//This function gives a pointer to the nth node in the subtree attached to this node. Again, used mainly 
	//when mutation and crossover points.
}

