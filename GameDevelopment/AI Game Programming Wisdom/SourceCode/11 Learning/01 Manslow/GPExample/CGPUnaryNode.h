//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPUnaryNode_
#define _GPUnaryNode_

#include "CGPNode.h"

class CGP;

//See inherited class for comments
//Unary node is a parent class of all classes that that contain functions of one arguments
class CGPUnaryNode : public CGPNode
{
public:
	CGPUnaryNode();
	~CGPUnaryNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual unsigned long ulGetNumberOfNodesInSubtree(unsigned long);
	virtual void GetnthNode(unsigned long&,unsigned long,CGPNode**&);
};

#endif