//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPTernaryNode_
#define _GPTernaryNode_

#include "CGPNode.h"

class CGP;

//See inherited class for comments
//Ternary node is a parent class of all classes that contain functions of three arguments
class CGPTernaryNode : public CGPNode
{
public:
	CGPTernaryNode();
	~CGPTernaryNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual unsigned long ulGetNumberOfNodesInSubtree(unsigned long);
	virtual void GetnthNode(unsigned long&,unsigned long,CGPNode**&);
};

#endif