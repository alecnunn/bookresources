//GAPBILExample
//Copyright John Manslow
//29/09/2001

#ifndef _CGA_
#define _CGA_

class CGA
{
public:

	CGA(		
					const unsigned long,			//Population size
					const unsigned long,			//Chromosome length
					const int * const					//Gene type information
					);

	~CGA();

	//Calls the Mate() function (see below) to create a new chromosome and returns a pointer to it so
	//that its fitness can be evaluated
	double *pdGetChromosomeForEvaluation(void);

	//Sets the fitness of the working chromosome. All that is necessary to use the GA is to call the above
	//function to create a new chromosome, evaluate its fitness, and pass the fitness back to the GA 
	//using this function
	void SetFitness(const double);

	//Returns a pointer to the best chromosome discovered so far
	double *pdGetBestChromosome(void);

	//Returns the performance of the best chromosome found so far
	double dGetBestPerformance(void);

	//Load and save the status of the GA
	int Save(const char*const);
	int Load(const char*const);

private:
	//These functions don't need to be called from outside the class to make the GA work

	//Allocate and deallocate memory
	void AllocateMemory(void);
	void DeallocateMemory(void);

	//Create a random population and resets fintess statistics
	void InitialisePopulation(void);

	//Mutates the chromosome pointed to by the pointer passed in the parameter list
	void Mutate(double * const);

	//Creates a child of the paremts pointed to by the pointers in the parameter list and places
	//it in the population at the location indicated by the last parameter
	void Crossover(const double * const, const double * const,const unsigned long);

	//Chooses two parents, crosses them, replaces the less fit of the two with the child, and
	//mutates the child
	void Mate(void);

	//Per-gene mutation and crossover probabilities
	double dMutationRate;
	double dCrossoverRate;

	//Population size and chromosome length
	unsigned long ulPopulationSize;
	unsigned long ulChromosomeLength;

	//Pointer to the actual population of genes
	double **ppdGenes;

	//Pointer to the list of fitnesses
	double *pdFitnesses;

	//Pointer to a list of gene type indicators: 0=gene is binary, 1=gene is real
	int *pnGeneTypes;

	//Storage for the fittest chromosome discovered so far and its fitness
	double dBestFitness;
	double *pdBestChromosome;

	//The position of the current working chromosome in the population. The working chromosome is the
	//one that is currently having its fitness evaluated
	unsigned long ulWorkingChromosome;

	//The number of iterations (i.e. number of fitness evaluations)
	unsigned long ulIteration;

};

#endif