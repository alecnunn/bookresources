//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _GPNode_
#define _GPNode_

class CGP;

//Base class for all nodes that can appear in a genetic program tree
class CGPNode
{
public:
	CGPNode();
	virtual ~CGPNode();

	//Returns a copy of the node
	virtual CGPNode *pGetCopy(CGP*);

	//Evaluates the node. This causes non-teminal nodes to evaluate their children and return some 
	//function of their return values. Some nodes - usual terminals - cause the agent to perform actions
	//in the world, such a moving forwards, or return sensory information from the agent, such as the
	//type of object directly in front of it
	virtual double dEvaluate(void);

	//Returns a string containing the subprogram starting at this node. In principle, the programs could
	//be placed directly into a scipring engine (provided, of course, that the syntax is appropriately
	//constructed)
	virtual char *psGetString(char*);

	//Returns the number of nodes in the subtree attached to this node
	virtual unsigned long ulGetNumberOfNodesInSubtree(unsigned long);

	//Returns a pointer to the specified node in the subtree
	virtual void GetnthNode(unsigned long&,unsigned long,CGPNode**&);

	//Adds the current node to the prototype list
	virtual int AddToPrototypeList(void);

	//A pointer to an array of child nodes
	CGPNode **pChildren;

	//The number of child nodes attached to this node
	unsigned long ulNumberOfChildren;

	//The type of this node
	int nType;

	//Indicates whether this nodes is a working node or if it is just a prototype
	int nIsPrototype;

	//A weighting factor affecting how likely this type of node is to appear in a randomly generated
	//subtree
	double dPrior;

	//A string name for the node
	char *psName;
};

#endif