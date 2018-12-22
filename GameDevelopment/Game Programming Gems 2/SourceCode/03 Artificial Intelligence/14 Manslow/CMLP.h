//						Tanks
//			(c) Copyright John Manslow
//					  18/04/2001

#ifndef _CMLP_
#define _CMLP_

class CMLP
{
public:
	CMLP(
		const unsigned long,		//Number of inputs
		const unsigned long,		//Number of hidden neurons
		const unsigned long			//Number of outputs
		);

	~CMLP();

	void Reset(void);				//Used to reset the perturbation search and set the 
									//network's weights to random values

		//This function performs one step in the perturbation search that allows the network
		//to learn. The function is passed the quantity and location of the exemplar data
		//and returns a measure of the network's current performance
	double dTrainingStep(
		const unsigned long,		//The number of samples in the exemplar data
		double ** const,			//A pointer to the array of input samples
		double ** const				//A pointer to the array of target outputs
		);

		//Returns a pointer to an array containing the network outputs computed in response
		//to the inputs provided
	double *pdGetOutputs(
		const double * const		//A pointer to an array of inputs
		);

		//Returns the current performance of the network on the exemplar data. Is computed
		//in dTrainingStep
	double dGetPerformance(void);

		//Computes the performance of the network on the data set indicated in the function's
		//parameters. Used internally. Should only be called from outside the class if
		//the network's performance on a data set different to that passed to dTrainingStep is
		//required.
	double dGetPerformance(
		const unsigned long,		//The number of samples in the data set
		double** const,				//A pointer to the array of inputs
		double** const				//A pointer to the array of target outputs
		);

	void AllocateMemory(void);		//Functions used to allocate and deallocate memory
	void DeallocateMemory(void);	//for the network structure

	int Save(const char * const);	//Used to load and save the network
	int Load(const char * const);

private:
		//These variables contain information about the network's structure
	unsigned long ulNumberOfInputs;
	unsigned long ulNumberOfHiddenNodes;
	unsigned long ulNumberOfOutputs;

		//This is the step size currently used in the perturbation search
	double dStepSize;

		//The performance of the network measured during the dTrainingStep function
	double dBestError;

		//Pointers to the network's weights
	double **ppdwih;
	double **ppdwho;

		//Pointers to the best weights found during learning
	double **ppdBestwih;
	double **ppdBestwho;

		//Used to store the time and date when training started
	char *pTrainingStartTime;
	char *pTrainingStartDate;
};

#endif

