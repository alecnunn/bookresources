//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPIfGreaterThanZeroNode_
#define _GPIfGreaterThanZeroNode_

#include "CGPTernaryNode.h"

class CGP;

//See inherited class for comments
class CGPIfGreaterThanZeroNode : public CGPTernaryNode
{
public:
	CGPIfGreaterThanZeroNode();
	~CGPIfGreaterThanZeroNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif