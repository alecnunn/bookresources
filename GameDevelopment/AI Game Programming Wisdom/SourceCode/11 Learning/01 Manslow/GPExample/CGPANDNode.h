//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPANDNode_
#define _GPANDNode_

#include "CGPBinaryNode.h"

class CGP;

//See inherited class for comments
class CGPANDNode : public CGPBinaryNode
{
public:
	CGPANDNode();
	~CGPANDNode();
	virtual CGPNode *pGetCopy(CGP*);
	virtual double dEvaluate(void);
	virtual char *psGetString(char*);
};

#endif