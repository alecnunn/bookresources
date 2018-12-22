//GPExample
//Copyright John Manslow
//29/09/2001

#ifndef _CGP_
#define _CGP_

//Forward declaration of base class for GP program node
class CGPNode;

class CGP
{
public:

	CGP(unsigned long);			//Population size
	~CGP();

	//Returns a pointer to a new randomly created program
	CGPNode *pGetRandomSubtree(void);

	//Mutates the program passed in the function's parameter list
	CGPNode* pMutate(CGPNode *);

	//Creates a new program by applying a crossover operator to the two program specified (by their locations
	//in the population) in the parameter list
	CGPNode *pCross(unsigned long,unsigned long);

	//Returns a pointer to a progarm so that its fitness can be evaluated
	CGPNode *pGetChromosomeForEvaluation(void);

	//Sets the fitness of the working program to the fitness provided
	void SetFitness(double);

	//Returns a pointer to the best program found so far
	CGPNode *pGetBestChromosome(void);

	//Returns the performance of the best program discovered so far
	double dGetBestPerformance(void);

private:

	//The population size
	unsigned long ulPopulationSize;

	//The number of fitness evaluations made so far
	unsigned long ulIteration;

	//The program whos fitness is currently being evaluated
	unsigned long ulWorkingTree;

	//The population of programs
	CGPNode **pProgram;
	//And their fitnesses
	double *pdFitnesses;

	//Search parameters
	double dMutationProbability;
	double dCrossoverProbability;

	//A copy of the best program discovered so far and its performance
	CGPNode *pFittestTree;
	double dBestFitness;
};

#endif