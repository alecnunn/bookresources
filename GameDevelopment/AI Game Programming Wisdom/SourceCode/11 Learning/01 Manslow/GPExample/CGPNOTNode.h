//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPNOTNode_
#define _GPNOTNode_

#include "CGPUnaryNode.h"

class CGP;

//See inherited class for comments
class CGPNOTNode : public CGPUnaryNode
{
public:
	CGPNOTNode();
	~CGPNOTNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif