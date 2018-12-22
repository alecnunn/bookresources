//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPTerminalNode_
#define _GPTerminalNode_

#include "CGPNode.h"

class CGP;

//See inherited class for comments
//Terminal node is a parent class of all classes that contain functions of no arguments
class CGPTerminalNode : public CGPNode
{
public:
	CGPTerminalNode();
	~CGPTerminalNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual unsigned long ulGetNumberOfNodesInSubtree(unsigned long);
	virtual void GetnthNode(unsigned long&,unsigned long,CGPNode**&);
	double dTerminal;
};

#endif