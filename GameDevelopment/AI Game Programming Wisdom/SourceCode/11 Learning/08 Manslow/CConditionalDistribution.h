//Tanks
//Copyright John Manslow
//29/09/2001
#ifndef _CConditionalDistribution_
#define _CConditionalDistribution_

class CConditionalDistribution
{
public:
	CConditionalDistribution(
		const unsigned long,		//Number of inputs
		const unsigned long,		//Number of hidden neurons
		const unsigned long			//Number of bins
		);

	~CConditionalDistribution();

	//Used to reset the perturbation search and set the 
	//weights in the neural network component to random values
	void Reset(void);				

	//This function must be called repeatedly (perhaps many thousands of times) in order to accurately model
	//the distribution that describes the exemplar data. The function is passed the quantity and location of the 
	//exemplar data and returns a measure of the network's current performance
	double dTrainingStep(
		const unsigned long,		//The number of samples in the exemplar data
		double ** const,			//A pointer to the array of input samples
		double * const				//A pointer to the array of target outputs
		);

	//Returns a single random sample from the distribution computed in response to the inputs provided
	double dGetOutputs(
		const double * const		//A pointer to an array of inputs
		);

	//Returns a vector of probabiltiies that are to be associated with the distribution's bins given the input provided
	double *pdGetBinProbabilities(
		const double * const		//A pointer to an array of inputs
		);

	//Returns the performance of the network on the exemplar data.
	double dGetPerformance(void);

	//Computes the performance of the network on the data set indicated in the function's
	//parameters. Used internally. Should only be called from outside the class if
	//the network's performance on a data set different to that passed to dTrainingStep is
	//required.
	double dGetPerformance(
		const unsigned long,		//The number of samples in the data set
		double** const,				//A pointer to the array of inputs
		double* const				//A pointer to the array of target outputs
		);

	//Functions used to allocate and deallocate memory
	void AllocateMemory(void);		
	void DeallocateMemory(void);

	int Save(const char * const);	//Used to load and save the model
	int Load(const char * const);

//A couple of the member variables are accessed from outside the class to aid visualisation 
//private:
	//These variables contain information about the model's structure
	unsigned long ulNumberOfInputs;
	unsigned long ulNumberOfHiddenNodes;
	unsigned long ulNumberOfOutputs;				//(Note, ulNumberOfOutputs = number of bins in the model)

	//This is the step size currently used in the perturbation search
	double dStepSize;

	//The performance of the model measured during the dTrainingStep function
	double dBestError;

	//Pointers to the model's weights
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

