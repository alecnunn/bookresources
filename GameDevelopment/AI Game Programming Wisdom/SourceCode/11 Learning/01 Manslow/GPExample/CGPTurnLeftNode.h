//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPTurnLeftNode_
#define _GPTurnLeftNode_

#include "CGPTerminalNode.h"

class CGP;

//See inherited class for comments
class CGPTurnLeftNode : public CGPTerminalNode
{
public:
	CGPTurnLeftNode();
	~CGPTurnLeftNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif