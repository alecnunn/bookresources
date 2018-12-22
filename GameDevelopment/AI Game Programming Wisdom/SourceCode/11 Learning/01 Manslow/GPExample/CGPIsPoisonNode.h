//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPIsPoisonNode_
#define _GPIsPoisonNode_

#include "CGPTerminalNode.h"

class CGP;

//See inherited class for comments
class CGPIsPoisonNode : public CGPTerminalNode
{
public:
	CGPIsPoisonNode();
	~CGPIsPoisonNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif