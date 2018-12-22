//GAPBILExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////
//Remove this include if not compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////

#include "CPBIL.h"
#include "assert.h"
#include "fstream.h"

CPBIL::CPBIL(
		 const unsigned long	ulNewPopulationSize,
		 const unsigned long	ulNewChromosomeLength
		 )
{
	//Check validity of arguments
	assert(!(ulNewPopulationSize<1));
	ulPopulationSize=ulNewPopulationSize;

	assert(!(ulNewChromosomeLength<1));
	ulChromosomeLength=ulNewChromosomeLength;

	//Set default mutation and adaptation rates
	dMutationRate=0.01/double(ulChromosomeLength);
	dAdaptationRate=0.1;

	//Reset the fitness evaluation counter
	ulIteration=0;

	//Allocate memory to store the population
	AllocateMemory();
}

CPBIL::~CPBIL()
{
	//Clean up
	DeallocateMemory();
}

void CPBIL::AllocateMemory(void)
{
	unsigned long i;

	//Create storage for the population
	ppdGenes=new double*[ulPopulationSize];
	for (i=0;i<ulPopulationSize;i++)
	{
		ppdGenes[i]=new double[ulChromosomeLength];
	}

	//And for the fitnesses of the chromosomes within it
	pdFitnesses=new double[ulPopulationSize];

	//Create storage for the gene probability vector
	pdGeneProbabilities=new double[ulChromosomeLength];

	//Set gene probabilities to 0.5 so that initially, each gene is equally likely to be 0 or 1
	for (i=0;i<ulChromosomeLength;i++)
	{
			pdGeneProbabilities[i]=0.5;
	}

	//Declare stoarge for a copy of the best chromosome discovered so far and set its fitness to a large
	//negative number so that it'll be overwritten immediately
	pdBestChromosome=new double[ulChromosomeLength];
	dBestFitness=-1e+100;
}

void CPBIL::DeallocateMemory(void)
{
	unsigned long i;
	delete []pdFitnesses;

	for (i=0;i<ulPopulationSize;i++)
	{
		delete []ppdGenes[i];
	}
	delete []ppdGenes;
	delete []pdGeneProbabilities;
	delete []pdBestChromosome;
}

void CPBIL::CreatePopulation(void)
{
	//This function is called to create a population of chromosomes by sampling form the probability distribution
	//given in pdGeneProbabilities. 
	unsigned long i,j;

	//For each chromosome in the population
	for (i=0;i<ulPopulationSize;i++)
	{
		//Reset its fitness
		pdFitnesses[i]=0.0;

		//For each gene in the ith chromosome
		for (j=0;j<ulChromosomeLength;j++)
		{
			//Set the gene to either 0 or 1 in accordance with the gene probability vector
			if(double(rand())/double(RAND_MAX)>pdGeneProbabilities[j])
			{
				ppdGenes[i][j]=0.0;
			}
			else
			{
				ppdGenes[i][j]=1.0;
			}
		}
	}
}

void CPBIL::Mutate(void)
{
	//The mutate function resets a randomly selected element of the gene probability vector to 0.5. Mutation
	//can usually be removed with the PBIL - provded that its population size is large and rate of adaptation is
	//small. Mutation should have a much lower probability than in the GA
	unsigned long i;
	for (i=0;i<ulChromosomeLength;i++)
	{
		if(double(rand())/double(RAND_MAX)<dMutationRate)
		{
			pdGeneProbabilities[i]=0.5;
		}
	}
}

double *CPBIL::pdGetChromosomeForEvaluation(void)
{
	//This function automatically selects each chromosome in the population, and once
	//all have been evaluated, creates a new population
	unsigned long i;

	//ff the chromosome selector (i.e. ulIteration%ulPopulationSize) indicates the first chromosome in the
	//population needs to be evaluted, a new population needs to be created
	if (ulIteration%ulPopulationSize==0)
	{
		CreatePopulation();
	}

	//Set the working chromosome to the location of the chromosome in the population so that when SetFitness
	//is called, we know which chromosome it refers to
	ulWorkingChromosome=ulIteration%ulPopulationSize;

	//Create space for a copy of the chromosome that needs to be evaluated
	double *pdChromosomeForEvaluation=new double[ulChromosomeLength];
	for (i=0;i<ulChromosomeLength;i++)
	{
		//Copy the genes into it
		pdChromosomeForEvaluation[i]=ppdGenes[ulWorkingChromosome][i];
	}

	//Return it to the clling function
	return pdChromosomeForEvaluation;
}
	
void CPBIL::SetFitness(const double dNewFitness)
{
	//This function records the ftiness of the chromosome that has just beenevaluated. If the chromosome 
	//selector (i.e. ulIteration%ulPopulationSize) has wrapped around to the start of the population, we have 
	//evaluated the fitnesses of every individual in the population and hence can update the gene probability 
	//vector.
	unsigned long i;
	double dHighestFitness=0.0;
	unsigned long ulFittestChromosome=0;

	//Increment the counter of the number of fitness evaluations
	ulIteration++;

	//Record the fitness
	pdFitnesses[ulWorkingChromosome]=dNewFitness;

	//If its the best discovered so far take a copy of it
	if(dNewFitness>dBestFitness)
	{
		dBestFitness=dNewFitness;
		unsigned long i;
		for(i=0;i<ulChromosomeLength;i++)
		{
			pdBestChromosome[i]=ppdGenes[ulWorkingChromosome][i];
		}
	}

	//If the chromosome selector has wrapped around to the start of the population
	if(ulIteration%ulPopulationSize==0)
	{
		//Find the fittest chromosome in the population
		for(i=0;i<ulPopulationSize;i++)
		{
			if(pdFitnesses[i]>dHighestFitness)
			{
				dHighestFitness=pdFitnesses[i];
				ulFittestChromosome=i;
			}
		}

		//Update the gene probabilities by changing them to increase the chance that the best chromosome in
		//this population is reproduced more frequenctly in future
		for(i=0;i<ulChromosomeLength;i++)
		{
			pdGeneProbabilities[i]+=dAdaptationRate*(ppdGenes[ulFittestChromosome][i]-pdGeneProbabilities[i]);
		}

		//Mutate the probability vector
		Mutate();
	}
}

double *CPBIL::pdGetBestChromosome(void)
{
	//Return a pointer to the best chromosome. Its not a copy, so don't delete it!
	return pdBestChromosome;
}

double CPBIL::dGetBestPerformance(void)
{
	//Return the best fitness achieved so far
	return dBestFitness;
}

int CPBIL::Save(const char * const psFilename)
{
	//Save the PBIL's status
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
	*pOut<<ulIteration;
	*pOut<<"\n";
	*pOut<<ulWorkingChromosome;
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
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pOut<<pdGeneProbabilities[i];
		*pOut<<"\t";
	}
	*pOut<<"\n";
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

int CPBIL::Load(const char * const psFilename)
{
	//Load the PBIL's status
	ifstream *pIn=new ifstream(psFilename,ios::nocreate);
	unsigned long i,j;
	if(!pIn || !pIn->is_open())
	{
		return 0;
	}
	DeallocateMemory();
	*pIn>>ulPopulationSize;
	*pIn>>ulChromosomeLength;
	AllocateMemory();
	*pIn>>dMutationRate;
	*pIn>>ulIteration;
	*pIn>>ulWorkingChromosome;
	for (i=0;i<ulPopulationSize;i++)
	{
		for (j=0;j<ulChromosomeLength;j++)
		{
			*pIn>>ppdGenes[i][j];
		}
		*pIn>>pdFitnesses[i];
	}
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pIn>>pdGeneProbabilities[i];
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
