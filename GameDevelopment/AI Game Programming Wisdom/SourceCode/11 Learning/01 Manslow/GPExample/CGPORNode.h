//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPORNode_
#define _GPORNode_

#include "CGPBinaryNode.h"

class CGP;

//See inherited class for comments
class CGPORNode : public CGPBinaryNode
{
public:
	CGPORNode();
	~CGPORNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif