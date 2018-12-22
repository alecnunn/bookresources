//Tanks
//Copyright John Manslow
//29/09/2001

////////////////////////////////////////////////////
//Remove this include if not compiling under windows
#include "stdafx.h"
#define new DEBUG_NEW
////////////////////////////////////////////////////
#include "CUnconditionalDistribution.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "assert.h"
#include "fstream.h"
#include "time.h"

CUnconditionalDistribution::CUnconditionalDistribution(
															const unsigned long ulNewNumberOfBins
																						)
{
	TRACE("\t\tCreating unconditional distribution...");

	//Record the structure of the model (the number of bins it uses)
	assert(!(ulNewNumberOfBins<1));
	ulNumberOfBins=ulNewNumberOfBins;

	//Allocate memory to store the bin probabilities
	AllocateMemory();

	//Reset the probabilities to default values
	Reset();

	//Set these character pointers to NULL so we know they're not used yet
	pTrainingStartTime=NULL;
	pTrainingStartDate=NULL;

	TRACE("successful.\n");
}

void CUnconditionalDistribution::AllocateMemory(void)
{
	//Allocate memory to store the bin probabilities
	pdBinProbabilities=new double[ulNumberOfBins];
}

void CUnconditionalDistribution::DeallocateMemory(void)
{
	//Deallocate the storage used for the bin probabilities
	delete []pdBinProbabilities;

	//If we've recorded the time and date of the start of training, delete them too
	if(pTrainingStartTime)
	{
		delete []pTrainingStartTime;
	}
	if(pTrainingStartDate)
	{
		delete []pTrainingStartDate;
	}
}

CUnconditionalDistribution::~CUnconditionalDistribution()
{
	TRACE("\t\tDestroying unconditional distribution...");
	DeallocateMemory();
	TRACE("successful.\n");
}

void CUnconditionalDistribution::Reset(void)
{
	unsigned long i;

	//Initialise the bin probabilities to be equal (and sum to unity)
	for(i=0;i<ulNumberOfBins;i++)
	{
		pdBinProbabilities[i]=1.0/double(ulNumberOfBins);
	}

	//Record that we have no data points (implicitly) "stored" in the distribution
	ulNumberOfDataPoints=0;
}

//This function "trains" the unconditional distribution model by computing the probabilities associated with
//each bin. These probabilities are simply the proportions of the example patterns that lie in each bin. The
//function is designed so that all data points do not have to be added in one go. The function can be repeatedly
//called as additional data becomes available. In this application, only a single call to this function is required.
void CUnconditionalDistribution::dTrainingStep(
							const unsigned long ulNumberOfPatternsInTrainingSet,
							const double * const pdTrainingTargets
							)
{
	unsigned long i,j;
	unsigned long ulNumberOfDataPointsAdded=0;

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

	//Transform the bin probabilities back into the number of samples in each bin by multiplying the bin
	//probabilities by the number of examples "stored" in the model
	for(i=0;i<ulNumberOfBins;i++)
	{
		pdBinProbabilities[i]*=double(ulNumberOfDataPoints);
	}

	//Increment the counts associated with any bin that contains new examples
	for(i=0;i<ulNumberOfPatternsInTrainingSet;i++)
	{
		for(j=0;j<ulNumberOfBins;j++)
		{
			if(pdTrainingTargets[i]>=double(j)/double(ulNumberOfBins) 
				&& pdTrainingTargets[i]<double(j+1)/double(ulNumberOfBins))
			{
				//This pattern (the ith example) is in this (the jth) bin, so increment the counter of the 
				//number of patterms in this bin
				pdBinProbabilities[j]++;
				ulNumberOfDataPointsAdded++;
			}
		}
	}

	//Keep track of the number of examples that have been added to the distribution
	ulNumberOfDataPoints+=ulNumberOfDataPointsAdded;

	//Normalise the bin counts to convert them back into probabilities
	for(i=0;i<ulNumberOfBins;i++)
	{
		pdBinProbabilities[i]/=double(ulNumberOfDataPoints);
	}
}

//This function generates an output from the model by taking a random sample from it.
double CUnconditionalDistribution::dGetOutputs(void)
{
	//Stoirage for the sample
	double dOutput;

	//Select a target cumulative probability. A sample will be generated from the bin that contains this cumulative
	//probability
	double dTargetProbability=double(rand())/double(RAND_MAX);

	//Prepare to count through the bins in turn
	unsigned long ulCurrentBin=0;

	//The cumulative probability
	double dAccumulator=pdBinProbabilities[ulCurrentBin];

	//While we've not yet found the target bin 
	while(dTargetProbability>dAccumulator)
	{
		//Move onto the next bin
		ulCurrentBin++;

		//Accumulate the probabilities
		dAccumulator+=pdBinProbabilities[ulCurrentBin];
	}

	//Now that we've found the correct bin, we generate a specific output by choosing a sample from a uniform
	//distribution over the support of the bin (i.e. somewhere between the lefthand and righthand edge of the bin)
	dOutput=double(ulCurrentBin)/double(ulNumberOfBins)+double(rand())/double(RAND_MAX)*(1.0/double(ulNumberOfBins));

	//Return the sample
	return dOutput;
}
