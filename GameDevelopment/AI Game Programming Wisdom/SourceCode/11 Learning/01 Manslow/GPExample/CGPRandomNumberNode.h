//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPRandomNumberNode_
#define _GPRandomNumberNode_

#include "CGPTerminalNode.h"

class CGP;

//See inherited class for comments
class CGPRandomNumberNode : public CGPTerminalNode
{
public:
	CGPRandomNumberNode();
	~CGPRandomNumberNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif