//GAPBILExample
//Copyright John Manslow
//29/09/2001

#ifndef _CPS_
#define _CPS_

class CPS
{
public:

	CPS(		
					const unsigned long,			//Chromosome length
					const int * const					//Gene type information
					);

	~CPS();

	//Creates a new chromosome by randomly mutating the best so far
	double *pdGetChromosomeForEvaluation(void);

	//Sets the fitness of the working chromosome. The PS works the same way as the GA 
	//and PBIL. Get a chromosome using the above function, evaluate its fitness, and feed
	//it back to the PS class using the function below
	void SetFitness(const double);

	//Returns a pointer to the best chromosome discovered so far
	double *pdGetBestChromosome(void);

	//Returns the performance of the best chromosome found so far
	double dGetBestPerformance(void);

	//Load and save the status of the PS
	int Save(const char*const);
	int Load(const char*const);

private:
	//These functions don't need to be called from outside the class to make the GA work
	void Mutate(void);

	//Allocate and deallocate memory
	void AllocateMemory(void);
	void DeallocateMemory(void);

	//Create a random starting chromosome and resets search statistics
	void InitialisePopulation(void);

	//Mutates the chromosome in the parameter list
	//void Mutate(double * const);

	//The mutation rate sets the probability with which binary genes are muated (flipped)
	double dMutationRate;

	//The step size controls the magnitude of mutations made to continuous genes
	double dStepSize;

	//Chromosome length
	unsigned long ulChromosomeLength;

	//Pointer to the working chromosome
	double *pdGenes;

	//Pointer to a list of gene type indicators: 0=gene is binary, 1=gene is real
	int *pnGeneTypes;

	//Storage for the fittest chromosome discovered so far and its fitness
	double *pdBestChromosome;
	double dBestFitness;

	//The number of iterations (i.e. number of fitness evaluations)
	unsigned long ulIteration;

};

#endif