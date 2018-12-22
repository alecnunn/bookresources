//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPIsFoodNode_
#define _GPIsFoodNode_

#include "CGPTerminalNode.h"

class CGP;

//See inherited class for comments
class CGPIsFoodNode : public CGPTerminalNode
{
public:
	CGPIsFoodNode();
	~CGPIsFoodNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif