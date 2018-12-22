//Tanks
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////
//Remove this include if not compiling under windows
#include "stdafx.h"
#define new DEBUG_NEW
#define false FALSE
////////////////////////////////////////////////////
#include "CConditionalDistribution.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "assert.h"
#include "fstream.h"
#include "time.h"

//Where backups will be made during training (guards against crashes, fires, etc.)
#define FileForBackupSaves "ErrorTrainingBackup.cdm"

CConditionalDistribution::CConditionalDistribution(
			const unsigned long ulNewNumberOfInputs,
			const unsigned long ulNewNumberOfHiddenNodes,
			const unsigned long ulNewNumberOfOutputs
			)
{
	TRACE("\t\tCreating conditional distribution...");

	//Record the structure (number of inputs, hidden neurons, and bins) of the model
	ulNumberOfInputs=ulNewNumberOfInputs;
	ulNumberOfHiddenNodes=ulNewNumberOfHiddenNodes;
	ulNumberOfOutputs=ulNewNumberOfOutputs;

	//Allocate memory to store the model's structure
	AllocateMemory();

	//Reset the learning procedure
	Reset();

	//Set these character pointers to NULL so we know they're not used yet
	pTrainingStartTime=NULL;
	pTrainingStartDate=NULL;

	TRACE("successful.\n");
}

void CConditionalDistribution::AllocateMemory(void)
{
	unsigned long i;

	//Allocate memory to store the current values of the input to hidden layer weights 
	//of the neural network component of the model and their best values
	ppdwih=new double*[ulNumberOfHiddenNodes];
	ppdBestwih=new double*[ulNumberOfHiddenNodes];
	assert(ppdwih && ppdBestwih);
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		ppdwih[i]=new double[ulNumberOfInputs+1];
		ppdBestwih[i]=new double[ulNumberOfInputs+1];
		assert(ppdwih[i] && ppdBestwih[i]);
	}

	//Do the same for the hidden to output layer weights
	ppdwho=new double*[ulNumberOfOutputs];
	ppdBestwho=new double*[ulNumberOfOutputs];
	assert(ppdwho && ppdBestwho);
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		ppdwho[i]=new double[ulNumberOfHiddenNodes+1];
		ppdBestwho[i]=new double[ulNumberOfHiddenNodes+1];
		assert(ppdwho[i] && ppdBestwho[i]);
	}
}

void CConditionalDistribution::DeallocateMemory(void)
{
	//Deallocate the storage used for current and best weight values.
	unsigned long i;
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		delete []ppdwih[i];
		delete []ppdBestwih[i];
	}
	delete []ppdwih;
	delete []ppdBestwih;
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		delete []ppdwho[i];
		delete []ppdBestwho[i];
	}
	delete []ppdwho;
	delete []ppdBestwho;

	//If we've recorded the time and date of the start of training, delete them too.
	if(pTrainingStartTime)
	{
		delete []pTrainingStartTime;
	}
	if(pTrainingStartDate)
	{
		delete []pTrainingStartDate;
	}
}

CConditionalDistribution::~CConditionalDistribution()
{
	TRACE("\t\tDestroying conditional distribution...");
	DeallocateMemory();
	TRACE("successful.\n");
}

void CConditionalDistribution::Reset(void)
{
	unsigned long i,j;

	//Give the neural network component of the model weights random values between -1 and +1.
	//Since this effectively resets training the best recorded weights (stored in ppdBest...) are set 
	//to the new random values.
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		for(j=0;j<ulNumberOfInputs+1;j++)
		{
			ppdwih[i][j]=2.0*(double(rand())/double(RAND_MAX)-0.5);
			ppdBestwih[i][j]=ppdwih[i][j];
		}
	}

	//Do the same for the hidden to output layer weights
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		for(j=0;j<ulNumberOfHiddenNodes+1;j++)
		{
			ppdwho[i][j]=2.0*(double(rand())/double(RAND_MAX)-0.5);
			ppdBestwho[i][j]=ppdwho[i][j];
		}
	}

	//Reset the best recorded error to an impossible value (error is always positive) to indicate
	//that no training has taken place with the current values of the weights.
	dBestError=-1.0;

	//Reset the step size to a conservative value.
	dStepSize=0.001;
}

//This function trains the conditional distribution model by using a perturbation search (as discussed in the
//book Game Programming Gems 2) to search for weights its neural network component 
//that are most consistent with the exemplar data passed to this function 
double CConditionalDistribution::dTrainingStep(
							const unsigned long ulNumberOfPatternsInTrainingSet,
							double ** const ppdTrainingInputs,
							double * const ppdTrainingTargets
							)
{
	unsigned long i,j;
	double dNewError;

	//If dBestError=-1, this is the first training step so we need to do some initialisation
	if(dBestError==-1.0)
	{
		//Make sure we deallocate memory pointed to by these pointers (if any) before we 
		//reassign them
		if(pTrainingStartTime)
		{
			delete []pTrainingStartTime;
		}
		if(pTrainingStartDate)
		{
			delete []pTrainingStartDate;
		}

		//Record time and date that training started.
		pTrainingStartTime=new char[256];
		_strtime(pTrainingStartTime);
		pTrainingStartDate=new char[256];
		_strdate(pTrainingStartDate);

		//Measure the performance of the model with the weights set to their current values.
		//Since this is the only performance measurement so far, it must be the best. Store it.
		dBestError=dGetPerformance(
									ulNumberOfPatternsInTrainingSet,
									ppdTrainingInputs,
									ppdTrainingTargets
									);
	}

	//Perturb the model's weights by adding a random value between +dStepSize and -StepSize
	//to each one.
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		for(j=0;j<ulNumberOfInputs+1;j++)
		{
			ppdwih[i][j]+=dStepSize*(double(rand())/double(RAND_MAX)-0.5)*2.0;
		}
	}

	//And for the hidden to output layer weights
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		for(j=0;j<ulNumberOfHiddenNodes+1;j++)
		{
			ppdwho[i][j]+=dStepSize*(double(rand())/double(RAND_MAX)-0.5)*2.0;
		}
	}

	//Measure the performance of the model with its new weights
	dNewError=dGetPerformance(
								ulNumberOfPatternsInTrainingSet,
								ppdTrainingInputs,
								ppdTrainingTargets
								);

	//If its performance has deteriorated (the new error is larger)
	if(dNewError>dBestError)
	{
		//Reduce the size of the perturbation a bit - we need to be more conservative!
		dStepSize*=0.9;

		//Restore the weights back to their old values
		for(i=0;i<ulNumberOfHiddenNodes;i++)
		{
			for(j=0;j<ulNumberOfInputs+1;j++)
			{
				ppdwih[i][j]=ppdBestwih[i][j];
			}
		}
		for(i=0;i<ulNumberOfOutputs;i++)
		{
			for(j=0;j<ulNumberOfHiddenNodes+1;j++)
			{
				ppdwho[i][j]=ppdBestwho[i][j];
			}
		}
	}
	else
	{
		//Otherwise the new weights performed at least as well as the old ones, so record the
		//performance of the model with its new weights,
		dBestError=dNewError;

		//Increase the step size a little - we're doing well and can afford to be more 
		//adventurous.
		dStepSize*=1.2;

		//Record the new weights as the best so far discovered
		for(i=0;i<ulNumberOfHiddenNodes;i++)
		{
			for(j=0;j<ulNumberOfInputs+1;j++)
			{
				ppdBestwih[i][j]=ppdwih[i][j];
			}
		}
		for(i=0;i<ulNumberOfOutputs;i++)
		{
			for(j=0;j<ulNumberOfHiddenNodes+1;j++)
			{
				ppdBestwho[i][j]=ppdwho[i][j];
			}
		}

		//Save the model just in case we have a crash (or power failure or something)
		Save(FileForBackupSaves);
	}

	//Tell the calling function what the performance of the model currently is, so it can 
	//decide whether to continue training. This function always leaves the model with the 
	//best weights found so far, so there's no need to restore them externally at the end of 
	//training
	return dBestError;
}

//This function uses the neural network component of the conditional distribution model to calculate
//the probability associated with each bin in the distribution. This is done simply by querying the neural
//network with the distribution inputs because it has one output for each bin
double *CConditionalDistribution::pdGetBinProbabilities(
							const double * const pdInputs
							)
{
	//For detailed comments on querying a neural network, see CMLP.cpp
	unsigned long i,j;

	//Declare storage for the activities of the hidden and output neurons
	double *pdah=new double[ulNumberOfHiddenNodes];
	double *pdao=new double[ulNumberOfOutputs];

	//Declare storage for the amount of stimulation coming onto a neuron
	double dStimulus;

	//Calculate the activity of the network's hidden neurons.
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		dStimulus=ppdwih[i][0];
		for(j=1;j<ulNumberOfInputs+1;j++)
		{
			dStimulus+=ppdwih[i][j]*pdInputs[j-1];
		}
		pdah[i]=1.0/(1.0+exp(-dStimulus));
	}
	double dTotalOutput=0.0;

	//Unlike the neural network in CMLP.cpp, the one used here has a special function to calculate the
	//activity of its outputs in order to guarantee that they sum to unity (as befits closed-world probabilities). 
	//This function is called "softmax" and is described more fully in books such as [Bishop95]
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		dStimulus=ppdwho[i][0];
		for(j=1;j<ulNumberOfHiddenNodes+1;j++)
		{
			dStimulus+=ppdwho[i][j]*pdah[j-1];
		}

		//Translate this stimulation into the activity of the output neuron using the activation 
		//function:
		pdao[i]=exp(dStimulus);
		dTotalOutput+=pdao[i];
	}

	//Deallocate the temporary storage that was used for the hidden neuron activities
	delete []pdah;

	//Normalise the output neuron activities
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		pdao[i]/=dTotalOutput;
	}

	//Remember that we're returning a pointer to "new"ed storage, so the calling function
	//must deallocate it to avoid memory leaks.
	return pdao;
}

//Generates a sample from the conditional distribution by selecting one of the distribution's bins in 
//accordance with their probabilities and generating a specific sample from within the bin
double CConditionalDistribution::dGetOutputs(
							const double * const pdInputs
							)
{
	//Use the neural network to calculate the bin probabilities as a function of the conditional distribution's inputs
	double *pdOutputs;
	pdOutputs=pdGetBinProbabilities(pdInputs);

	unsigned long ulCurrentBin;
	double dOutput;
	//This outer loop guarantees that events less likely than 1 in 100 do not occur. Though not strictly necessary
	//it helps to avoid samples that cannot be justified statistically on the basis of the model's training data.
	do
	{
		//The sampling mechanism in this inner loop is identical to that of the unconditional distribution
		ulCurrentBin=0;
		double dTargetProbability=double(rand())/double(RAND_MAX);
		double dAccumulator=pdOutputs[ulCurrentBin];
		while(dTargetProbability>dAccumulator)
		{
			ulCurrentBin++;
			dAccumulator+=pdOutputs[ulCurrentBin];
		}
	}
	while(pdOutputs[ulCurrentBin]<0.01);

	dOutput=double(ulCurrentBin)/double(ulNumberOfOutputs)+double(rand())/double(RAND_MAX)*(1.0/double(ulNumberOfOutputs));

	delete []pdOutputs;

	return dOutput;
}

double CConditionalDistribution::dGetPerformance(void)
{
	//dBestError is computed by a call to dGetPerformance
	return dBestError;
}

//This function measures the performance of the conditional distribution model - that is how consistent it is with 
//the exemplar data passed to this function. This consistency is measured as the mean log-probability with which 
//the distribution model "explains" the data.
double CConditionalDistribution::dGetPerformance(
								const unsigned long ulNumberOfPatterns,
								double ** const ppdInputs,
								double * const pdTargets
								)
{
	unsigned long i,j;

	//Storage for the error measure
	double dError=0.0;

	//Will store the neural network outputs (i.e. bin probabilities)
	double *pdOutputs;

	//Prepare some storage for the binary target vector (explained below)
	double *pdBinaryTargets=new double[ulNumberOfOutputs];
	
	//Go through each pattern (example) in the data set and
	for(i=0;i<ulNumberOfPatterns;i++)
	{
		//Compute the probabilities associated with each bin in the distribution for the current input
		pdOutputs=pdGetBinProbabilities(ppdInputs[i]);

		//Now, for each bin
		for(j=0;j<ulNumberOfOutputs;j++)
		{
			//This section of code translates the target output associated with the current example into a 
			//vector of length ulNumberOfOutputs, containing zeros in all positions but for a one corresponding to
			//the bin that actually contains target output specified in the current example. 

			//This vector can be thought of as a vector of target probabiltiies for the bins in the distribution model
			//and hence for the outputs of teh neural network: if the distribution predicted the examples perfectly 
			//then, for this pattern, all bins would have probability zero except for the one that actually contained
			//the example output.
			if(pdTargets[i]>=double(j)/double(ulNumberOfOutputs) 
				&& pdTargets[i]<double(j+1)/double(ulNumberOfOutputs))
			{
				pdBinaryTargets[j]=1.0;
			}
			else
			{
				pdBinaryTargets[j]=0.0;
			}

			//The actual error measure is derived below. This is the cross entropy function and can only be used
			//when the bin probabilities are constrained to sum to, at most, unity. (See [Bishop95].)
			dError-=pdBinaryTargets[j]*log(pdOutputs[j]);
		}
		//Deallocate the memory used to store the network outputs.
		delete []pdOutputs;
	}

	//Divide the error by the number of patterns to give the average error per sample - makes
	//the error measure independent of the number of samples and hence a little more 
	//interpretable.
	dError/=double(ulNumberOfPatterns);

	//Clean up allocated memory
	delete []pdBinaryTargets;

	//Return the computed error
	return dError;
}

int CConditionalDistribution::Save(const char * const pFileName)
{
	unsigned long i,j;
	assert(pFileName);

	//Create the output stream to save the network to.
	ofstream *pOut=new ofstream(pFileName);

	//Make sure it was created and opened successfully.
	if(!pOut)
	{
		assert(false);
		return 0;
	}
	if(!pOut->is_open())
	{
		assert(false);
		delete pOut;
		return 0;
	}
	//Make sure we don't lose information.
	pOut->precision(15);

	//Save all the network info:
	//Its structure...
	*pOut<<ulNumberOfInputs;
	*pOut<<"\n";
	*pOut<<ulNumberOfHiddenNodes;
	*pOut<<"\n";
	*pOut<<ulNumberOfOutputs;
	*pOut<<"\n";

	//Its weights
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		for(j=0;j<ulNumberOfInputs+1;j++)
		{
			*pOut<<ppdwih[i][j];
			*pOut<<"\t";
		}
		*pOut<<"\n";
	}
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		for(j=0;j<ulNumberOfHiddenNodes+1;j++)
		{
			*pOut<<ppdwho[i][j];
			*pOut<<"\t";
		}
		*pOut<<"\n";
	}

	//When training started...
	*pOut<<"Training started\t";
	*pOut<<pTrainingStartTime;
	*pOut<<"\t";
	*pOut<<pTrainingStartDate;
	*pOut<<"\n";

	//the current date and time...
	char *pTime=new char[256];
	*pOut<<"Current time\t\t";
	_strtime(pTime);
	*pOut<<pTime;
	*pOut<<"\t";
	_strdate(pTime);
	*pOut<<pTime;
	*pOut<<"\n";
	delete []pTime;

	//And how well the network currently performs.
	*pOut<<"Performance\t\t";
	*pOut<<dBestError;
	*pOut<<"\n";

	//Close the file and delete the stream.
	pOut->close();
	delete pOut;

	//Return that the save was successful.
	return 1;
}

int CConditionalDistribution::Load(const char * const pFileName)
{
	unsigned long i,j;
	assert(pFileName);

	//Create a stream to load the network from
	ifstream *pIn=new ifstream(pFileName,ios::nocreate);

	//Check to make sure that it was created and could be opened.
	if(!pIn)
	{
		assert(false);
		return 0;
	}
	if(!pIn->is_open())
	{
		assert(false);
		delete pIn;
		return 0;
	}

	//Since we're about to load a new network, we should delete the storage used by the
	//current one to prevent memory leaks.
	DeallocateMemory();

	//Load the structure of the new network
	*pIn>>ulNumberOfInputs;
	*pIn>>ulNumberOfHiddenNodes;
	*pIn>>ulNumberOfOutputs;

	//Allocate memory to store its weights
	AllocateMemory();

	//Reset its status so that it can be trained if necessary
	Reset();

	//Load in its weights
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		for(j=0;j<ulNumberOfInputs+1;j++)
		{
			*pIn>>ppdwih[i][j];
			ppdBestwih[i][j]=ppdwih[i][j];
		}
	}
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		for(j=0;j<ulNumberOfHiddenNodes+1;j++)
		{
			*pIn>>ppdwho[i][j];
			ppdBestwho[i][j]=ppdwho[i][j];
		}
	}

	//Close and delete the stream.
	pIn->close();
	delete pIn;

	//Indicate that we've been successful.
	return 1;
}
