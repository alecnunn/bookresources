//Tanks
//Copyright John Manslow
//29/09/2001

#include "stdafx.h"
#include "CWorld.h"
#include "CTank.h"
#include "CProjectile.h"
#include "assert.h"
#include "math.h"
#include "fstream.h"

//if not compiling under Windows, use #define BOOL bool, #define TRUE true, #define FALSE false, etc.
extern BOOL boGeneratingErrorTrainingData;

//Where example aiming error data will be saved to create the exemplar set with which the aiming error
//distribution models will be trained 
#define FileForExemplarData "NewExemplarData.txt"

CWorld::CWorld(const unsigned long culNewTerrainResolution)
{
	TRACE("\tConstructing world...\n");

	//If we're going to generate training data, open a file stream. If not, make sure the
	//pointer to the non-existent file stream is NULL so we don't try to delete it in the 
	//destructor
	pHitData=NULL;
	if(boGeneratingErrorTrainingData)
	{
		pHitData=new ofstream(FileForExemplarData);
		pHitData->precision(15);
	}

	//Record the resolution of the terrain and allocate memory for the terrain height array.
	ulTerrainResolution=culNewTerrainResolution;
	plHeight=new long[ulTerrainResolution];
	if(!plHeight)
	{
		TRACE("failed.\n");
		assert(FALSE);
	}

	//Declare the array of tanks objects. Only two players and one tank per player.
	ppPlayer=new CTank*[2];
	if(!ppPlayer)
	{
		TRACE("failed.\n");
		assert(FALSE);
	}
	ppPlayer[0]=new CTank();
	if(!ppPlayer[0])
	{
		TRACE("failed.\n");
		assert(FALSE);
	}
	ppPlayer[1]=new CTank();
	if(!ppPlayer[1])
	{
		TRACE("failed.\n");
		assert(FALSE);
	}
	//Initialise the tanks' barrels to point horizontally in the direction of thge enemy tank.
	ppPlayer[0]->dBarrelx=1.0;
	ppPlayer[1]->dBarrelx=-1.0;

	//Set the projectile pointer to NULL to indicate that there is currently no projectile in
	//the world. Note that only one projectile is allowed at any one time.
	pProjectile=NULL;

	//Set each player's score to zero
	dScore[0]=0.0;
	dScore[1]=0.0;

	//Create the terrain, place the tanks on it, decide who goes first, pick a random wind 
	//speed, etc. etc.
	Initialise();

	//If we get this far, we're doing well!
	TRACE("\tsuccessful.\n");
}

void CWorld::Initialise(void)
{
	//Indicate that no shots have yet been made in the current game
	ulShotNumber=0;

	//Pick a random wind speed. For simplicity, wind speed is fixed for each shot.
	dWindSpeed=2.0*(double(rand())/double(RAND_MAX)-0.5);

	//If we're generating training data, the active player (the one that has the
	//controls is always player 1 - the AI.
	if(boGeneratingErrorTrainingData)
	{
		nActivePlayer=0;
	}
	else
	{
		//Otherwise, choose whether the AI or human player goes first
		nActivePlayer=rand()%2;
	}

	//Generate some terrain for the world.
	GenerateTerrain();

	//The follwoing code searches the terrain for relatively flat areas to place
	//each tank. This improves the appearance of the game since the tank graphics 
	//aren't adjusted if a tank appears on a steep slope.
	long i,j;
	double dDeviation;
	double dMeanHeightInInterval;
	double dMinimumDeviation=1e+99;
	double dMeanHeightAtMinimumDeviation;
	unsigned long ulPositionOfMinimumDeviation=0;

	ppPlayer[0]->dyPosition=300.0;

	//Find the flattest area towards the left part of the landscape where the
	//player's tank will reside
	for(i=20;i<250;i++)
	{
		//The flat regions are identified by measuring the variance of the landscape
		//height over stretches of 20 elements in the landscape height array. Low
		//variance implies flat lanscape.
		dMeanHeightInInterval=0.0;

		//First, calculate the mean height of the landscape in the interval 
		//currently under consideration.
		for(j=-10;j<10;j++)
		{
			dMeanHeightInInterval+=double(plHeight[i+j]);
		}
		dMeanHeightInInterval/=20.0;

		//Then measure the variance of the landscape's height.
		dDeviation=0.0;
		for(j=-10;j<10;j++)
		{
			dDeviation+=(dMeanHeightInInterval-plHeight[i+j])*(dMeanHeightInInterval-plHeight[i+j]);
		}

		//Test to see if this interval is the flattest so far. If so, record its
		//location for future reference
		if(dDeviation<dMinimumDeviation)
		{
			dMinimumDeviation=dDeviation;
			ulPositionOfMinimumDeviation=i;
			dMeanHeightAtMinimumDeviation=dMeanHeightInInterval;
		}
	}

	//Put the tank in the flattest area
	ppPlayer[0]->dxPosition=ulPositionOfMinimumDeviation;
	ppPlayer[0]->dyPosition=long(dMeanHeightAtMinimumDeviation);

	//Repeat the procedure for the AI tank.
	dMinimumDeviation=1e+99;

	//Find the flattest area towards the rightmost part of the landscape where 
	//the AI's tank will reside.
	for(i=350;i<600;i++)
	{
		dMeanHeightInInterval=0.0;
		for(j=-10;j<10;j++)
		{
			dMeanHeightInInterval+=double(plHeight[i+j]);
		}
		dMeanHeightInInterval/=20.0;

		dDeviation=0.0;
		for(j=-10;j<10;j++)
		{
			dDeviation+=(dMeanHeightInInterval-plHeight[i+j])*(dMeanHeightInInterval-plHeight[i+j]);
		}

		if(dDeviation<dMinimumDeviation)
		{
			dMinimumDeviation=dDeviation;
			ulPositionOfMinimumDeviation=i;
			dMeanHeightAtMinimumDeviation=dMeanHeightInInterval;
		}
	}

	//Put the AI tank on the landscape
	ppPlayer[1]->dxPosition=ulPositionOfMinimumDeviation;
	ppPlayer[1]->dyPosition=long(dMeanHeightAtMinimumDeviation);
}

CWorld::~CWorld()
{
	//Destructor - deallocate memory.
	TRACE("\tDestroying world...\n");

	//The height data should always exist at this time but check anyway
	if(plHeight!=NULL)
	{
		delete []plHeight;
	}

	//Must check this because the data file can be closed and deleted at any time
	//by the user.
	if(pHitData!=NULL)
	{
		pHitData->close();
		delete pHitData;
	}

	//If there's a projectile in the world, delete it.
	if(pProjectile!=NULL)
	{
		delete pProjectile;
	}
	
	//Delete the players' tanks
	delete ppPlayer[0];
	delete ppPlayer[1];
	delete []ppPlayer;

	TRACE("\tsuccessful.\n");
}

void CWorld::GenerateTerrain(void)
{
	unsigned long i,p;
	//Variables to record the heights of the lowest and highest points on the
	//terrain
	long lLowest,lHighest;

	//Choose the height of the leftmost point on the terrain. The exact value 
	//doesn't really matter because we'll normalise the terrain later.
	plHeight[0]=150+rand()%100;

	//Since we only have one point on the terrain, its height must be the
	//lowest and highest so far
	lLowest=plHeight[0];
	lHighest=plHeight[0];

	//p is a variable used to control the probability that the next element in 
	//the terrain array will be higher or lower than the current one. This approach
	//produces landscapes that systematically wander up and down on a large scale
	//but also vary randomly at a smal scale.
	p=rand()%5;

	//Fill the rest of the terrain array with height values.
	for(i=1;i<ulTerrainResolution;i++)
	{
		//Occasionally, choose a new value for p to change the up/down trend
		//of the terrain
		if(rand()%50==0)
		{
			p=rand()%5;
		}

		//Use p to decide randomly whether this element is higher or lower than
		//the one to its left
		if(unsigned long(rand()%7)>p+1)
		{
			plHeight[i]=plHeight[i-1]-1;
		}
		else
		{
			plHeight[i]=plHeight[i-1]+1;
		}

		//Maintain a record of the highest and lowest points
		if(plHeight[i]<lLowest)
		{
			lLowest=plHeight[i];
		}
		if(plHeight[i]>lHighest)
		{
			lHighest=plHeight[i];
		}
	}

	//This is necessary to ensure that the scaling of the terrain doesn't produce
	//really jaggedy landscapes
	if(lHighest-lLowest<100)
	{
		lHighest=100+lLowest;
	}

	//Scale the terrain to show a minimum amount of variation.
	for(i=0;i<ulTerrainResolution;i++)
	{
		plHeight[i]=long(double((plHeight[i]-lLowest)/double(lHighest-lLowest))*100.0+100.0);
	}
}

BOOL CWorld::TimeStep(void)
{
	//First, check to see if a projectile exists in the world
	if(pProjectile)
	{
		//Check to see if the projectile has passed beyond the edge of the window and hence needs to
		//be removed
		if(	long(pProjectile->dxPosition)>750 
			|| long(pProjectile->dxPosition)<0)
		{
			if(boGeneratingErrorTrainingData)
			{
				//Save aiming error data
				*pHitData<<ulShotNumber;
				*pHitData<<"\t";
				*pHitData<<dAngularError;
				*pHitData<<"\n";
			}
			delete pProjectile;
			pProjectile=NULL;

			dWindSpeed=2.0*(double(rand())/double(RAND_MAX)-0.5);
			if(!boGeneratingErrorTrainingData)
			{
				//If we're not generating training data, its the other player's
				//turn
				nActivePlayer=1-nActivePlayer;

			}
			//Return true to indicate that an event occured in this time step.
			return TRUE;
		}

		//See if the projectile collided with the enemy tank in this time step
		if(ppPlayer[1-nActivePlayer]->nTestForProjectileTankCollision(pProjectile))
		{
			//If so and we're generating training data,
			if(boGeneratingErrorTrainingData)
			{
				//Save aiming error information
				*pHitData<<ulShotNumber;
				*pHitData<<"\t";
				*pHitData<<dAngularError;
				*pHitData<<"\n";
			}
			//Delete the projectile
			delete pProjectile;
			pProjectile=NULL;

			//Choose a new wind speed
			dWindSpeed=2.0*(double(rand())/double(RAND_MAX)-0.5);

			//Update the players' scores
			dScore[nActivePlayer]++;

			//Reinitialise the world (generate new terrain, etc.) for another
			//game.
			Initialise();

			//Return true to indicate that an event occured and the world should
			//be redrawn.
			return TRUE;
		}

		//Finally, test to see if the projectile has hit the ground.
		//Calculate the x and y movement of the projectile in this time step
		double dx=pProjectile->dxPosition-pProjectile->dPreviousxPosition;
		double dy=pProjectile->dyPosition-pProjectile->dPreviousyPosition;

		//Get the projectile's old position
		double y=pProjectile->dPreviousyPosition;
		double x=pProjectile->dPreviousxPosition;

		//Make sure we don't divide by zero later on!
		if(long(dx)==0)
		{
			dx=1;
		}

		unsigned long i;
		long j;

		//For every height array element the projectile has moved in the last 
		//time step
		for(i=0;i<unsigned long(labs(long((dx))));i++)
		{
			//If the height of the projectile is less than the height of the 
			//terrain element, the projectile has collided with the terrain
			if(y<plHeight[long(x)])
			{
				if(boGeneratingErrorTrainingData)
				{
					//Save aiming error data
					*pHitData<<ulShotNumber;
					*pHitData<<"\t";
					*pHitData<<dAngularError;
					*pHitData<<"\n";
				}
				//Delete the projectile...
				delete pProjectile;
				pProjectile=NULL;

				//Pick a new wind speed
				dWindSpeed=2.0*(double(rand())/double(RAND_MAX)-0.5);

				//...and deform the terrain to make a small crater.
				for(j=-10;j<11;j++)
				{
					double dCraterDepth=4+rand()%2;
					if(x+j>0 && x+j<700)
					{
						plHeight[long(x)+j]-=long(dCraterDepth*exp(-0.02*pow(j,2.0))+rand()%2);
					}
				}

				//If we're not generating training data,
				dWindSpeed=2.0*(double(rand())/double(RAND_MAX)-0.5);
				if(!boGeneratingErrorTrainingData)
				{
					//Pick a new wind speed and switch to the other player
					nActivePlayer=1-nActivePlayer;
				}
	
				//Return true to indicate that an event has occurred and the
				//display should be redrawn.
				return TRUE;
			}

			//Move to the next terrain array element to test for a collision
			x+=long(dx)/labs(long(dx));

			//Compute the projectile's height aboe that element.
			y+=dy/fabs(dx);
		}

		//Tell the projectile that a time step has occurred.
		pProjectile->TimeStep(dWindSpeed);
	}

	//If we get this far, nothing significant has happened in this time step.
	//Return false to indicate this.
	return FALSE;
}