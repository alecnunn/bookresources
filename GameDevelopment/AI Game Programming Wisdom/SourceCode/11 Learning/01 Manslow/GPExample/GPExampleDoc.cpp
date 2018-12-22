//GPExample
//Copyright John Manslow
//29/09/2001

// GPExampleDoc.cpp : implementation of the CGPExampleDoc class
//

#include "stdafx.h"
#include "GPExample.h"

#include "GPExampleDoc.h"
#include "fstream.h"
#include "math.h"

#include "CWorld.h"
#include "CGP.h"
#include "CGPNode.h"
#include "CGPANDNode.h"
#include "CGPIfGreaterThanZeroNode.h"
#include "CGPIsFoodNode.h"
#include "CGPIsPoisonNode.h"
#include "CGPIsWallNode.h"
#include "CGPMoveForwardNode.h"
#include "CGPNOTNode.h"
#include "CGPORNode.h"
#include "CGPRandomNumberNode.h"
#include "CGPTurnLeftNode.h"
#include "CGPTurnRightNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//List of prototype GP functions/node tpyes
extern unsigned long ulNumberOfPrototypes;
extern CGPNode **pPrototypeList;

//Pointer toalog file - useful for debugging
ofstream *pLogFile;

/////////////////////////////////////////////////////////////////////////////
// CGPExampleDoc

IMPLEMENT_DYNCREATE(CGPExampleDoc, CDocument)

BEGIN_MESSAGE_MAP(CGPExampleDoc, CDocument)
	//{{AFX_MSG_MAP(CGPExampleDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//Pointers to the view so we can draw to its window
class CGPExampleView;
extern CGPExampleView *pView;

//The environment in which the agent will be evaluated
CWorld *pWorld;

//Pointer to the genetic programming class
CGP *pGP;

//Indicates which way the agent is facing 0-4 = E N W S
int nFacing;
long dx,dy;

//The "health" of the agent, i.e. its performance
double dHealth=0.0;

//The number of fitness evaluations
unsigned long ulIteration=0;

//The number of evaluations of the current program
unsigned long ulEvaluationIteration=0;

//The number of food and poison items eaten by the current program
unsigned long ulFoodEaten=0;
unsigned long ulPoisonEaten=0;

//Whether we're running in slow motion (and hance watching the behaviour of the best program discovered
//thus far
int nSlowMotion=0;

//Pointer to the top node of the program
CGPNode *pdChromosome;

/////////////////////////////////////////////////////////////////////////////
// CGPExampleDoc construction/destruction

CGPExampleDoc::CGPExampleDoc()
{
	unsigned long i;

	//Seed the random number generator
	srand(unsigned(time(NULL)));

	//Creates a log file that records progress - useful for debugging and comparing debug and release versions
	char pString[10000];
#ifdef _DEBUG
	pLogFile=new ofstream("c:\\DebugGPLog.txt");
#else
	pLogFile=new ofstream("c:\\ReleaseGPLog.txt");
#endif
	_strtime(pString);
	*pLogFile<<"Starting at: ";
	*pLogFile<<pString;
	*pLogFile<<"\n";

	//This section of code bubble sorts the prorotypes according to their type IDs. This is only necessary in order
	//to compare the progress of debug and release versions. Without the sort, direct comparisons can't be made 
	//because the prortypes register themselves in a different order
	int nSwaps;
	do
	{
		nSwaps=0;
		for(i=0;i<ulNumberOfPrototypes-1;i++)
		{
			if(pPrototypeList[i]->nType>pPrototypeList[i+1]->nType)
			{
				CGPNode *pBackup=pPrototypeList[i];
				pPrototypeList[i]=pPrototypeList[i+1];
				pPrototypeList[i+1]=pBackup;
				nSwaps=1;
			}
		}
	}
	while(nSwaps);

	//Create the genetic programming class
	pGP=new CGP(100);

	//Create the world
	pWorld=new CWorld(50,50);
}

CGPExampleDoc::~CGPExampleDoc()
{
	//Close the log file and deallocate all the memory
	pLogFile->close();
	delete pLogFile;
	delete pGP;
	delete pWorld;
	delete []pPrototypeList;
}

void TurnLeft(void)
{
	//Rotates to the left
	nFacing--;
	if(nFacing<0)
	{
		nFacing=3;
	}	
}

void TurnRight(void)
{
	//Rotates to the right
	nFacing++;
	if(nFacing>3)
	{
		nFacing=0;
	}	
}

int nLookForward(void)
{
	long lx,ly;

	//Set displacements into the world array according the direction we're facing
	if(nFacing==0)
	{
		dx=1;
		dy=0;
	}
	if(nFacing==1)
	{
		dx=0;
		dy=-1;
	}
	if(nFacing==2)
	{
		dx=-1;
		dy=0;
	}
	if(nFacing==3)
	{
		dx=0;
		dy=1;
	}

	//Convert the displacements into indices into the world array
	lx=pWorld->ulCharacterLocationX+dx;
	ly=pWorld->ulCharacterLocationY+dy;

	//If we're looking beyond the edge of the world, return "wall"
	if(lx<1 || ly<1 || lx>=long(pWorld->ulWorldSizeX) || ly>=long(pWorld->ulWorldSizeY))
	{
		return 2;
	}

	//Otherwise return the contents of the world array
	return pWorld->ppnWorld[lx][ly];
}

int nLookLeft(void)
{
	int ReturnValue;
		
	//We look left by tunring left, looking and turning back
	TurnLeft();
	ReturnValue=nLookForward();
	TurnRight();
	return ReturnValue;
}

int nLookRight(void)
{
	int ReturnValue;

	//We look right by tunring right, looking and turning back;
	TurnRight();
	ReturnValue=nLookForward();
	TurnLeft();
	return ReturnValue;
}

void MoveForward(void)
{
	double dScale=10.0*4.0/5.0;
	//Create a drawing surface
	CClientDC *pDC=new CClientDC((CView*)pView);

	//If we're running in slow motion, its so that we can see the motion of the agent - so we bettwer display it.
	//Since the agent is about to move, we better remove it from the display
	if(nSlowMotion)
	{
		pDC->SelectStockObject(WHITE_PEN);
		pDC->SelectStockObject(WHITE_BRUSH);
		pDC->Rectangle(
									int(pWorld->ulCharacterLocationX*dScale),
									int(pWorld->ulCharacterLocationY*dScale),
									int((pWorld->ulCharacterLocationX+1)*dScale+1),
									int((pWorld->ulCharacterLocationY+1)*dScale+1));
	}

	//Work out which way we're going to move from which way we're facing
	if(nFacing==0)
	{
		dx=1;
		dy=0;
	}
	if(nFacing==1)
	{
		dx=0;
		dy=-1;
	};
	if(nFacing==2)
	{
		dx=-1;
		dy=0;
	}
	if(nFacing==3)
	{
		dx=0;
		dy=1;
	}

	//See whether we've eaten any food, poison, etc. and update the health measure accordingly
	if(pWorld->ppnWorld[pWorld->ulCharacterLocationX+dx][pWorld->ulCharacterLocationY+dy]==1)
	{
		ulFoodEaten++;
		dHealth++;
	}
	if(pWorld->ppnWorld[pWorld->ulCharacterLocationX+dx][pWorld->ulCharacterLocationY+dy]==3)
	{
		ulPoisonEaten++;
		dHealth--;
	}

	//If we can actually move forward (i.e. there's not a wall in front of us) do so
	if(pWorld->ppnWorld[pWorld->ulCharacterLocationX+dx][pWorld->ulCharacterLocationY+dy]!=2)
	{
		pWorld->ppnWorld[pWorld->ulCharacterLocationX][pWorld->ulCharacterLocationY]=0;
		pWorld->ulCharacterLocationX+=dx;
		pWorld->ulCharacterLocationY+=dy;
	}

	//If we're running in slow motion, draw the agent in its new position 
	if(nSlowMotion)
	{
		pDC->SelectStockObject(BLACK_PEN);
		pDC->SelectStockObject(BLACK_BRUSH);
		pDC->Rectangle(
									int(pWorld->ulCharacterLocationX*dScale),
									int(pWorld->ulCharacterLocationY*dScale),
									int((pWorld->ulCharacterLocationX+1)*dScale+1),
									int((pWorld->ulCharacterLocationY+1)*dScale+1));

		//And wait a while before continuing
		unsigned long ulDelay=0;
		do
		{
			ulDelay++;
		}
		while(ulDelay<1e+6);
	}
	delete pDC;

	//Update a counter of the number of steps used so far to evaluate the fitness of this individual
	ulEvaluationIteration++;
}

void Evaluate(void)
{
	unsigned long ulDelay=0;

	SYSTEMTIME StartTime,CurrentTime;
	GetSystemTime(&StartTime);
	unsigned long ulCurrentTime;

	//Create a drawing surface
	CClientDC *pDC=new CClientDC((CView*)pView);

	//If we've finished evaluating the last chromosome and need a new one
	if(ulEvaluationIteration==0)
	{
		//Reset the record of the amount of food and poison eaten
		ulFoodEaten=0;
		ulPoisonEaten=0;

		//Clear the window
		pDC->SelectStockObject(WHITE_PEN);
		pDC->Rectangle(500,0,2000,2000);

		//Keep track of the number of programs evalutaed so far
		ulIteration++;

		//Reste the health (fitness/performance) estimate
		dHealth=0.0;

		//Reset the world
		pWorld->Initialise();

		//If not we're runnning in slow motion (and hence not watching the best performing program)
		if(!nSlowMotion)
		{
			//Get a new program from the GP class so that we can evaluate its performance
			pdChromosome=pGP->pGetChromosomeForEvaluation();
		}
		else
		{
			//Otherwise get a copy of the best program found so far so that we can watch it perform
			pdChromosome=pGP->pGetBestChromosome();

			//Draw the world
			pWorld->Draw(pDC);
		}
	}
	char pString[1000];
	unsigned long ulStartTime=	
													StartTime.wMilliseconds
													+1000*StartTime.wSecond
													+60*StartTime.wMinute
													+3600*StartTime.wHour
													+24*3600*StartTime.wDay;
	do
	{
		//Run the program
		pdChromosome->dEvaluate();

		//Keep track of the number of times its been executed
		ulEvaluationIteration++;

		//Put some info on the screen so that we can monitor the progress
		sprintf(pString,"Food eaten:           %u",ulFoodEaten);
		pDC->TextOut(550,20,pString);
		sprintf(pString,"Poison eaten:         %u",ulPoisonEaten);
		pDC->TextOut(550,40,pString);
		sprintf(pString,"Highest fitness:    %+6.2lf",pGP->dGetBestPerformance());
		pDC->TextOut(550,60,pString);

		GetSystemTime(&CurrentTime);
		ulCurrentTime=	
													CurrentTime.wMilliseconds
													+1000*CurrentTime.wSecond
													+60*CurrentTime.wMinute
													+3600*CurrentTime.wHour
													+24*3600*CurrentTime.wDay;
	}
	//Keep going for either 50ms or until we've finished. See additional comments for this line in 
	//CGAPBILExampleDoc.cpp
	while(ulCurrentTime-ulStartTime<1000 && ulEvaluationIteration<=5000);

	//If we have actually finished
	if(ulEvaluationIteration>5000)
	{
		//Reset the counter of the number of step made in evaluating the current program
		ulEvaluationIteration=0;

		//Reduce the estmiate of the program's health (i.e. performance) by subtracting some function of the number
		//of nodes (instricutions) in the program. Typcially, the lengths of the programs produced by GP tend to 
		//increase in length with time and usually contain large redundant sections. This makes the programs inefficient
		//and impossible to understand. The penalty provided by the line below helps to encourage the emergence of
		//shorter programs
		dHealth-=0.1*pow(pdChromosome->ulGetNumberOfNodesInSubtree(0),1.0);

		//If we're not just watching the best program perform
		if(!nSlowMotion)
		{
			//Set the health (performance) of the program that was just evaluated
			pGP->SetFitness(dHealth);

			//If we've reached out performance target, stop evolving and just watch the best performer
			if(pGP->dGetBestPerformance()>250)
			{
				nSlowMotion=1;
			}
		}
	}
	//Delete the drawing surface
	delete pDC;
}

BOOL CGPExampleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGPExampleDoc serialization

void CGPExampleDoc::Serialize(CArchive& ar)
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
// CGPExampleDoc diagnostics

#ifdef _DEBUG
void CGPExampleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGPExampleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGPExampleDoc commands
