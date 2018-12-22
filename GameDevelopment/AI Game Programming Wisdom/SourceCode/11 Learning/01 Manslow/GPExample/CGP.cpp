//GPExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////////////

#include "CGP.h"
#include "CGPNode.h"
#include "math.h"
#include "assert.h"
#include "fstream.h"
#include "time.h"

//This is a list of prototype "instructions" or nodes that can be used in the construction of genetic "programs"
//or trees. Nodes are declared as automatic variables in their implementation files and register themselves in 
//the prototype list in their constructors. The prorotype list provides a convenient way of copying and creating
//nodes
CGPNode **pPrototypeList=NULL;
unsigned long ulNumberOfPrototypes=0;

//A log file is useful for debugging
extern ofstream *pLogFile;

CGP::CGP(unsigned long ulNewPopulationSize)
{
	//The constructor allocates memory required for the population and its statistics. It also fills the population
	//with random programs
	unsigned long i;

	//Check parameter
	assert(!(ulNewPopulationSize<1));
	ulPopulationSize=ulNewPopulationSize;

	//Storage for the programs and their fitness statistics
	pProgram=new CGPNode*[ulPopulationSize];
	pdFitnesses=new double[ulPopulationSize];

	//Fill the population with random programs and set their fitnesses to deault values
	for(i=0;i<ulPopulationSize;i++)
	{
		pProgram[i]=pGetRandomSubtree();
		pdFitnesses[i]=0.0;
	}

	//Mutation and crossover probabilities need to be much higher in GP than GA
	dMutationProbability=0.1;
	dCrossoverProbability=0.5;

	//Reset the count of the number of fitness evaluations
	ulIteration=0;

	//Set the fitness statistic of the best program to a large negative number so that it'll be overwritten immediately
	dBestFitness=-1e+100;
	pFittestTree=NULL;
}

CGP::~CGP()
{
	//Deallocate memory
	unsigned long i;
	for(i=0;i<ulPopulationSize;i++)
	{
		delete pProgram[i];
	}
	delete []pProgram;
	delete []pdFitnesses;
	delete pFittestTree;
}

CGPNode *CGP::pGetRandomSubtree(void)
{
	//This function creates a random program (i.e. tree) 
	unsigned long i;
	double dTotalProbability=0.0;

	//Nodes are sampled from the prototype list with probability proportional to their dPrior member varibles.
	//Since we don't know how many elements there'll be in the list, we need to normalise the priors. Obviously,
	//this could be made more efficient by performing the normalisation once in the GP constructor but, since
	//the normalisation is negligible compared to the fitness evalutations, its okay to do it repeatedly here
	double *pdProbabilities=new double[ulNumberOfPrototypes];
	for(i=0;i<ulNumberOfPrototypes;i++)
	{
		dTotalProbability+=pPrototypeList[i]->dPrior;
	}
	for(i=0;i<ulNumberOfPrototypes;i++)
	{
		pdProbabilities[i]=pPrototypeList[i]->dPrior/dTotalProbability;
	}

	//Select a node using a "roulette wheel". We'll choose the node that contains this cumulative probability
	double dTargetProbability=double(rand())/double(RAND_MAX);

	//The prototype that will be chosen
	unsigned long ulPrototype=0;

	//The cumulative probability
	double dAccumulator=pdProbabilities[ulPrototype];

	//Repeat until the target cumulative probability lies within the current node
	while(dTargetProbability>dAccumulator+1e-14)
	{
		ulPrototype++;
		dAccumulator+=pdProbabilities[ulPrototype];
	}
	delete []pdProbabilities;

	//Return a copy of the selected node
	return pPrototypeList[ulPrototype]->pGetCopy(this);
}

CGPNode* CGP::pMutate(CGPNode *pTreeToMutate)
{
	//This function mutates the progarm (tree) passed as the argument. 
	assert(pTreeToMutate);

	//First, find out how many nodes there are in the program tree so that we can pick one as the target 
	//of the mutation
	unsigned long ulNumberOfNodesInTree=pTreeToMutate->ulGetNumberOfNodesInSubtree(0);

	//Decide whether we're actually going to mutate this program or not
	if(double(rand())/double(RAND_MAX)>dMutationProbability)
	{
		//If not, return the program unmodified
		return pTreeToMutate;
	}

	//If there are no nodes in the tree below the top level,
	if(ulNumberOfNodesInTree==0)
	{
		//Delete the entire tree
		delete pTreeToMutate;

		//And create a new random one and return it
		pTreeToMutate=pGetRandomSubtree();
		return pTreeToMutate;
	}
	else
	{
		//Otherwise,
		CGPNode **pNode=NULL;
		unsigned long ulNodeCounter=0;

		//Pick one of the nodes to mutate
		unsigned long ulNodeToMutate=rand()%ulNumberOfNodesInTree;

		//Get a pointer to ut
		pTreeToMutate->GetnthNode(ulNodeCounter,ulNodeToMutate,pNode);

		//Delete the existing subtree
		delete *pNode;

		//And create a new one
		*pNode=pGetRandomSubtree();
	}
	return pTreeToMutate;
}

CGPNode *CGP::pCross(unsigned long ulParentA,unsigned long ulParentB)
{
	//This function produces a child program by crossing the programs at the locations in the population 
	//indicated by the function's arguments. Crossover is performed by randomly selecting crossover
	//points in each of the parent programs trees and swapping the subtrees associated with them

	//First, find out how many nodes there are in each of the parents (excluding the root node)
	unsigned long ulNumberOfNodesInParentA=pProgram[ulParentA]->ulGetNumberOfNodesInSubtree(0);
	unsigned long ulNumberOfNodesInParentB=pProgram[ulParentB]->ulGetNumberOfNodesInSubtree(0);

	unsigned long ulSourceNode,ulDestinationNode;

	//Parent B will form the basis of the child, so take a copy of it
	CGPNode *pChildTree=pProgram[ulParentB]->pGetCopy(this);

	//Decide randomly whether crossover will be performed at all
	if(double(rand())/double(RAND_MAX)>dCrossoverProbability)
	{
		//If not, return the child tree unmodified
		return pChildTree;
	}

	//If there are enough nodes in parent A to select a crossover point
	if(ulNumberOfNodesInParentA>0)
	{
		//Select a crossover site in parent A
		ulSourceNode=rand()%(ulNumberOfNodesInParentA);
	}
	else
	{
		//If not, crossover will not be performed, so return the child tree unmodified
		return pChildTree;
	}

	//If there are enough nodes in parent B to perform crossover
	if(ulNumberOfNodesInParentB>0)
	{
		//Select a crossover site
		ulDestinationNode=rand()%(ulNumberOfNodesInParentB);
	}
	else
	{
		//Otherwise, crossover cannot be performed, so return the child tree unmodified 
		return pChildTree;
	}

	CGPNode **pNode=NULL;
	unsigned long ulNodeCounter;

	//Get a pointer to the node at the child tree's crossover site
	ulNodeCounter=0;
	pChildTree->GetnthNode(ulNodeCounter,ulDestinationNode,pNode);

	//Delete the existing subtree
	delete *pNode;
	CGPNode **pNewSubtree=NULL;

	//Get a pointer to the node at parent A's crossover site
	ulNodeCounter=0;
	pProgram[ulParentA]->GetnthNode(ulNodeCounter,ulSourceNode,pNewSubtree);

	//Set the node at the child's crossover site to point to a copy of the subtree at parent A's
	//crossover site
	*pNode=(*pNewSubtree)->pGetCopy(this);
	return pChildTree;
}


CGPNode *CGP::pGetChromosomeForEvaluation(void)
{
	//Only this function and SetFitness need to be called to make this GP class work. This function
	//selects two parents from the population, mates them to produce a child program by crossover,
	//mutates the child and places it in the population in place of the least fit parent. A pointer to the 
	//child is returned so that its fitness can be evaluated

	//If we've not yet evaluated the fitness of every program in the population, select the next one in line.
	//At this stage we don't need to create any new programs because we've not yet tried all the ones
	//we have
	if(ulIteration<ulPopulationSize)
	{
		ulWorkingTree=ulIteration;
	}
	else
	{
		//If we have measured the fitness of all programs in the population, we need to create new ones
		//by applying the crossover and mutation operators
		CGPNode *pChild;
		unsigned long ulParentA,ulParentB;

		//The crossover operator requires two different parents, so select them at random from the 
		//population
		ulParentA=rand()%ulPopulationSize;
		do
		{
			ulParentB=rand()%ulPopulationSize;
		}
		while(ulParentA==ulParentB);
		ulWorkingTree=ulParentA;

		//Produce a child program by crossing the parents
		pChild=pCross(ulParentA,ulParentB);

		//Mutate the child
		pChild=pMutate(pChild);

		//If parent A was fittest
		if(pdFitnesses[ulParentA]>pdFitnesses[ulParentB])
		{
			//the new program will replace parent B
			delete pProgram[ulParentB];
			pProgram[ulParentB]=pChild;
			ulWorkingTree=ulParentB;
		}
		else
		{
			//otherwise, itwill replace parent A
			delete pProgram[ulParentA];
			pProgram[ulParentA]=pChild;
			ulWorkingTree=ulParentA;
		}
	}
	//Return a pointer to the newly created program so that its fitness can be evaluated
	return pProgram[ulWorkingTree];
}

void CGP::SetFitness(double dNewFitness)
{
	//Keep track of the number of fitness evaluations
	ulIteration++;

	//Record the fitness of the rpogram that was just evaluated
	pdFitnesses[ulWorkingTree]=dNewFitness;

	//If the program performed bettwe than the best so far
	if(dNewFitness-dBestFitness>0)
	{
		//Record its fitness and take a copy of the program
		dBestFitness=dNewFitness;
		if(pFittestTree!=NULL)
		{
			delete pFittestTree;
		}
		pFittestTree=pProgram[ulWorkingTree]->pGetCopy(this);
		TRACE("New fittest:\t%+16.3lf\n",dBestFitness);

		//Also, dump some info to the log file		
		char pString[10000];
		sprintf(pString,"CGP::NewBestFitness:	Chromosome %5u has fitness %+18.3lf on iteration %u",ulWorkingTree,dNewFitness,ulIteration);
		*pLogFile<<pString;
		*pLogFile<<"\n";
		sprintf(pString,"");
		pProgram[ulWorkingTree]->psGetString(pString);
		*pLogFile<<pString;
		*pLogFile<<"\n";
		sprintf(pString,"");
		_strtime(pString);
		*pLogFile<<pString;
		*pLogFile<<"\n";
	}
}

CGPNode *CGP::pGetBestChromosome()
{
	//Returns a pointer to the best program found so far. It is not a copy, so don't delete it!
	return pFittestTree;
}

double CGP::dGetBestPerformance(void)
{
	//Returns the best performance so far achieved
	return dBestFitness;
}