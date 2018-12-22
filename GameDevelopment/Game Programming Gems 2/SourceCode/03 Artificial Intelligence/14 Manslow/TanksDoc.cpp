//						Tanks
//			(c) Copyright John Manslow
//					  18/04/2001

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
#include "math.h"
#include "fstream.h"

//This information is is worked out from the exemplar file so the initial value isn't too
//important 
unsigned long ulNumberOfPatterns=1000;

//We have three inputs (x-displacement, y-displacement, and wind speed)
unsigned long ulNumberOfInputs=3;

//Ten hidden area used in this application. You should generally use as few as possible, since
//networks with smaller numbers train more quickly, operate more quickly once trained and tend
//to learn the exemplars more robustly.
unsigned long ulNumberOfHiddenNodes=10;

//We have just one output - the angle of the AI tank's barrel necessary to hit the player.
unsigned long ulNumberOfOutputs=1;

//This level of error on the training samples was found to produce a neural network that hit the
//player's tank around 98% of the time (without the random noise added to its output - i.e. with 
//dErrorBias and dErrorVariance set to zero). The value of termination error will have to be changed
//for different types of error measure (see CMLP::dGetPerformance).
double dTerminationError=0.000025;

//Pointers to the neural network and the game world
CMLP *pMLP;
CWorld *pWorld;

//A flag to indicate whether we're just generating training data (i.e. writing examples of
//hits to disk by recording x-displacement, y-displacement, wind speed and the angle
//that was successful) or actually playing a proper game. In that former case, the player
//gets no turn and the neural network AI is disabled.
BOOL boGeneratingTrainingData=FALSE;

//When true, tells the game to load an already trained neural network for use in game. When 
//false causes the game to create a new network and train it.
BOOL boLoadTrainedNetwork=TRUE;

//Information used to scale the inputs to the neural network (see below)
double *pdMin;
double *pdMax;

//The file containing the exemplar data
#define ExemplarData "ExemplarData.txt"

//The file containing the trained MLP
#define TrainedMLP "TrainedMLP.mlp"

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

	//If we're generating training data, we don't want to try to load any or create a neural
	//network.
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
		TRACE("Creating MLP object...");

		//Create the MLP neural network, telling it how many inputs we need (3 in this case,
		//x-displacement, y-displacement, and wind speed), how many hidden neurons (or nodes)
		//and how many outputs (only one - for the inclination of teh tank's barrel). Ten hidden
		//neurons are used here, but you should generally use as few a possible. This helps to
		//speed training and maximise performance in-game. The MLP created has a random set of
		//weights and hence will not do anything useful unless it is trained or a trained set of 
		//weights is loaded into it (as with the pMLP->Load call below).
		pMLP=new CMLP(
						ulNumberOfInputs,
						ulNumberOfHiddenNodes,
						ulNumberOfOutputs
						);
		if(pMLP)
		{
			TRACE("successful.\n");
		}
		else
		{
			TRACE("failed.\n");
			ASSERT(FALSE);
		}

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
				TRACE("\tIteration: %9u  Training Error: %+3.12e\n",
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
		//in game. Start off with quite large values of dTerminationError like 0.25 and work down.
		//Bear in mind that if the output variable varies wildly, larger errors will tend to be 
		//produced. I.e. if the output varies between +1000 and -1000, then a termination error of, 
		//say, 50 may be more suitable.
		while(
				pMLP->dGetPerformance()>dTerminationError &&
				!boGeneratingTrainingData
				);

		TRACE("successful.\n");

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

	//The neural network
	delete pMLP;

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
