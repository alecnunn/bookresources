//Tanks
//Copyright John Manslow
//29/09/2001

// TanksView.cpp : implementation of the CTanksView class
//

#include "stdafx.h"
#include "Tanks.h"

#include "TanksDoc.h"
#include "TanksView.h"

#include "CWorld.h"
#include "CProjectile.h"
#include "CTank.h"
#include "CMLP.h"
#include "CConditionalDistribution.h"
#include "CUnconditionalDistribution.h"
#include "math.h"
#include "fstream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTanksView

//Too many of these warnings for them to be useful in this file!
#pragma warning(disable:4244)

IMPLEMENT_DYNCREATE(CTanksView, CView)

BEGIN_MESSAGE_MAP(CTanksView, CView)
	//{{AFX_MSG_MAP(CTanksView)
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//See TanksDoc for explanations of these
extern BOOL boGeneratingErrorTrainingData;
extern CWorld *pWorld;
extern CMLP *pMLP;

extern CConditionalDistribution *pErrorDistribution;
extern CUnconditionalDistribution *pUnconditionalErrorDistribution;

extern double *pdMax,*pdMin;
extern double dErrorMin,dErrorMax;
extern double *pdErrorInputMin,*pdErrorInputMax;

/////////////////////////////////////////////////////////////////////////////
// CTanksView construction/destruction

CTanksView::CTanksView()
{
}

CTanksView::~CTanksView()
{
}

BOOL CTanksView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTanksView drawing

void CTanksView::OnDraw(CDC* pDC)
{
	CTanksDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//Get the size of the window. Useful for drawing the terrain at the bottom of the window and
	//centreing the wind speed arrow in it.
	CRect rect;
	GetClientRect(&rect);
	int x = (rect.Width());
	int y = (rect.Height());

	//Draw the terrain at the bottom of the window.
	unsigned long i;
	for(i=0;i<pWorld->ulTerrainResolution;i++)
	{
		pDC->MoveTo(i,y);
		pDC->LineTo(i,y-pWorld->plHeight[i]);
	}

	//The sign variable is used to reverse the direction of the wind speed arrow
	//for "negative" wind speeds - i.e. winds blowing from right to left.
	double dSign=1.0;
	if(pWorld->dWindSpeed<0) 
	{
		dSign=-1;
	}

	//Draw the centre-aligned wind speed arrow with a length proportional to the
	//wind speed
	pDC->Rectangle(0.5*x,50,15*dSign+0.5*x+100.0*pWorld->dWindSpeed,51);
	pDC->MoveTo(0.5*x,50);
	pDC->LineTo(0.5*x-10*dSign,55);
	pDC->MoveTo(0.5*x,50);
	pDC->LineTo(0.5*x-10*dSign,45);
	pDC->MoveTo(0.5*x+5*dSign+00.0*pWorld->dWindSpeed,50);
	pDC->LineTo(0.5*x-5*dSign+00.0*pWorld->dWindSpeed,55);
	pDC->MoveTo(0.5*x+5*dSign+00.0*pWorld->dWindSpeed,50);
	pDC->LineTo(0.5*x-5*dSign+00.0*pWorld->dWindSpeed,45);
	pDC->MoveTo(15*dSign+0.5*x-0*dSign+100.0*pWorld->dWindSpeed,50);
	pDC->LineTo(15*dSign+0.5*x-10*dSign+100.0*pWorld->dWindSpeed,55);
	pDC->MoveTo(15*dSign+0.5*x-0*dSign+100.0*pWorld->dWindSpeed,50);
	pDC->LineTo(15*dSign+0.5*x-10*dSign+100.0*pWorld->dWindSpeed,45);

	//If a projectile exists, draw it as a rectangle
	if(pWorld->pProjectile!=NULL)
		pDC->Rectangle(pWorld->pProjectile->dxPosition-4,y-pWorld->pProjectile->dyPosition-2,
			pWorld->pProjectile->dxPosition,y-pWorld->pProjectile->dyPosition+2);

	//All shapes drawn after this point are filled black
	pDC->SelectStockObject(BLACK_BRUSH);

	//Define the exact position for the player's tank graphic.
	long tx=long(pWorld->ppPlayer[0]->dxPosition-10.0);
	long ty=long(y-pWorld->ppPlayer[0]->dyPosition+1.0);

	//Can be used to scale the tank graphics. Should be left at 4.0 since this
	//is the length for which collision detection is calibrated.
	double dScale=4.0;

	//Draw the tank's wheels
	pDC->Ellipse(tx,ty,dScale+tx,-dScale+ty);
	pDC->Ellipse(dScale+tx,ty,2*dScale+tx,-dScale+ty);
	pDC->Ellipse(2*dScale+tx,ty,3*dScale+tx,-dScale+ty);
	pDC->Ellipse(3*dScale+tx,ty,4*dScale+tx,-dScale+ty);
	pDC->Ellipse(4*dScale+tx,-0.25*dScale+ty,5*dScale+tx,-1.25*dScale+ty);

	//Draw its hull
	pDC->Rectangle(tx,-dScale+ty,4.5*dScale+tx,-1.5*dScale+ty);
	pDC->Rectangle(tx,-1.5*dScale+ty,2*dScale+tx,-2.5*dScale+ty);
	CPoint Poly[3];
	Poly[0].x=tx+2*dScale;
	Poly[0].y=ty-1.5*dScale;
	Poly[1].x=tx+2.5*dScale;
	Poly[1].y=ty-1.5*dScale;
	Poly[2].x=tx+2*dScale;
	Poly[2].y=ty-2.5*dScale;
	pDC->Polygon(Poly,3);

	//Draw its barrel at the correct angle 
	CPoint Barrel[4];
	Barrel[0].x=tx+2.0*dScale-0.1*pWorld->ppPlayer[0]->dBarrely*dScale;;
	Barrel[0].y=ty-2.0*dScale-0.1*pWorld->ppPlayer[0]->dBarrelx*dScale;
	Barrel[1].x=tx+2.0*dScale+0.1*pWorld->ppPlayer[0]->dBarrely*dScale;;
	Barrel[1].y=ty-2.0*dScale+0.1*pWorld->ppPlayer[0]->dBarrelx*dScale;
	Barrel[2].x=tx+2.0*dScale+3.9*pWorld->ppPlayer[0]->dBarrelx*dScale;;
	Barrel[2].y=ty-2.0*dScale-4.1*pWorld->ppPlayer[0]->dBarrely*dScale;
	Barrel[3].x=tx+2.0*dScale+4.1*pWorld->ppPlayer[0]->dBarrelx*dScale;;
	Barrel[3].y=ty-2.0*dScale-3.9*pWorld->ppPlayer[0]->dBarrely*dScale;
	pDC->Polygon(Barrel,4);

	//Define exact position of AI tank
	tx=pWorld->ppPlayer[1]->dxPosition+10;
	ty=y-pWorld->ppPlayer[1]->dyPosition+1;
	
	//Render it
	pDC->Ellipse(tx,ty,-dScale+tx,-dScale+ty);
	pDC->Ellipse(-dScale+tx,ty,-2*dScale+tx,-dScale+ty);
	pDC->Ellipse(-2*dScale+tx,ty,-3*dScale+tx,-dScale+ty);
	pDC->Ellipse(-3*dScale+tx,ty,-4*dScale+tx,-dScale+ty);
	pDC->Ellipse(-4*dScale+tx,-0.25*dScale+ty,-5*dScale+tx,-1.25*dScale+ty);
	pDC->Rectangle(tx,-dScale+ty,-4.5*dScale+tx,-1.5*dScale+ty);
	pDC->Rectangle(tx,-1.5*dScale+ty,-2*dScale+tx,-2.5*dScale+ty);

	Poly[0].x=tx-2*dScale;
	Poly[0].y=ty-1.5*dScale;
	Poly[1].x=tx-2.5*dScale;
	Poly[1].y=ty-1.5*dScale;
	Poly[2].x=tx-2*dScale;
	Poly[2].y=ty-2.5*dScale;
	pDC->Polygon(Poly,3);

	Barrel[0].x=tx-2.0*dScale+0.1*pWorld->ppPlayer[1]->dBarrely*dScale;;
	Barrel[0].y=ty-2.0*dScale-0.1*pWorld->ppPlayer[1]->dBarrelx*dScale;
	Barrel[1].x=tx-2.0*dScale-0.1*pWorld->ppPlayer[1]->dBarrely*dScale;;
	Barrel[1].y=ty-2.0*dScale+0.1*pWorld->ppPlayer[1]->dBarrelx*dScale;
	Barrel[2].x=tx-2.0*dScale+3.9*pWorld->ppPlayer[1]->dBarrelx*dScale;;
	Barrel[2].y=ty-2.0*dScale-4.1*pWorld->ppPlayer[1]->dBarrely*dScale;
	Barrel[3].x=tx-2.0*dScale+4.1*pWorld->ppPlayer[1]->dBarrelx*dScale;;
	Barrel[3].y=ty-2.0*dScale-3.9*pWorld->ppPlayer[1]->dBarrely*dScale;
	pDC->Polygon(Barrel,4);

	//Prepare storage for scores
	char *pString=new char[100];

	//Dispplay the player's score in the top left of the window.
	sprintf(pString,"%0.0lf",pWorld->dScore[0]);
	pDC->TextOut(25,20,pString);

	//Display the AI's score in the top right.
	sprintf(pString,"%3.0lf",pWorld->dScore[1]);
	pDC->TextOut(715,20,pString);

	//Clean up.
	delete []pString;

	//Set the timer to generate ticks every 10 ms or so to update the game world
	SetTimer(10,10,NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CTanksView diagnostics

#ifdef _DEBUG
void CTanksView::AssertValid() const
{
	CView::AssertValid();
}

void CTanksView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTanksDoc* CTanksView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTanksDoc)));
	return (CTanksDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTanksView message handlers

void CTanksView::OnTimer(UINT nIDEvent) 
{
	CTanksDoc* pDoc = GetDocument();

	//If an event (e.g. a collision) occured in the game world, clear the window contents and
	//redraw. The TimeSptep function returns TRUE if an event occurred.
	if(pWorld->TimeStep())
	{
		//Clear the screen
		CClientDC *pDC=new CClientDC(this);
		pDC->Rectangle(0,0,1000,1000);
		delete pDC;
	}

	//Tell Windows that the window contents will need to be redraw but not to clear the window contents first
	Invalidate(FALSE);

	//If there's no projectile in the world...
	if(!pWorld->pProjectile)
	{
		//...and its the AI's turn to fire,
		if(pWorld->nActivePlayer==1)
		{
			//Get the size of the window
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width());
			int y = (rect.Height());

			//If we're not generating training data (i.e. this is a normal game)
			if(!boGeneratingErrorTrainingData)
			{
				//Inside this if statement is where the AI uses the barrel angle calculating neural network 
				//to compute the optimal barrel angle, and the unconditional and conditional distribution models
				//to add random variation.
				pWorld->ulShotNumber++;

				//Allocate space for the three barrel angle neural network inputs (x-displacement between player 
				//and AI tanks, y-displacement and wind speed)
				double dInputs[3];

				//Introduce temporary variables to store the relative displacements of the tanks
				//in the game world
				double dxDisplacement=-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dxPosition;
				double dyDisplacement=-pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dyPosition;

				//Set the network inputs to scaled versions of x-displacement, y-displacement and
				//wind speed. Scaling factors are calculated from the example data and ensure that
				//all network inputs lie roughly in the range -1 to +1. Scaling is not strictly 
				//necessary but can often reduce the time needed to train the network. You MUST be 
				//consistent with scaling: the same scaling factors that were used during training
				//MUST be used to here
				dInputs[0]=2.0*((dxDisplacement-pdMin[0])/(pdMax[0]-pdMin[0])-0.5);
				dInputs[1]=2.0*((dyDisplacement-pdMin[1])/(pdMax[1]-pdMin[1])-0.5);
				dInputs[2]=2.0*((pWorld->dWindSpeed-pdMin[2])/(pdMax[2]-pdMin[2])-0.5);

				//Pass the inputs to the network and compute its output.
				double *pdOutputs=pMLP->pdGetOutputs(dInputs);

				//Prepare to get the random aiming error
				double dAngularError=0.0;

				//This will point to the vector of bin probabilities and is only used to plot the error distribution on-screen
				double *pdErrorOutputs;

				//If this is the first shot in a new game,
				if(pWorld->ulShotNumber==1)
				{
					//Choose an aiming error by randomly sampling from the unconditional distribution
					dAngularError=pUnconditionalErrorDistribution->dGetOutputs()*(dErrorMax-dErrorMin)+dErrorMin;

					//Get the bin probabilities so we can plot them on the screen
					pdErrorOutputs=pUnconditionalErrorDistribution->pdBinProbabilities;
				}
				else
				{
					//Otherwise, choose it by sampling from the conditional distribution, which takes into account the error
					//made on the preceding shot.

					//The input to the conditional aiming error distribution is a scaled version of the aiming error made on the
					//preceding shot. Again, the scaling must be consistent with that used when the distribution was trained.
					double dErrorDistributionInput=2.0*((pWorld->dLastAngularError-dErrorMin)/(dErrorMax-dErrorMin)-0.5);

					//Get a sample from the conditional error distribution and descale it
					dAngularError=(pErrorDistribution->dGetOutputs(&dErrorDistributionInput))*(dErrorMax-dErrorMin)+dErrorMin;

					//Get the bin probabilities so we can plot them on the screen
					pdErrorOutputs=pErrorDistribution->pdGetBinProbabilities(&dErrorDistributionInput);
				}
				//This error will be the "last error" on the next turn
				pWorld->dLastAngularError=dAngularError;

				//Add the aiming error to the optimal barrel angle recommnded by the barrel angle neural network
				pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination=pdOutputs[0]+dAngularError;

				////Draw the bar-graph of the error distribution (just for curiosity)
				CClientDC *pDC=new CClientDC(this);
				pDC->Rectangle(0,0,1000,1000);
				for(unsigned long i=0;i<pErrorDistribution->ulNumberOfOutputs;i++)
				{
					pDC->Rectangle(100+i*4,250,104+i*4,250-400*pdErrorOutputs[i]);
				}
				if(pWorld->ulShotNumber!=1)
				{
					//Don't delete this in the case of the unconditional distribution because it points to
					//a member variable itself rather than a copy of it
					delete []pdErrorOutputs;
				}
				delete pDC;

				//Clean up the barrel angle neural network's outputs
				delete []pdOutputs;

				//Set the AI tank's barrel to the chosen angle
				pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx=sin(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination);
				pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely=cos(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination);

				//Create a new projectile with location (roughly) at the end of the AI
				//tank's barrel, and direction given by its inclination.
				pWorld->pProjectile=new CProjectile(pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx+4,
					pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely+7,
					9*sin(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination),
					9*cos(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination));

			}
			else
			{
				pWorld->nActivePlayer=0;
			}
		}
	}
	CView::OnTimer(nIDEvent);
}

void CTanksView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTanksDoc* pDoc = GetDocument();

	//If the active player is not the AI and a projectile does not already exist
	//in the world,
	if(pWorld->nActivePlayer==0 && !pWorld->pProjectile)
	{
		//Get the size of the window
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width());
		int y = (rect.Height());

		//Use the location of the mouse click relative to the player's tank to
		//set the inclination of the player's tank's barrel. If we're not generating error training data,
		//the player is controlling the usual (lefthand) tank. Otherwise, the player is controlling
		//the AI tank (the rightmost one)
		if(!boGeneratingErrorTrainingData)
		{
			//Work out the direction of the player tank's barrel
			pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx=(point.x-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition)
				/sqrt(pow(point.x-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition,2.0)+pow(point.y-y+pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition,2.0));
			pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely=-(point.y-y+pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition)
				/sqrt(pow(point.x-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition,2.0)+pow(point.y-y+pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition,2.0));

			//Create a new projectile
			pWorld->pProjectile=new CProjectile(
				pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx,
				pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely+6,
				9*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx,
				9*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely);
		}
		else
		{
			//Work out the angle of the AI tank's barrel (if we're generating aiming error data, the player is controlling
			//the AI tank)
			pWorld->ppPlayer[1-pWorld->nActivePlayer]->dBarrelx=(point.x-pWorld->ppPlayer[1-pWorld->nActivePlayer]->dxPosition)
				/sqrt(pow(point.x-pWorld->ppPlayer[1-pWorld->nActivePlayer]->dxPosition,2.0)+pow(point.y-y+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dyPosition,2.0));
			pWorld->ppPlayer[1-pWorld->nActivePlayer]->dBarrely=-(point.y-y+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dyPosition)
				/sqrt(pow(point.x-pWorld->ppPlayer[1-pWorld->nActivePlayer]->dxPosition,2.0)+pow(point.y-y+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dyPosition,2.0));

			//Increment the shot count
			pWorld->ulShotNumber++;

			//Make sure the AI doesn't get a turn
			pWorld->nActivePlayer=1-pWorld->nActivePlayer;

			//Create a new projectile
			pWorld->pProjectile=new CProjectile(
				pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx+2,
				pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely+7,
				9*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx,
				9*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely);

			//We need to know how the tank should have aimed in order to calculate the aiming error. To this end,
			//we now use the barrel angle neural network to calculate the optimal barrel angle:
			double Inputs[3];
			double dxDisplacement=-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dxPosition;
			double dyDisplacement=-pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dyPosition;

			//Set up the scaled inputs to the neural network (x-displacement, y-displacement, wind speed)
			Inputs[0]=2.0*((dxDisplacement-pdMin[0])/(pdMax[0]-pdMin[0])-0.5);
			Inputs[1]=2.0*((dyDisplacement-pdMin[1])/(pdMax[1]-pdMin[1])-0.5);
			Inputs[2]=2.0*((pWorld->dWindSpeed-pdMin[2])/(pdMax[2]-pdMin[2])-0.5);

			//Pass the inputs to the network and compute its output - the optimal barrel angle
			double *pdOutputs=pMLP->pdGetOutputs(Inputs);

			//Work out the angle that the player chose
			pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination=asin(pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx/sqrt(pow(pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx,2.0)+pow(pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely,2.0)));

			//Calculate the aiming error as the difference between the two. This error information will be written
			//to a data file in the world class when the projectile is destroyed (i.e. lands, goes out of range, etc.)
			pWorld->dAngularError=pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination-pdOutputs[0];

			//Clean up the barrel angle neural network's outputs
			delete []pdOutputs;
		}
		//Redraw the game world with the tank's barrel in its new position
		Invalidate(TRUE);
	}

	CView::OnLButtonUp(nFlags, point);
}