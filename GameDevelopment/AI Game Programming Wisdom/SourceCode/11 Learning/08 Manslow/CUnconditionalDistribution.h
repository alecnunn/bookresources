//Tanks
//Copyright John Manslow
//29/09/2001

#ifndef _CUnconditionalDistribution_
#define _CUnconditionalDistribution_

class CUnconditionalDistribution
{
public:
	CUnconditionalDistribution(
							const unsigned long		//Number of bins
							);

	~CUnconditionalDistribution();

	//Called to train the unconditional distribution model. Only needs to be called once because the
	//training procedure is so simple
	void dTrainingStep(			
												const unsigned long,		//Number of training examples
												const double * const		//Pointer to the examples
										);

	//Returns a sample from the distribution
	double dGetOutputs(void);

	//Resets the distribution to be uniform and resets the number of "stored" examples to zero
	void Reset(void);

	//Allocate and deallocate memory
	void AllocateMemory(void);		
	void DeallocateMemory(void);

	//These variables contain information about the network's structure
	unsigned long ulNumberOfDataPoints;
	unsigned long ulNumberOfBins;

	//The performance of the network measured during the dTrainingStep function
	double dBestError;

	//Pointers to the probabilities of each of the model's bins
	double *pdBinProbabilities;

	//Used to store the time and date when training started
	char *pTrainingStartTime;
	char *pTrainingStartDate;
};

#endif

