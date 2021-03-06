//Tanks
//Copyright John Manslow
//29/09/2001

// TanksDoc.cpp : implementation of the CTanksDoc class
//

#include "stdafx.h"
#include "Tanks.h"
#include "TanksDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double pi=3.1415926535;

#include "CWorld.h"
#include "CTank.h"
#include "CProjectile.h"
#include "CMLP.h"
#include "CConditionalDistribution.h"
#include "CUnconditionalDistribution.h"
#include "math.h"
#include "fstream.h"

//This information is worked out from the exemplar files so these values aren't too
//important.
unsigned long ulNumberOfPatterns=1000;
unsigned long ulNumberOfErrorPatterns=1000;

//The number of inputs to the neural network that calculates the optimal barrel angle and to the conditional
//aiming error distribution model
unsigned long ulNumberOfInputs=3;
unsigned long ulNumberOfErrorInputs=1;

//Ten hidden neurons are used in the barrel angle MLP, and six in the MLP component of the conditional error
//distribution model. You should generally use as few as possible, since networks with smaller numbers 
//train more quickly, operate more quickly once trained and tend to learn the exemplars more robustly.
unsigned long ulNumberOfHiddenNodes=10;
unsigned long ulNumberOfHiddenNodesInConditionalErrorModel=6;

//This many bins gives good resolution but produces overfitting in the conditional error distribution model when 
//the preceding shot was poor (since such shots are relatively rare in the exemplar data). This isn't a huge problem
//in game, since such shots are rare there too!
unsigned long ulNumberOfBinsInDistributionModels=50;

//For the barrel angle MLP, we have just one output - the angle of the AI tank's barrel taht is necessary to hit 
//the player.
unsigned long ulNumberOfOutputs=1;

//This level of error on the training samples was found to produce a barrel angle neural network that hit the
//player's tank around 98% of the time. The value of termination error will have to be changed
//for different types of error measure (see CMLP::dGetPerformance).
double dTerminationError=0.000025;

//The error measure used by the conditional probability model has different units to that used by the neural network
//that calculates the AI tank's barrel angle. In fact, for the distribution model, the units are log-probabilities. Terminating
//training at this error seems to give a good distribution of aiming errors for the AI tank
double dErrorModelTerminationError=2.455;

//Pointers to the distribution models, the barrel angle MLP, and the game world object
CConditionalDistribution *pErrorDistribution;
CUnconditionalDistribution *pUnconditionalErrorDistribution;
CMLP *pMLP;
CWorld *pWorld;

//Should always be true. See the book Game Programming Gems 2 for the original purpose of this flag. 
BOOL boGeneratingTrainingData=FALSE;

//A flag to indicate whether we're just generating training data (i.e. writing examples of
//aiming errors to disk or actually playing a proper game. In the former case, the AI doesn't get
//a turn and the player controls the AI tank. The aiming errors made as the player does this
//are logged to disk and can be used later to train the unconditional and conditional aiming error 
//distribution models.
BOOL boGeneratingErrorTrainingData=FALSE;

//Should always be true. See the book Game Programming Gems 2 for the original purpose of this flag. 
BOOL boLoadTrainedNetwork=TRUE;

//When true, tells the game to load an pre-trained conditional distribution model. When 
//false causes the game to create a new model and train it using examples loaded from disk
//(which were generated by playing the game with boGeneratingErrorTrainingData=TRUE).
//Note that unconditional distribution models are so quick to create we don't bother saving them 
//and re-train them every time the game runs
BOOL boLoadTrainedErrorNetwork=TRUE;

//Information used to scale the inputs to the various models (see below)
double *pdMin;
double *pdMax;
double *pdErrorInputMin;
double *pdErrorInputMax;
double dErrorMin,dErrorMax;

//The file containing the exemplar data
#define ExemplarData "BarrelAngleExemplarData.txt"

//#define ErrorExemplarData "NewErrorExemplarData.txt"
#define ErrorExemplarData "AimingErrorExemplarData.txt"

//The file containing the trained neural network that is used to calculate the optimal barrel angle for the AI tank
#define TrainedMLP "TrainedBarrelAngleMLP.mlp"

//The file containing the trained aiming error distribution model that is used to add random
//variation to the aiming of the AI tank.
#define TrainedConditionalDistributionModel "TrainedAimingErrorModel.cdm"

IMPLEMENT_DYNCREATE(CTanksDoc, CDocument)

BEGIN_MESSAGE_MAP(CTanksDoc, CDocument)
	//{{AFX_MSG_MAP(CTanksDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTanksDoc::CTanksDoc()
{
	//Seed the random number generator
	srand(unsigned(time(NULL)));

	TRACE("---------------------------------------------------------------------------------\n");
	TRACE("Initialising.\n");
	TRACE("---------------------------------------------------------------------------------\n");

	//Create the game world. The terrain will be 760 units long.
	TRACE("Creating world...\n");
	pWorld=new CWorld(760);

	//Check to make sure it was a success.
	if(pWorld)
	{
		TRACE("successful.\n");
	}
	else
	{
		//If not, inform the user and assert.
		TRACE("failed.\n");
		ASSERT(FALSE);
	}

	//Initialise these arrays with NULL pointers so we don't try to delete them later
	//even if they're unused.
	pdMin=NULL;
	pdMax=NULL;

	/******************************************************************************************************************/
	//This section of code loads training data for and trains the MLP neural network that computes the "optimal"
	//angle for the AI tank's barrel. The way this is done was described in detail in the book 
	//"Game Programming Gems 2".
	if(!boGeneratingTrainingData)
	{
		unsigned long i,j;
		TRACE("Opening training data file...");

		//Open the file containing the training data.
		ifstream *pTrainingData;
		pTrainingData=new ifstream(ExemplarData,ios::nocreate);
		if(pTrainingData)
		{
			TRACE("successful.\n");
		}
		else
		{
			TRACE("failed.\n");
			ASSERT(FALSE);
		}

		//Read in the number of examples (patterns) the file contains and how many inputs. In this
		//case, there should be three inputs, corresponging to x and y displacement between the
		//tanks and wind speed. We don't need to load the number of outputs because in this 
		//application, we'll always have only one - the angle of the tank's barrel
		*pTrainingData>>ulNumberOfPatterns;
		*pTrainingData>>ulNumberOfInputs;
		
		TRACE("Loading training data...");

		//Allocate some memory for the example data
		double **ppdTrainingInputs;
		double **ppdTrainingTargets;

		ppdTrainingInputs=new double*[ulNumberOfPatterns];
		ppdTrainingTargets=new double*[ulNumberOfPatterns];

		//Allocate memory to record the maximum and minimum values of each input
		pdMin=new double[ulNumberOfInputs];
		pdMax=new double[ulNumberOfInputs];

		//If any of the memory allocation failed, alert the user.
		if(!(	ppdTrainingInputs &&
				ppdTrainingTargets &&
				pdMin &&
				pdMax))
		{
			TRACE("failed.\n");
			ASSERT(FALSE);
		}

		//Initialise the min/max statistics to large values to ensure that they'll be overwritten
		//when the data are analysed.
		for(i=0;i<ulNumberOfInputs;i++)
		{
			pdMin[i]=1000.0;
			pdMax[i]=-1000.0;
		}

		//This code loads the example data and records the minimum and maximum values attained by
		//each input
		for(i=0;i<ulNumberOfPatterns;i++)
		{
			//Allocate memory to store the ith input example and check to make sure it succeded
			ppdTrainingInputs[i]=new double[ulNumberOfInputs];
			if(!ppdTrainingInputs[i])
			{
				TRACE("failed.\n");
				ASSERT(FALSE);
			}

			//Load the ith example
			for(j=0;j<ulNumberOfInputs;j++)
			{
				*pTrainingData>>ppdTrainingInputs[i][j];
			}

			//Allocate memory to store the corresponding output (barrel angle) of the ith pattern
			ppdTrainingTargets[i]=new double[1];
			if(!ppdTrainingTargets[i])
			{
				TRACE("failed.\n");
				ASSERT(FALSE);
			}

			//Load it
			*pTrainingData>>ppdTrainingTargets[i][0];

			//Maintain the record of the maximum and minimum values of each input
			for(j=0;j<ulNumberOfInputs;j++)
			{
				if(ppdTrainingInputs[i][j]<pdMin[j])
				{
					pdMin[j]=ppdTrainingInputs[i][j];
				}
				if(ppdTrainingInputs[i][j]>pdMax[j])
				{
					pdMax[j]=ppdTrainingInputs[i][j];
				}
			}
		}

		pTrainingData->close();
		delete pTrainingData;

		//Once all data has been loaded, this code scales the inputs so that they all lie in the range
		//-1 to +1. This is not strictly necessary but can often speed up neural network learning
		//quite significantly. Note that all values put into the network in future must be scaled 
		//in the same way, so the array of min/max values for each input have to be stored for future 
		//use. For example, look in the OnTimer function in TanksView.cpp where the inputs to the
		//network are scaled using the same min/max values used here.
		for(i=0;i<ulNumberOfPatterns;i++)
		{
			for(j=0;j<ulNumberOfInputs;j++)
			{
				//Inputs range between min and max
				ppdTrainingInputs[i][j]-=pdMin[j];
				//Inputs range between 0 and max-min
				ppdTrainingInputs[i][j]/=(pdMax[j]-pdMin[j]);
				//Inputs range between 0 and 1
				ppdTrainingInputs[i][j]-=0.5;
				//Inputs range between -0.5 and +0.5
				ppdTrainingInputs[i][j]*=2.0;
				//Inputs range between -1.0 and +1.0
			}
		}
		TRACE("successful.\n");

		//Now we have the example data with which to teach a neural network, we need a network
		//to teach.

		//Create the MLP neural network, telling it how many inputs we need (3 in this case,
		//x-displacement, y-displacement, and wind speed), how many hidden neurons (or nodes)
		//and how many outputs (only one - for the inclination of the AI tank's barrel). Ten hidden
		//neurons are used here, but you should generally use as few a possible. This helps to
		//speed training and maximise performance in-game. The MLP created has a random set of
		//weights and hence will not do anything useful unless it is trained or a trained set of 
		//weights is loaded into it (as with the pMLP->Load call below).
		pMLP=new CMLP(
						ulNumberOfInputs,
						ulNumberOfHiddenNodes,
						ulNumberOfOutputs
						);

		//Create a variable to store return values
		int nReturnValue;

		//Do we want to load a pre-trained network?
		if(boLoadTrainedNetwork)
		{
			//If yes, load it.
			TRACE("Loading MLP...");
			nReturnValue=pMLP->Load(&TrainedMLP);
			if(nReturnValue)
			{
				TRACE("successful.\n");
			}
			else
			{
				TRACE("failed.\n");
				ASSERT(FALSE);
			}
		}

		TRACE("Training MLP...\n");

		//Create a variable to record the number of training iterations
		unsigned long ulIteration=0;

		//This do-while loop actually does the training. It continually calls the neural network's
		//training function (each call doing only a single training step) until the network's
		//ability to reproduce the targets (barrel angles) contained in the example data (as
		//measured by the network's dBestError variable) is adequate. This training process will
		//require many tens or hundred of thousands of steps and can last several hours. Of course,
		//this shouldn't really be done here in the constructor...
		do
		{
			//To perform a training step, tell the network how many patterns (examples) there are
			//in the example data and pass it pointers to it.
			pMLP->dTrainingStep(
									ulNumberOfPatterns,
									ppdTrainingInputs,
									ppdTrainingTargets
									);


			//Every hundred training steps provide some feedback on the progress of the network's
			//learning
			if(ulIteration%100==0)
			{
				TRACE("\n\tIteration: %9u  Training Error: %+3.12e",
								ulIteration,
								pMLP->dGetPerformance());
			}

			//Keep a count of the number of steps so far
			ulIteration++;
		}
		//Keep going until the network's ability to reproduce the barrel angles in the example
		//data is good enough. The error produced by the network will never be zero, so 
		//dTerminationError will need to be some positive value. Some experimentation will be
		//required to see what value of dTerminationError is required to give adequate performance
		//in game. Start off with quite large values of dTerminationError like 0.1 and work down.
		//Bear in mind that if the output variable varies wildly, larger errors will tend to be 
		//produced. I.e. if the output varies between +1000 and -1000, then a termination error of, 
		//say, 50 may be more suitable.
		while(
				pMLP->dGetPerformance()>dTerminationError &&
				!boGeneratingTrainingData
				);

		TRACE("\nsuccessful.\n");

		//Once we've finished training, we don't need the exemplar data any more, so we can delete it.
		for(i=0;i<ulNumberOfPatterns;i++)
		{
			delete []ppdTrainingInputs[i];
			delete []ppdTrainingTargets[i];
		}
		delete []ppdTrainingInputs;
		delete []ppdTrainingTargets;
		
		//Save the trained network
		TRACE("Saving trained MLP...");

		//We'll save this new network under a different name so we don't overwrite the old version
		nReturnValue=pMLP->Save(&"NewTrainedMLP.mlp");
		if(nReturnValue)
		{
			TRACE("successful.\n");
		}
		else
		{
			TRACE("failed.\n");
			ASSERT(FALSE);
		}
	}
	/******************************************************************************************************************/
	//This code loads the examples of aiming errors, scales them as required by the unconditional and
	//conditional distribution models, and trains those models.
	if(!boGeneratingErrorTrainingData)
	{
		unsigned long i,j;
		TRACE("Opening error training data file...");

		//Open the file containing the error training data.
		ifstream *pErrorTrainingData;
		pErrorTrainingData=new ifstream(ErrorExemplarData,ios::nocreate);
		if(pErrorTrainingData)
		{
			TRACE("successful.\n");
		}
		else
		{
			TRACE("failed.\n");
			ASSERT(FALSE);
		}

		//Read in the number of examples the file contains.
		*pErrorTrainingData>>ulNumberOfErrorPatterns;
		*pErrorTrainingData>>ulNumberOfErrorInputs;
		
		//Create a new conditional distribution model that will be used to capture the relationship between the
		//aiming error made on one shot and the error on the next. 
		pErrorDistribution=new CConditionalDistribution(
																							ulNumberOfErrorInputs,
																							ulNumberOfHiddenNodesInConditionalErrorModel,
																							ulNumberOfBinsInDistributionModels);
		if(boLoadTrainedErrorNetwork)
		{
			//If we've already trained one, load it
			pErrorDistribution->Load(TrainedConditionalDistributionModel);
		}

		//Create a new unconditional distribution model that will be used to represent the distribution of aiming
		//errors made on the first shot of each game
		pUnconditionalErrorDistribution=new CUnconditionalDistribution(ulNumberOfBinsInDistributionModels);

		TRACE("Loading error training data...");

		//Allocate some memory for the example data
		double **ppdErrorTrainingInputs;
		double *pdErrorTrainingTargets;

		ppdErrorTrainingInputs=new double*[ulNumberOfErrorPatterns];
		pdErrorTrainingTargets=new double[ulNumberOfErrorPatterns];

		//Allocate memory to record the maximum and minimum values of each input. All inputs to the conditional
		//distribution model need to be scaled into the range -1 to +1, and the example outputs for both models
		//in 0 to +1.
		pdErrorInputMin=new double[ulNumberOfErrorInputs];
		pdErrorInputMax=new double[ulNumberOfErrorInputs];

		//If any of the memory allocation failed, alert the user.
		if(!(	ppdErrorTrainingInputs &&
				pdErrorTrainingTargets &&
				pdErrorInputMin &&
				pdErrorInputMax))
		{
			TRACE("failed.\n");
			ASSERT(FALSE);
		}

		//Initialise the min/max statistics to large values to ensure that they'll be overwritten
		//when the data are analysed.
		for(i=0;i<ulNumberOfErrorInputs;i++)
		{
			pdErrorInputMin[i]=1000.0;
			pdErrorInputMax[i]=-1000.0;
		}
		dErrorMin=1000.0;
		dErrorMax=-1000.0;

		//This code loads the example data and records the minimum and maximum values attained by
		//each input

		 //Each line of the data file consists of a number indicating when the shot occurred in the game 
		//(i.e. 1 means the first shot of a new game) and the angular error that the player made when 
		//aiming on that shot
		for(i=0;i<ulNumberOfErrorPatterns;i++)
		{
			//Allocate memory to store the ith input example and check to make sure it succeded
			ppdErrorTrainingInputs[i]=new double[ulNumberOfErrorInputs];
			if(!ppdErrorTrainingInputs[i])
			{
				TRACE("failed.\n");
				ASSERT(FALSE);
			}

			//Load the ith example
			for(j=0;j<ulNumberOfErrorInputs;j++)
			{
				*pErrorTrainingData>>ppdErrorTrainingInputs[i][j];
			}

			//Allocate memory to store the corresponding output (angular aiming error in rads) of the ith pattern
			if(!pdErrorTrainingTargets[i])
			{
				TRACE("failed.\n");
				ASSERT(FALSE);
			}

			//Load it
			*pErrorTrainingData>>pdErrorTrainingTargets[i];

			if(pdErrorTrainingTargets[i]<dErrorMin)
			{
				dErrorMin=pdErrorTrainingTargets[i];
			}
			if(pdErrorTrainingTargets[i]>dErrorMax)
			{
				dErrorMax=pdErrorTrainingTargets[i];
			}

			//Maintain a record of the maximum and minimum values of each input
			for(j=0;j<ulNumberOfErrorInputs;j++)
			{
				if(ppdErrorTrainingInputs[i][j]<pdErrorInputMin[j])
				{
					pdErrorInputMin[j]=ppdErrorTrainingInputs[i][j];
				}
				if(ppdErrorTrainingInputs[i][j]>pdErrorInputMax[j])
				{
					pdErrorInputMax[j]=ppdErrorTrainingInputs[i][j];
				}
			}
		}

		//Close and delete the exemplar data file
		pErrorTrainingData->close();
		delete pErrorTrainingData;

		//Now scale all inputs to lie between -1.0 and +1.0
		for(i=0;i<ulNumberOfErrorPatterns;i++)
		{
			for(j=0;j<ulNumberOfErrorInputs;j++)
			{
				//Inputs range between min and max
				ppdErrorTrainingInputs[i][j]-=pdErrorInputMin[j];
				//Inputs range between 0 and max-min
				ppdErrorTrainingInputs[i][j]/=(pdErrorInputMax[j]-pdErrorInputMin[j]);
				//Inputs range between 0 and 1
				ppdErrorTrainingInputs[i][j]-=0.5;
				//Inputs range between -0.5 and +0.5
				ppdErrorTrainingInputs[i][j]*=2.0;
				//Inputs range between -1.0 and +1.0
			}
		}

		//Scale the distribution model targets to lie between 0 to +1. This needs to be done
		//because the positions of the bins in the models are fixed and distributed evenly in this range.
		for(i=0;i<ulNumberOfErrorPatterns;i++)
		{
			pdErrorTrainingTargets[i]-=dErrorMin;
			pdErrorTrainingTargets[i]/=(dErrorMax-dErrorMin);
		}

		TRACE("successful.\n");

		//Work out how many examples we're going to have to train the unconditional and conditional models.
		//(See below for more details of how this works.)
		unsigned long ulNumberOfUnconditionalPatterns=0;
		unsigned long ulNumberOfConditionalPatterns=0;
		for(i=0;i<ulNumberOfErrorPatterns;i++)
		{
			if(ppdErrorTrainingInputs[i][0]==-1)
			{
				ulNumberOfUnconditionalPatterns++;
			}
			else
			{
				ulNumberOfConditionalPatterns++;
			}
		}

		//Allocate memory to store seperate data sets that will be used to train the unconditional and conditional
		//distribution models. Note that the number of inputs to the conditional distribution is fixed at one in this 
		//application because we're are assuming that the distribution of aiming errors on each shot is primarily
		//affected by the aiming error on the preceding shot.
		double *pdUnconditionalTrainingTargets=new double[ulNumberOfUnconditionalPatterns];
		double *pdConditionalTrainingTargets=new double[ulNumberOfConditionalPatterns];
		double **ppdConditionalTrainingInputs=new double*[ulNumberOfConditionalPatterns];
		for(i=0;i<ulNumberOfConditionalPatterns;i++)
		{
			ppdConditionalTrainingInputs[i]=new double[1];
		}
	
		//This section of code partitions the exemplar data into unconditional and conditional data sets. The 
		//unconditional data set contains the aiming errors made on the first shot of each game. The conditional
		//data set consists of input-output pairs, where, for each pair, the input consists of the aiming error
		//made on the previous shot, and the output the aiming error made on the current shot. The conditional 
		//model of aiming errors thus represents the effect of the accuracy of each shot on the accuracy of the next.
		double dLastError=0.0;
		ulNumberOfUnconditionalPatterns=0;
		ulNumberOfConditionalPatterns=0;
		for(i=0;i<ulNumberOfErrorPatterns;i++)
		{
			//This condition is true if the current pattern (example) was the first shot of a new game
			if(ppdErrorTrainingInputs[i][0]==-1)
			{
				//If it was, add it to the data that will be used to train the unconditional distribution
				pdUnconditionalTrainingTargets[ulNumberOfUnconditionalPatterns]=pdErrorTrainingTargets[i];
				ulNumberOfUnconditionalPatterns++;
			}
			else
			{
				//If it wasn't, add it as a target output for the conditional distribution model...
				pdConditionalTrainingTargets[ulNumberOfConditionalPatterns]=pdErrorTrainingTargets[i];

				//...and set the corresponding input to be equal to the error made on the preceding shot. Note that
				//this has to be re-scaled to be between -1 and +1.
				ppdConditionalTrainingInputs[ulNumberOfConditionalPatterns][0]=2.0*(dLastError-0.5);
				ulNumberOfConditionalPatterns++;
			}

			//Keep a record of the aiming error that was made on the current shot
			dLastError=pdErrorTrainingTargets[i];
		}
		
		//Now we've copied this data into two new data sets, we can delete the original
		for(i=0;i<ulNumberOfErrorPatterns;i++)
		{
			delete []ppdErrorTrainingInputs[i];
		}
		delete []ppdErrorTrainingInputs;
		delete []pdErrorTrainingTargets;
		
		TRACE("Training error distribution model...\n");
		unsigned long ulIteration=0;

		//This do-while loop trains the conditional distribution model. It continually calls the distribution's
		//training function until the measured performance is adequate. The process typically requires
		//many tens or hundred of thousands of steps and can last several hours. In fact, the trained model provided
		//on the CD was produced by training over night using an Intel Celeron 500MHz processor.
		do
		{
			//To perform a training step, tell the model how many patterns (examples) there are
			//in the example data and pass it pointers to it.
			pErrorDistribution->dTrainingStep(
									ulNumberOfConditionalPatterns,
									ppdConditionalTrainingInputs,
									pdConditionalTrainingTargets
									);



			//Every hundred training steps provide some feedback on the progress of the model's
			//learning (if in debug mode)
			if(ulIteration%100==0)
			{
				TRACE("\n\tIteration: %9u\tTraining Error: %+12.6e\tStep Size: %12.6e",
								ulIteration,
								pErrorDistribution->dGetPerformance(),
								pErrorDistribution->dStepSize);
			}

			//Keep a count of the number of steps so far
			ulIteration++;
		}
		while(
					pErrorDistribution->dGetPerformance()>dErrorModelTerminationError
				);

		TRACE("\nsuccessful.\n");

		//Once we've finished training, we don't need the processed versions of the exemplar data
		//so we can delete them
		for(i=0;i<ulNumberOfConditionalPatterns;i++)
		{
			delete []ppdConditionalTrainingInputs[i];
		}
		delete []ppdConditionalTrainingInputs;
		delete []pdConditionalTrainingTargets;

		//Train the unconditional distribution
		pUnconditionalErrorDistribution->dTrainingStep(ulNumberOfUnconditionalPatterns,pdUnconditionalTrainingTargets);

		//Delete the unconditional exemplar data
		delete []pdUnconditionalTrainingTargets;
	
	}

	TRACE("---------------------------------------------------------------------------------\n");
	TRACE("Game starting.\n");
	TRACE("---------------------------------------------------------------------------------\n");
}

CTanksDoc::~CTanksDoc()
{
	//Deallocate memory:
	TRACE("---------------------------------------------------------------------------------\n");
	TRACE("Game finishing.\n");
	TRACE("---------------------------------------------------------------------------------\n");

	//If the scaling information exists, delete it
	if(pdMin)
	{
		delete []pdMin;
	}
	if(pdMax)
	{
		delete []pdMax;
	}
	if(pdErrorInputMin)
	{
		delete []pdErrorInputMin;
	}
	if(pdErrorInputMax)
	{
		delete []pdErrorInputMax;
	}

	//The neural networks
	delete pMLP;
	delete pErrorDistribution;
	delete pUnconditionalErrorDistribution;

	//And the game world
	TRACE("Deleting world...\n");
	delete pWorld;
	TRACE("successful.\n");

	//That's it, we've finished!
	TRACE("---------------------------------------------------------------------------------\n");
	TRACE("Finished.\n");
	TRACE("---------------------------------------------------------------------------------\n");
}

BOOL CTanksDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTanksDoc serialization

void CTanksDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTanksDoc diagnostics

#ifdef _DEBUG
void CTanksDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTanksDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTanksDoc commands
