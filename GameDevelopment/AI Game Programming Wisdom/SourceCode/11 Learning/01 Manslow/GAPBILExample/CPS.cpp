//GAPBILExample
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////
//Remove this include if not compiling under Windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////

#include "CPS.h"
#include "assert.h"
#include "fstream.h"

CPS::CPS(
		 const unsigned long	ulNewChromosomeLength,
		 const int * const			pnNewGeneTypes
		 )
{
	unsigned long i;

	//Use asserrts to check the validity of the parameters
	assert(!(ulNewChromosomeLength<1));
	ulChromosomeLength=ulNewChromosomeLength;

	//The default initial values for the mutation rate and step size are quite large to allow
	//the search to be quite broad at the start. This makes sense because at the start of the
	//search any behaviours we discover are likely to be poor, so we want to spend most 
	//of our time exploring new ones. These values may need to be changed for different
	//applications (particularly the initial step size)
	dMutationRate=0.5;
	dStepSize=0.5;

	//Allocate memory.
	AllocateMemory();

	//Record the types of each gene
	for (i=0;i<ulChromosomeLength;i++)
	{
		pnGeneTypes[i]=pnNewGeneTypes[i];
	}

	//And initialise the search with a random chromosome
	InitialisePopulation();
}

CPS::~CPS()
{
	//Deallocate memory!
	DeallocateMemory();
}

void CPS::AllocateMemory(void)
{
	//An array to store the type of each gene. Genes can be either binary (type 0) or real (type 1)
	pnGeneTypes=new int[ulChromosomeLength];

	//A place to store the working chromosome (that which is having its fitness evaluated)
	pdGenes=new double[ulChromosomeLength];

	//Space to store a copy of the best chromosome found so far
	pdBestChromosome=new double[ulChromosomeLength];
}

void CPS::DeallocateMemory(void)
{
	//Deallocate everything
	delete []pnGeneTypes;
	delete []pdBestChromosome;
	delete []pdGenes;
}

void CPS::InitialisePopulation(void)
{
	unsigned long i;

	//Since this restarts the search, we might as well reset the iteration counter
	ulIteration=0;

	//Also set the best fitness to a large negative value so that it will be overwritten immediately. Of
	//course, make sure that no fitness evaluations can result in fitnesses more negative than this
	dBestFitness=-1e+100;

	//Initialise the best and working chromosomes and the fitness statistics
	//Sets the genes of the ith chromosome to random values
	for (i=0;i<ulChromosomeLength;i++)
	{
		if(pnGeneTypes[i]==0)
		{
			//If this gene is binary set it to either 0 or 1
			pdGenes[i]=double(rand()%2);
		}
		else
		{
			//Otherwise, set it to a random value between 0 and 1
			pdGenes[i]=double(rand())/double(RAND_MAX);
		}
		pdBestChromosome[i]=pdGenes[i];
	}
}

void CPS::Mutate(void)
{
	unsigned long i;

	//Make sure the argument is valid
	assert(pdGenes);

	//For every gene in the chromosome
	for (i=0;i<ulChromosomeLength;i++)
	{
		//Is the gene binary?
		if(pnGeneTypes[i]==0)
		{
			//If so, mutate it with probability dMutationRate
			if(double(rand())/double(RAND_MAX)<dMutationRate)
			{
				pdGenes[i]=1.0-pdGenes[i];
			}
		}
		else
		{
			//Otherwise, perturb it with a random value between -dStepSize and +dStepSize
			pdGenes[i]=2.0*(double(rand())/double(RAND_MAX)-0.5)*dStepSize;
		}
	}
}

double *CPS::pdGetChromosomeForEvaluation(void)
{
	//This and SetFitness are the only functions that need to be called to make the search work.
	unsigned long i;

	//Prepare some storage for a copy of the new chromosome so that it can be returned to the calling
	//function and its fitness evaluated
	double *pdChromosomeForEvaluation=new double[ulChromosomeLength];
	for (i=0;i<ulChromosomeLength;i++)
	{
		//Copy the best chromosome into the working chromosome
		pdGenes[i]=pdBestChromosome[i];
	}

	//Mutate the working chromosome
	Mutate();

	for (i=0;i<ulChromosomeLength;i++)
	{
		//And make a copy of it so that its fitness can be evaluated
		pdChromosomeForEvaluation[i]=pdGenes[i];
	}

	//Return it
	return pdChromosomeForEvaluation;
}
	
void CPS::SetFitness(const double dNewFitness)
{
	//This is the only other function that needs to be called and is used to set the fitness of a chromosome
	//that has just been evaluated

	//The following line could be made stachastic (resulting in an algorithm similar to 
	//simulated annealing (SA). If the fitness is higher than anything yet achieved:
	if(dNewFitness>=dBestFitness)
	{
		//Record it
		dBestFitness=dNewFitness;

		//And keep a copy of the chromosome
		unsigned long i;
		for(i=0;i<ulChromosomeLength;i++)
		{
			pdBestChromosome[i]=pdGenes[i];
		}

		//Increase the scope of the search a little
		dStepSize*=1.2;
		dMutationRate*=1.2;

		//The largest meaningful value for the mutation probability is 0.5
		if(dMutationRate>0.5)
		{
			dMutationRate=0.5;
		}

		//Stop the mutation rate dropping too low. Because the search is mutation based,
		//it ends if the mutation rate drops too low, because the agent spends all its time
		//exploiting the best behaviour it has discovered and spends no time exploring 
		//alternatives
		if(dMutationRate<0.5/double(ulChromosomeLength))
		{
			dMutationRate=0.5/double(ulChromosomeLength);
		}
	}
	else
	{
		//If this behaviour was not at least as good as the best, reduce the breadth of the
		//search a little because the behaviours we're trying may be too different. The 
		//multipliers in these lines and those above control the rate of convergence of the 
		//algorithms and set the trade-off between exploration and exploitation. Setting the
		//constants below to be very small (which, in this case would mean 0.9, say) would
		//make the algorithm converge very rapidly and the agent would spend its time
		//exploiting a very poor behaviour because it hasn't adequately explored. Setting the 
		//constants above (which are 1.2 by default) to be too large (say, 10) would make 
		//the agent spent a lot of time exploring radical behaviours that are so different to
		//what actually wroks that its average performance would be very poor
		dStepSize*=0.99;
		dMutationRate*=0.99;
	}

	//Record another fitness evaluation
	ulIteration++;
}

double *CPS::pdGetBestChromosome(void)
{
	//Returns a pointer to the best chromosome discovered so far. Don't delete!
	return pdBestChromosome;
}

double CPS::dGetBestPerformance(void)
{
	//Return the best fitness achieved so far
	return dBestFitness;
}

int CPS::Save(const char * const psFilename)
{
	//Saves the status of the search
	ofstream *pOut=new ofstream(psFilename);
	unsigned long i,j;
	if(!pOut || !pOut->is_open())
	{
		return 0;
	}
	pOut->precision(18);
	*pOut<<ulChromosomeLength;
	*pOut<<"\n";
	*pOut<<dMutationRate;
	*pOut<<"\n";
	*pOut<<dStepSize;
	*pOut<<"\n";
	*pOut<<ulIteration;
	*pOut<<"\n";
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pOut<<pnGeneTypes[i];
		*pOut<<"\t";
	}
	*pOut<<"\n";
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pOut<<pdGenes[i];
		*pOut<<"\t";
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

int CPS::Load(const char * const psFilename)
{
	//And load it!
	ifstream *pIn=new ifstream(psFilename,ios::nocreate);
	unsigned long i,j;
	if(!pIn || !pIn->is_open())
	{
		return 0;
	}

	//Free up memory used by the algorithm as it is now
	DeallocateMemory();

	*pIn>>ulChromosomeLength;

	//Allocate new memory
	AllocateMemory();
	*pIn>>dMutationRate;
	*pIn>>dStepSize;
	*pIn>>ulIteration;
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pIn>>pnGeneTypes[i];
	}
	for (i=0;i<ulChromosomeLength;i++)
	{
		*pIn>>pdGenes[i];
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
