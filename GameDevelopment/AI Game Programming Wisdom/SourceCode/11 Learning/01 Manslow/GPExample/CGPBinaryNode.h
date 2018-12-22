//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPBinaryNode_
#define _GPBinaryNode_

#include "CGPNode.h"

class CGP;

//See inherited class for comments
//Binary node is a parent class of all classes that contain functions of two arguments
class CGPBinaryNode : public CGPNode
{
public:
	CGPBinaryNode();
	~CGPBinaryNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual unsigned long ulGetNumberOfNodesInSubtree(unsigned long);
	virtual void GetnthNode(unsigned long&,unsigned long,CGPNode**&);
};

#endif