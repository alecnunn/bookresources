//						Tanks
//			(c) Copyright John Manslow
//					  18/04/2001

////////////////////////////////////////////////////
//Remove these includes if not compiling under windows
#include "stdafx.h"
#define false FALSE
////////////////////////////////////////////////////
#include "CMLP.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "assert.h"
#include "fstream.h"
#include "time.h"

//Where backups will be made during training (guards against crashes, etc.)
#define FileForBackupSaves "TrainingBackup.mlp"

CMLP::CMLP(
			const unsigned long ulNewNumberOfInputs,
			const unsigned long ulNewNumberOfHiddenNodes,
			const unsigned long ulNewNumberOfOutputs
			)
{
	//Record the structure (number of inputs, hidden neurons, and outputs) of the network
	ulNumberOfInputs=ulNewNumberOfInputs;
	ulNumberOfHiddenNodes=ulNewNumberOfHiddenNodes;
	ulNumberOfOutputs=ulNewNumberOfOutputs;

	//Allocate memory to store the network's "current" weights and the best weights.
	AllocateMemory();

	//Set the network's weights to random values and reset all variables used in training.
	Reset();

	//Set these character pointers to NULL so we know they're not used yet
	pTrainingStartTime=NULL;
	pTrainingStartDate=NULL;
}

void CMLP::AllocateMemory(void)
{
	unsigned long i;
	//Allocate memory to store the current values of the input to hidden layer weights and
	//their best values
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

void CMLP::DeallocateMemory(void)
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

CMLP::~CMLP()
{
	DeallocateMemory();
}

void CMLP::Reset(void)
{
	unsigned long i,j;

	//Give the network weights random values between -1 and +1. Since this effectively resets 
	//training the best recorded weights (stored in ppdBest...) are set to the new random values.
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

//This is the function that allows the network to learn. It uses a perturbation search
//to find values of the network's weights that allow it to reproduce the set of example 
//input-output pairs to the desired accuracy. Each call to this function improves the network
//only very slightly, so this function will often have to be called many hundreds of thousands
//of times before the network is good enough.
double CMLP::dTrainingStep(
							const unsigned long ulNumberOfPatternsInTrainingSet,
							double ** const ppdTrainingInputs,
							double ** const ppdTrainingTargets
							)
{
	//This function performs one step of a perturbation search: it randomly perturbs the
	//neural network's weights. If the network's performance has improved, the perturbed
	//values are kept. If not, the old values are restored.
	unsigned long i,j;
	double dNewError;

	//If dBestError=-1, this is the first training step so we need to do some initialisation
	//of the perturbation search.
	if(dBestError==-1.0)
	{
		//Make sure we deallocate memory pointer to be these to pointers (if any) before we 
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

		//Measure the performance of the network with the weights set to their current values.
		//Since this is the only performance measurement so far, it must be the best. Store it.
		dBestError=dGetPerformance(
									ulNumberOfPatternsInTrainingSet,
									ppdTrainingInputs,
									ppdTrainingTargets
									);
	}

	//Perturb the network's weights by adding a random value between +dStepSize and -StepSize
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

	//Measure the performance of the network with the new weights
	dNewError=dGetPerformance(
								ulNumberOfPatternsInTrainingSet,
								ppdTrainingInputs,
								ppdTrainingTargets
								);

	//If the performance is worse (the new error is larger)
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
		//performance of the network with the new weights,
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

		//Save the network just in case we have a crash (or power failure or something)
		Save(FileForBackupSaves);
	}

	//Tell the calling function what the performance of the network currently is, so it can 
	//decide whether to continue training. This function always leaves the network with the 
	//best weights found so far, so there's no need to restore them externally.
	return dBestError;
}

double *CMLP::pdGetOutputs(
							const double * const pdInputs
							)
{

	unsigned long i,j;
	
	//Declare storage for the activities of the hidden and output neurons
	double *pdah=new double[ulNumberOfHiddenNodes];
	double *pdao=new double[ulNumberOfOutputs];

	//Declare storage for the amount of stimulation coming onto a neuron
	double dStimulus;

	//Calculate the activity of the network's hidden neurons.
	for(i=0;i<ulNumberOfHiddenNodes;i++)
	{
		//Each hidden neuron receives internal stimulation:
		dStimulus=ppdwih[i][0];
		//And stimulation from input neurons (the activites of which are just the inputs to the 
		//network, pdInputs) via the weights connecting it to each input (ppdwih). Remember
		//that pdInputs contains scaled versions of x-displacement, y-displacement and wind 
		//speed.
		for(j=1;j<ulNumberOfInputs+1;j++)
		{
			dStimulus+=ppdwih[i][j]*pdInputs[j-1];
		}
		//The stimulation that a hidden neuron receives is translated into its level of activity
		//by an "activation function":
		pdah[i]=1.0/(1.0+exp(-dStimulus));
		//The logistic function (used in the line above) is by far the most common, though almost
		//any function can be used. In fact, each hidden neuron can use a different function (though
		//such a network can no longer be considered neural). Of course, the weights learnt during
		//training are specific to the types of activation function used, so weights learnt
		//by a network with logistic activation functions won't work well in one with sine functions,
		//for example.

	}
	//The activity of the output neuron of the network is computed in
	//essentially the same way as the activities of the hidden neurons, except that the
	//output receives stimulation from the hidden neurons (pdah) via the hidden to output
	//layer weights (ppdwho). Note that a network may have several outputs but this application
	//requires only one - to represent the angle of the tank's barrel.
	for(i=0;i<ulNumberOfOutputs;i++)
	{
		//Account for the neuron's internal stimulation.
		dStimulus=ppdwho[i][0];
		for(j=1;j<ulNumberOfHiddenNodes+1;j++)
		{
			//Account for the stimulation it receives from the hidden neurons
			dStimulus+=ppdwho[i][j]*pdah[j-1];
		}
		//Translate this stimulation into the activity of the output neuron using the activation 
		//function:
		pdao[i]=dStimulus;
		//In this case, the activation function of the output neuron is just the identity:
		//the output neuron's activity equals the amount of stimulation it receives. This 
		//is the function normally used when a network is estimating a continuous variable 
		//(like the angle of a tank's barrel). As with hidden neurons, other activation 
		//functions can be used, though care should be taken to make sure that it is 
		//possible for the network output to reach the desired value. e.g. the angle of 
		//the AI tank's barrel that hits the player is always negative, so there'd be no 
		//point in using the logistic activation function for the network outputs (because
		//the output of the logistic function is always positive).
	}
	//Deallocate the temporary storage that was used for the hidden neuron activities
	delete []pdah;

	//Remember that we're returning a pointer to "new"ed storage, so the calling function
	//must deallocate it to avoid memory leaks.
	return pdao;
}

double CMLP::dGetPerformance(void)
{
	//dBestError is computed in dTrainingStep and indicates the current performance of the
	//network on the exemplar data. The function below need only be called from outside the class
	//if the network's performance on some other data needs to be measured.
	return dBestError;
}

double CMLP::dGetPerformance(
								const unsigned long ulNumberOfPatterns,
								double ** const ppdInputs,
								double ** const ppdTargets
								)
{
	double dError=0.0;
	double *pdOutputs;
	unsigned long i,j;

	//Go through each pattern (example) in the data set and
	for(i=0;i<ulNumberOfPatterns;i++)
	{
		//Compute the output (estimated barrel angle) produced by the network in response 
		//to its inputs (x-displacement, y-displacement and wind speed)
		pdOutputs=pdGetOutputs(ppdInputs[i]);
		for(j=0;j<ulNumberOfOutputs;j++)
		{
			//Compute the squared error between the output produced by the network
			//(the barrel angle it estimated as being correct) and the target output
			//contained in the sample (the barrel angle that actually scored the hit)
			dError+=0.5*pow(fabs(ppdTargets[i][j]-pdOutputs[j]),2.0);
			//Again, multiple outputs are supported, but only one is used in this
			//application. Occasionally, different error measures are employed. Using a number
			//greater than 2.0 in the above equation tends to result in much less variation
			//in the performance of the network accross samples in the data set, but slows 
			//training down, whereas values less than 2.0 speed training up but allow the
			//network to show greater variation in performance accross the data set - almost 
			//ignoring some of the more difficult to learn examples. A value of 2.0 should be
			//sufficient for virtually all applications. Note that if a value other than 2.0 
			//is used, the termination error specified in TanksDoc must be revised accordingly.
		}
		//Deallocate the memory used to store the network outputs.
		delete []pdOutputs;
	}

	//Divide the error by the number of patterns to give the average error per sample - makes
	//the error measure independent of the number of samples and hence a little more 
	//interpretable.
	dError/=double(ulNumberOfPatterns);

	//Return the computed error
	return dError;
}

int CMLP::Save(const char * const pFileName)
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

int CMLP::Load(const char * const pFileName)
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