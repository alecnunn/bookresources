//GAPBILExample
//Copyright John Manslow
//29/09/2001

#ifndef _CPBIL_
#define _CPBIL_

class CPBIL
{
public:

	CPBIL(
							const unsigned long,			//Population size
							const unsigned long				//Chromosome length
				);

	~CPBIL();

	//One of the two functions required to make the PBIL work.Calling this function returns one member of
	//the population to allow its fintess to be evaluated
	double *pdGetChromosomeForEvaluation(void);

	//Called after the fitness evaluation to update the fitness statistics of teh population 
	void SetFitness(const double);

	//Load and save the status of the PBIL
	int Save(const char*const);
	int Load(const char*const);

	//Returns a pointer to the best performing chromosome found so far
	double *pdGetBestChromosome(void);

	//Returns the performance of the best chromosome found so far
	double dGetBestPerformance(void);

private:

	//Creates a population by sampling from the gene probabilities
	void CreatePopulation(void);

	//Allocates and deallocates memory to store the population, fitness statisticas, etc.
	void AllocateMemory(void);
	void DeallocateMemory(void);

	//Mutates the chromosome probability vector
	void Mutate(void);

	//The per-gene mutation probability
	double dMutationRate;

	//Population size and chromosome length
	unsigned long ulPopulationSize;
	unsigned long ulChromosomeLength;

	//Points to the population
	double **ppdGenes;

	//Points to the fitness statistics
	double *pdFitnesses;

	//Points to the vector of gene probabilities from which the population is sampled
	double *pdGeneProbabilities;

	//Controls the rate at which the PBIL learns
	double dAdaptationRate;

	//The fitness of the best chromosome found so far and the chromosome itself
	double dBestFitness;
	double *pdBestChromosome;

	//The position of the chromosome currently being evaluated in the population
	unsigned long ulWorkingChromosome;

	//The number of fitness evaluations so far
	unsigned long ulIteration;

};

#endif