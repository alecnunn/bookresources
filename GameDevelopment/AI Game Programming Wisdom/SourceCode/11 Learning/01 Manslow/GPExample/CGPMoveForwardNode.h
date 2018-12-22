//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPMoveForwardNode_
#define _GPMoveForwardNode_

#include "CGPTerminalNode.h"

class CGP;

//See inherited class for comments
class CGPMoveForwardNode : public CGPTerminalNode
{
public:
	CGPMoveForwardNode();
	~CGPMoveForwardNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif