//GAPBILExample
//Copyright John Manslow
//29/09/2001

// GAPBILExampleDoc.cpp : implementation of the CGAPBILExampleDoc class
//

#include "stdafx.h"
#include "GAPBILExample.h"

#include "GAPBILExampleDoc.h"

#include "CWorld.h"
#include "CPBIL.h"
#include "CGA.h"
#include "CPS.h"

class CGAPBILExampleView;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleDoc

IMPLEMENT_DYNCREATE(CGAPBILExampleDoc, CDocument)

BEGIN_MESSAGE_MAP(CGAPBILExampleDoc, CDocument)
	//{{AFX_MSG_MAP(CGAPBILExampleDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern CGAPBILExampleView *pView;

//Pointer to the world in which the agent will be evaluated
CWorld *pWorld;

//The optimisation algorithms that can be used
CGA *pOptimiser;
//CPBIL *pOptimiser;
//CPS *pOptimiser;

//The directions the agent is facing 0 - 4 = E N W S
int nFacing;
long dx,dy;

//The health (performance) of the current agent
double dHealth=0.0;

//The number of rules that can be used
unsigned long ulNumberOfRules=6;

//The number of performance measurements that have been made
unsigned long ulIteration=0;

//The number of iterations used in evaluating the current rule base
unsigned long ulEvaluationIteration=0;

//The chromosome currently under evaluation
double *pdChromosome=NULL;

//The quantity of food and poison eaten thus far
unsigned long ulFoodEaten=0;
unsigned long ulPoisonEaten=0;

//Whether we're running in slow motion - i.e. juts watching the behaviour of the best rule base
int nSlowMotion=0;

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleDoc construction/destruction

CGAPBILExampleDoc::CGAPBILExampleDoc()
{
	unsigned long i;

	//Seed the random number generator
	srand(unsigned(time(NULL)));

	//Create the world
	pWorld=new CWorld(50,50);

	//This array contains information about the types required for each gene in the optimisation algorithm. In this 
	//case, they all all binary - that is, ot type zero
	int *pnTypes=new int[16*ulNumberOfRules+3];
	for(i=0;i<16*ulNumberOfRules+3;i++)
	{
		pnTypes[i]=0;
	}

	//Create whichever type of optimisation algorithm is required
	pOptimiser=new CGA(50,8*ulNumberOfRules+3,pnTypes);
//	pOptimiser=new CPBIL(50,8*ulNumberOfRules+3);
//	pOptimiser=new CPS(8*ulNumberOfRules+3,pnTypes);

	//Clean up
	delete []pnTypes;

	//Load an optimisation algorithm if there's one pre-saved
//		pOptimiser->Load("GA.txt");
//		pOptimiser->Load("PBIL.txt");
//		pOptimiser->Load("PS.txt");
}

CGAPBILExampleDoc::~CGAPBILExampleDoc()
{
	delete pOptimiser;
	delete pWorld;
	delete []pdChromosome;
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
	unsigned long i;

	SYSTEMTIME StartTime,CurrentTime;
	GetSystemTime(&StartTime);
	unsigned long ulCurrentTime;

	//Create a drawing surface
	CClientDC *pDC=new CClientDC((CView*)pView);

	//If we've finished evaluating the last chromosome and need a new one
	if(ulEvaluationIteration==0)
	{
		unsigned long ulDelay=0;

		//This function evaluates the fitness of an individual
		ulFoodEaten=0;
		ulPoisonEaten=0;

		pDC->SelectStockObject(WHITE_PEN);
		pDC->Rectangle(500,0,2000,2000);

		ulEvaluationIteration=0;
		ulIteration++;
		dHealth=0.0;

		//If we're not running in slow motion (i.e. actually watching the agent move around)
		if(!nSlowMotion)
		{
			//Get a new chromosome from the optimisation algorithm so that its performance can be evaluated
			pdChromosome=pOptimiser->pdGetChromosomeForEvaluation();
		}
		else
		{
			//If we are watching teh agent, redraw the world
			pWorld->Draw(pDC);

			//And get the best performing chromosome (because we want to see how the best behaves)
			pdChromosome=pOptimiser->pdGetBestChromosome();
		}
	}
	//This section of interprets the chromosome as a series of rules. Each rule consists of eight genes. The first
	//controls whether the rule always fires when its antecedent is true, or only with probability 0.5. The second, third 
	//forth and fifth form the rest of the antecednt and try to match the object that the agent can see to the four types
	//space, food, poison, and wall. The matches and ANDed in the antecedent and hence a rule only fires if they all
	//match. Genes six to eight form the consequent - what the agent does if the rule fires. This can be either turn
	//left, turn right or move forward. Finally, there are three genes that specify a deafult action that is to be taken when
	//none of the rules fire. 
	unsigned long ulBaseGene;
	int nRuleFires=0;
	int nFireDefaultRule=0;

	unsigned long ulStartTime=	
													StartTime.wMilliseconds
													+1000*StartTime.wSecond
													+60*StartTime.wMinute
													+3600*StartTime.wHour
													+24*3600*StartTime.wDay;
	do
	{
		nFireDefaultRule=1;

		//For each rule
		for(i=0;i<ulNumberOfRules;i++)
		{
			ulBaseGene=i*8;
			nRuleFires=1;

			//If gene zero is set, the rule is stochastic and may not fire even its antecedent is true
			if(pdChromosome[ulBaseGene]==1 && rand()%2==0)
			{
				nRuleFires=0;
			}

			//Match what the agent can see to the rule antecedents to see which fire
			if(i/4==0)
			{
				if(pdChromosome[ulBaseGene+1]!=(nLookForward()==0))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+2]!=(nLookForward()==1))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+3]!=(nLookForward()==2))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+4]!=(nLookForward()==3))
				{
					nRuleFires=0;
				}
			}
			if(i/4==1)
			{
				if(pdChromosome[ulBaseGene+1]!=(nLookLeft()==0))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+2]!=(nLookLeft()==1))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+3]!=(nLookLeft()==2))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+4]!=(nLookLeft()==3))
				{
					nRuleFires=0;
				}
			}
			if(i/4==2)
			{
				if(pdChromosome[ulBaseGene+1]!=(nLookRight()==0))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+2]!=(nLookRight()==1))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+3]!=(nLookRight()==2))
				{
					nRuleFires=0;
				}
				if(pdChromosome[ulBaseGene+4]!=(nLookRight()==3))
				{
					nRuleFires=0;
				}
			}

			//If the rule fires
			if(nRuleFires)
			{
				//the default won't
				nFireDefaultRule=0;

				//Decide what action to take based on the genes in the consequent
				if(pdChromosome[ulBaseGene+5]==1 && pdChromosome[ulBaseGene+6]==0)
				{
					TurnLeft();
				}
				if(pdChromosome[ulBaseGene+5]==0 && pdChromosome[ulBaseGene+6]==1)
				{
					TurnRight();
				}
				if(pdChromosome[ulBaseGene+7]==1)
				{
					MoveForward();
				}
			}
		}

		ulBaseGene=i*8;

		//If no rules have fired,
		if(nFireDefaultRule)
		{
			//Take the default action
			if(pdChromosome[ulBaseGene]==1 && pdChromosome[ulBaseGene+1]==0)
			{
				TurnLeft();
			}
			if(pdChromosome[ulBaseGene]==0 && pdChromosome[ulBaseGene+1]==1)
			{
				TurnRight();
			}
			if(pdChromosome[ulBaseGene+2]==1)
			{
				MoveForward();
			}
		}

		//Keep track of how many steps we've taken in evaluating the performance of the current chromosome
		ulEvaluationIteration++;

		//Put some info on the display so we can keep track of our progress
		char pString[1000];
		sprintf(pString,"Food eaten:           %u",ulFoodEaten);
		pDC->TextOut(550,20,pString);
		sprintf(pString,"Poison eaten:         %u",ulPoisonEaten);
		pDC->TextOut(550,40,pString);
		sprintf(pString,"Highest fitness:    %+6.2lf",pOptimiser->dGetBestPerformance());
		pDC->TextOut(550,60,pString);

		GetSystemTime(&CurrentTime);
		ulCurrentTime=	
													CurrentTime.wMilliseconds
													+1000*CurrentTime.wSecond
													+60*CurrentTime.wMinute
													+3600*CurrentTime.wHour
													+24*3600*CurrentTime.wDay;
	}
	//Keep going until we've evaluated the current chromosome. Evaluating it over too many iterations takes a lot
	//of time. Evaluating over too few makes the fitness estimates too noisy. Also, since the world starts off full
	//of food and poison, evaluating an agent for too short a time is likely to produce one that performs well only when
	//the world is in that state, but poorly later on once most of the food (and poison) have been eaten. This is a 
	//result of evaluating the agent's fitness in an unrepresentative environment (see the article for more details)
	while(ulCurrentTime-ulStartTime<1000 && ulEvaluationIteration<=5000);

	//If we have actually finished
	if(ulEvaluationIteration>5000)
	{

		//Reset the counter of the number of step made in evaluating the current program
		ulEvaluationIteration=0;

		//Reset the world
		pWorld->Initialise();

		//If we're not simple watching the best performing agent
		if(!nSlowMotion)
		{
			//Tell the optimisation algorithm how well the chromosome performed
			pOptimiser->SetFitness(dHealth);
			TRACE("Fitness: %+18.3lf\n",dHealth);
			delete []pdChromosome;
			pdChromosome=NULL;
		}
		else
		{
			//Otherwise, redraw the world in its newly reset status
			pWorld->Draw(pDC);
		}

		//If we're not simply watching the best performing agent, occasionally save the optimisation algorithm's status
		if(ulIteration%150==0 && !nSlowMotion)
		{
			pOptimiser->Save("GA.txt");
//			pOptimiser->Save("PBIL.txt");
//			pOptimiser->Save("PS.txt");
		}
	}
	//Don't leak DCs!
	delete pDC;
}

BOOL CGAPBILExampleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleDoc serialization

void CGAPBILExampleDoc::Serialize(CArchive& ar)
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
// CGAPBILExampleDoc diagnostics

#ifdef _DEBUG
void CGAPBILExampleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGAPBILExampleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGAPBILExampleDoc commands
