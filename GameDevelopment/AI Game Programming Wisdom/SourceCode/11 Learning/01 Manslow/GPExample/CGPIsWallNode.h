//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPIsWallNode_
#define _GPIsWallNode_

#include "CGPTerminalNode.h"

class CGP;

//See inherited class for comments
class CGPIsWallNode : public CGPNode
{
public:
	CGPIsWallNode();
	~CGPIsWallNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);

};

#endif