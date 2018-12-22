//GAPBILExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////
//Remove this include if not compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////

#include "CGA.h"
#include "assert.h"
#include "fstream.h"

CGA::CGA(
		 const unsigned long	ulNewPopulationSize,
		 const unsigned long	ulNewChromosomeLength,
		 const int * const			pnNewGeneTypes
		 )
{
	unsigned long i;

	//Use asserrts to check the validity of the parameters
	assert(!(ulNewPopulationSize<1));
	ulPopulationSize=ulNewPopulationSize;

	assert(!(ulNewChromosomeLength<1));
	ulChromosomeLength=ulNewChromosomeLength;

	//Default values of the mutation and crossover probabilities gives an average of one mutation and
	//crossover per mating
	dMutationRate=1.0/double(ulChromosomeLength);
	dCrossoverRate=1.0/double(ulChromosomeLength);

	//Allocate memory to store the population and fitnesses, etc.
	AllocateMemory();

	//Record the types of each gene
	for (i=0;i<ulChromosomeLength;i++)
	{
		pnGeneTypes[i]=pnNewGeneTypes[i];
	}

	//And initialise the population with random individuals
	InitialisePopulation();
}

CGA::~CGA()
{
	//Deallocate memory!
	DeallocateMemory();
}

void CGA::AllocateMemory(void)
{
	unsigned long i;

	//An array to store the type of each gene. Genes can be either binary (type 0) or real (type 1)
	pnGeneTypes=new int[ulChromosomeLength];

	//Store the actual genes of the population
	ppdGenes=new double*[ulPopulationSize];
	for (i=0;i<ulPopulationSize;i++)
	{
		//(each individual n the population is one chromosme)
		ppdGenes[i]=new double[ulChromosomeLength];
	}

	//An array to store the fitnesses of members of the population
	pdFitnesses=new double[ulPopulationSize];

	//Space to store a copy of the best individual found so far
	pdBestChromosome=new double[ulChromosomeLength];
}

void CGA::DeallocateMemory(void)
{
	unsigned long i;

	//Deallocate everything
	delete []pnGeneTypes;
	delete []pdFitnesses;
	delete []pdBestChromosome;

	for (i=0;i<ulPopulationSize;i++)
	{
		delete []ppdGenes[i];
	}
	delete []ppdGenes;
}

void CGA::InitialisePopulation(void)
{
	unsigned long i,j;

	//Since this effectively resets any evolution that has occured, we might as well set the iteration counter
	ulIteration=0;

	//Also set the best fitness to a large negative value so that it will be overwritten immediately. Of
	//course, make sure that no fitness evaluations can result in fitnesses more negative than this
	dBestFitness=-1e+100;

	//Initialise the population and the fitness statistics of its individual members
	for (i=0;i<ulPopulationSize;i++)
	{
		//Set the recorded fitnesses of all members of the population to zero. These values are never used,
		//so could be almost anything (see SetFitness function)
		pdFitnesses[i]=0.0;

		//Sets the genes of the ith chromosome to random values
		for (j=0;j<ulChromosomeLength;j++)
		{
			if(pnGeneTypes[j]==0)
			{
				//If this gene is binary set it to either 0 or 1
				ppdGenes[i][j]=double(rand()%2);
			}
			else
			{
				//Otherwise, set it to a random value between 0 and 1
				ppdGenes[i][j]=double(rand())/double(RAND_MAX);
			}
		}
	}
}

void CGA::Mutate(double * const pdGenes)
{
	unsigned long i;

	//Make sure the argument is valid
	assert(pdGenes);

	//For every gene in the chromosome
	for (i=0;i<ulChromosomeLength;i++)
	{
		//Do we want to mutate this gene?
		if(double(rand())/double(RAND_MAX)<dMutationRate)
		{
			//If so
			if(pnGeneTypes[i]==0)
			{
				//If the gene is binary, flip the bit
				pdGenes[i]=1.0-pdGenes[i];
			}
			else
			{
				//If the gene is real, perturb it
				pdGenes[i]=double(rand())/double(RAND_MAX);
			}
		}
	}
}

void CGA::Crossover(
					const double * const pdParentA, 
					const double * const pdParentB,
					const unsigned long ulLocationForChild
					)
{
	unsigned long i;

	//We'll use this array to mark locations for crossover
	int *pnCrossoverSites=new int[ulChromosomeLength];
	int nParentSelector;

	//Prepare storage for teh child
	double *pdChild=new double[ulChromosomeLength];

	//For every gene
	for(i=0;i<ulChromosomeLength;i++)
	{
		//Decide whether it'll be a crossover site
		if(double(rand())/double(RAND_MAX)<dCrossoverRate)
		{
			//If so, mark it with a one
			pnCrossoverSites[i]=1;
		}
		else
		{
			//Otherwise, mark it with a zero
			pnCrossoverSites[i]=0;
		}
	}

	//This variable selects which parent genes in the child will come from. Initially, they will come from the first
	//parent
	nParentSelector=0;

	//For each gene in the child
	for(i=0;i<ulChromosomeLength;i++)
	{
		//If we're copying this gene from the first parent
		if (nParentSelector==0)
		{
			//Copy it
			pdChild[i]=pdParentA[i];
		}
		else
		{
			//Otherwise, copy it from the other parent
			pdChild[i]=pdParentB[i];
		}

		//If this is a crossover site
		if(pnCrossoverSites[i]==1)
		{
			//Change the parent selector to copy genes from the other parent
			nParentSelector=1-nParentSelector;
		}
	}

	//Delete the chromosome that the child will replace in the population
	delete []ppdGenes[ulLocationForChild];

	//Delete the crossover site markers
	delete []pnCrossoverSites;

	//Put the new chromosome into the population in the specified location
	ppdGenes[ulLocationForChild]=pdChild;
}

void CGA::Mate(void)
{
	unsigned long ulParentA,ulParentB;

	//Choose two parents randomly form the populaiton
	ulParentA=rand()%ulPopulationSize;
	ulParentB=rand()%ulPopulationSize;

	//If they're the same
	while(ulParentA==ulParentB)
	{
		//Re-choose parent the second parent
		ulParentB=rand()%ulPopulationSize;
	}

	//We'll want to replace the least fit of the parents with the child, so if parent A is fittest,
	if (pdFitnesses[ulParentA]>pdFitnesses[ulParentB])
	{
		//Create the child by crossover, and replace parent B with it in the population
		Crossover(ppdGenes[ulParentA],ppdGenes[ulParentB],ulParentB);

		//Set the working chromosome to the position of the child in the population (so that when 
		//SetFitness is called we'll know which individual it is referring to).
		ulWorkingChromosome=ulParentB;
	}
	else
	{
		//If parent B was fittest, create a child by crossover and replace parent A
		Crossover(ppdGenes[ulParentA],ppdGenes[ulParentB],ulParentA);
		ulWorkingChromosome=ulParentA;
	}

	//Mutate the child
	Mutate(ppdGenes[ulWorkingChromosome]);
}

double *CGA::pdGetChromosomeForEvaluation(void)
{
	//This and SetFitness are the only functions that need to be called to make the GA work.
	unsigned long i;
	
	//If we've not yet evaluated every individual in the population at least once,
	if (ulIteration<ulPopulationSize)
	{
		//Prepare to evaluate the fitness of the ulIteration member of the population
		ulWorkingChromosome=ulIteration;
	}
	else
	{
		//Otherwise, create a new individual and place it in the population ready to be evaluated
		Mate();
	}

	//Prepare some storage for a copy of the new chromosome so that it can be returned to the calling
	//function and its fitness evaluated
	double *pdChromosomeForEvaluation=new double[ulChromosomeLength];
	for (i=0;i<ulChromosomeLength;i++)
	{
		//Copy the new chromosome
		pdChromosomeForEvaluation[i]=ppdGenes[ulWorkingChromosome][i];
	}

	//Return it
	return pdChromosomeForEvaluation;
}
	
void CGA::SetFitness(const double dNewFitness)
{
	//This is the only other function that needs to be called and is used to set the fitness of a chromosome
	//that has just been evaluated

	//Actually set the chromosome's fitness
	pdFitnesses[ulWorkingChromosome]=dNewFitness;

	//If the fitness is higher than anything else yet achieved
	if(dNewFitness>dBestFitness)
	{
		//Record it
		dBestFitness=dNewFitness;
		unsigned long i;

		//And keep a copy of the chromosome
		for(i=0;i<ulChromosomeLength;i++)
		{
			pdBestChromosome[i]=ppdGenes[ulWorkingChromosome][i];
		}
	}

	//Record another fitness evaluation
	ulIteration++;
}

double *CGA::pdGetBestChromosome(void)
{
	//Returns a pointer to the best chromosome discovered so far. Don't delete!
	return pdBestChromosome;
}

double CGA::dGetBestPerformance(void)
{
	//Return the best fitness achieved so far
	return dBestFitness;
}

int CGA::Save(const char * const psFilename)
{
	//Saves the status of the GA
	ofstream *pOut=new ofstream(psFilename);
	unsigned long i,j;
	if(!pOut || !pOut->is_open())
	{
		return 0;
	}
	pOut->precision(18);
	*pOut<<ulPopulationSize;
	*pOut<<"\n";
	*pOut<<ulChromosomeLength;
	*pOut<<"\n";
	*pOut<<dMutationRate;
	*pOut<<"\n";
	*pOut<<dCrossoverRate;
	*pOut<<"\n";
	*pOut<<ulIteration;
	*pOut<<"\n";
	*pOut<<ulWorkingChromosome;
	*pOut<<"\n";
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pOut<<pnGeneTypes[i];
		*pOut<<"\t";
	}
	*pOut<<"\n";
	for (i=0;i<ulPopulationSize;i++)
	{
		for (j=0;j<ulChromosomeLength;j++)
		{
			*pOut<<ppdGenes[i][j];
			*pOut<<"\t";
		}
		*pOut<<pdFitnesses[i];
		*pOut<<"\n";
	}
	*pOut<<dBestFitness;
	*pOut<<"\n";
	for (j=0;j<ulChromosomeLength;j++)
	{
		*pOut<<pdBestChromosome[j];
		*pOut<<"\t";
	}
	*pOut<<"\n";
	pOut->close();
	delete pOut;
	return 1;
}

int CGA::Load(const char * const psFilename)
{
	//And load it!
	ifstream *pIn=new ifstream(psFilename,ios::nocreate);
	unsigned long i,j;
	if(!pIn || !pIn->is_open())
	{
		return 0;
	}

	//Free up memory used by the GA as it is now
	DeallocateMemory();

	*pIn>>ulPopulationSize;
	*pIn>>ulChromosomeLength;

	//Allocate memory required to store the new GA that is about to be loaded
	AllocateMemory();
	*pIn>>dMutationRate;
	*pIn>>dCrossoverRate;
	*pIn>>ulIteration;
	*pIn>>ulWorkingChromosome;
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pIn>>pnGeneTypes[i];
	}
	for (i=0;i<ulPopulationSize;i++)
	{
		for (j=0;j<ulChromosomeLength;j++)
		{
			*pIn>>ppdGenes[i][j];
		}
		*pIn>>pdFitnesses[i];
	}
	*pIn>>dBestFitness;
	for (j=0;j<ulChromosomeLength;j++)
	{
		*pIn>>pdBestChromosome[j];
	}
	pIn->close();
	delete pIn;
	return 1;
}
