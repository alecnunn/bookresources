//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPTurnRightNode_
#define _GPTurnRightNode_

#include "CGPTerminalNode.h"

class CGP;

//See inherited class for comments
class CGPTurnRightNode : public CGPTerminalNode
{
public:
	CGPTurnRightNode();
	~CGPTurnRightNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif