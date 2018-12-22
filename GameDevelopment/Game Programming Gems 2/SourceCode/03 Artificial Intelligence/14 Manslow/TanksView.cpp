//						Tanks
//			(c) Copyright John Manslow
//					  18/04/2001

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
extern BOOL boGeneratingTrainingData;
extern CWorld *pWorld;
extern CMLP *pMLP;
extern double *pdMax,*pdMin;

double dErrorBias,dErrorVariance;
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
		Invalidate(TRUE);
	}

	//If there's no projectile in the world...
	if(!pWorld->pProjectile)
	{
		if(pWorld->nActivePlayer!=1 && boGeneratingTrainingData)
		{
			dErrorVariance=0.3;
			dErrorBias=0.3*(double(rand())/double(RAND_MAX)-0.5);
			pWorld->Initialise();
			Invalidate(TRUE);
		}
		//...and its the AI's turn to fire,
		if(pWorld->nActivePlayer==1)
		{
			//Get the size of the window
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width());
			int y = (rect.Height());
			//If we're not generating training data (i.e. this is a normal game)
			if(!boGeneratingTrainingData)
			{
				//Inside this if statement is where the AI uses the neural network to calculate
				//the angle of the tanks barrel.

				//Allocate space for the 3 neural network inputs (x-displacement between player 
				//and AI tanks, y-displacement and wind speed)
				double Inputs[3];

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
				Inputs[0]=2.0*((dxDisplacement-pdMin[0])/(pdMax[0]-pdMin[0])-0.5);
				Inputs[1]=2.0*((dyDisplacement-pdMin[1])/(pdMax[1]-pdMin[1])-0.5);
				Inputs[2]=2.0*((pWorld->dWindSpeed-pdMin[2])/(pdMax[2]-pdMin[2])-0.5);

				//Pass the inputs to the network and compute its output.
				double *pdOutputs=pMLP->pdGetOutputs(Inputs);

				//Set the inclination of the AI tank's barrel to that suggested by the neural 
				//network.
				pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination=pdOutputs[0];

				//Since the network is so good, we have to add a random deviation to the angle
				//of the tank's barrel. Without it, the neural network will nearly always
				//hit the player on its first shot.
				pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination+=dErrorBias+dErrorVariance*(double(rand())/double(RAND_MAX)-0.5);

				//Reduce the size of the deviation that will be added to the angle of the barrel
				//on the next shot. This creates the illusion of the AI tank homing in on the
				//player. The bias expresses a tendency for the AI tank to fire too short or too
				//long, whereas the variance is purely a random component.
				dErrorVariance*=0.9-0.3*double(rand())/double(RAND_MAX);
				dErrorBias*=0.5+0.2*double(rand())/double(RAND_MAX);

				//Often change the sign of the bias to create the impression of the AI
				//"bracketing" the player's tank.
				if(rand()%10<8)
				{
					dErrorBias*=-1.0;
				}

				//Clean up
				delete []pdOutputs;
			}
			else
			{
				//If we're generating training data, we don't have a neural network with which
				//to set the angle of the AI tank's barrel. As a rough estimate of the angle
				//required to score a hit, the formula for computing the angle in the absence
				//of wind, drag and when the tanks are on a level is used here. Since this often
				//misses, random noise is added to the angle so that it always (eventually) scores
				//a hit. (The multiplier in from of the asin should really be 0.5 but 0.66
				//helps to compensate for the atmospheric drag a little).
				pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination=0.66*asin(0.1*(
					-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition
					+pWorld->ppPlayer[1-pWorld->nActivePlayer]->dxPosition)/81);

				pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination+=0.25*(double(rand())/double(RAND_MAX)-0.5);

				//Occasionally add a little more random noise just in case we can't hit the
				//target due to strong wind.
				if(rand()%100<10)
				{
					pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination+=0.5*(double(rand())/double(RAND_MAX)-0.5);
				}
			}
			//Set the angle of the AI tank's barrel
			pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx=sin(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination);
			pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely=cos(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination);

			//Create a new projectile with location (roughly) at the end of the AI
			//tank's barrel, and direction given by its inclination.
			pWorld->pProjectile=new CProjectile(pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx+4,
				pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely+7,
				9*sin(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination),
				9*cos(pWorld->ppPlayer[pWorld->nActivePlayer]->dInclination));

			//Clear and redraw the contents of the window (to render the tank with the new 
			//barrel angle and remove the path of the old projectile)
			Invalidate(TRUE);
		}
	}
	else
	{
		//Redraw the contents of the window without clearing the window first.
		Invalidate(FALSE);
	}
	CView::OnTimer(nIDEvent);
}

void CTanksView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTanksDoc* pDoc = GetDocument();
	//If the active player is not the AI and a projectile does not already exist
	//in the world,
	if(pWorld->nActivePlayer!=1 && !pWorld->pProjectile)
	{
		//Get the size of the window
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width());
		int y = (rect.Height());

		//Use the location of the mouse click relative to the player's tank to
		//set the inclination of the player's tank's barrel.
		pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx=(point.x-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition)
			/sqrt(pow(point.x-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition,2.0)+pow(point.y-y+pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition,2.0));
		pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely=-(point.y-y+pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition)
			/sqrt(pow(point.x-pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition,2.0)+pow(point.y-y+pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition,2.0));

		//Create a new projectile with location (roughly) the end of the player's
		//tank's barrel, and direction given by the inclination of the barrel. The
		//constants in this call position the projectile roughly at the end of the
		//tank's barrel and give the projectile's initial speed
		pWorld->pProjectile=new CProjectile(
			pWorld->ppPlayer[pWorld->nActivePlayer]->dxPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx,
			pWorld->ppPlayer[pWorld->nActivePlayer]->dyPosition+18*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely+7,
			9*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrelx,
			9*pWorld->ppPlayer[pWorld->nActivePlayer]->dBarrely);

		//Redraw the game world with the tank's barrel in its new position
		Invalidate(TRUE);
	}

	CView::OnLButtonUp(nFlags, point);
}