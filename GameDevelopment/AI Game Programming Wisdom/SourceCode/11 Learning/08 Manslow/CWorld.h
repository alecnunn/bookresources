//Tanks
//Copyright John Manslow
//29/09/2001

#ifndef _CWorld_
#define _CWorld_

class CTank;
class CProjectile;

class ofstream;

class CWorld
{
public:
	//Tell the world how big the terrain height array needs to be
	CWorld(const unsigned long culTerrainResolution);
	~CWorld();

	//Fills the terrain height array with randomly generated terrain
	void GenerateTerrain(void);

	//Called once per time step to check for events in the game world. Events are 
	//projectile-tank collision, projectile-landscape collision and projectile leaving
	//the world. Returns TRUE if an event occurs, FALSE otherwise.
	BOOL TimeStep(void);

	//Initialises the world object. Generates terrain, places tanks, chooses starting 
	//player, etc.
	void Initialise(void);

	//The terrain height data array
	long *plHeight;

	//The length of the terrain height array
	unsigned long ulTerrainResolution;

	//The wind speed (constant during each player's turn)
	double dWindSpeed;

	//A pointer to the array of (two) tanks in the world
	CTank **ppPlayer;

	//A pointer to the projectile in the world. NULL if no projectile exists.
	CProjectile *pProjectile;

	//A pointer to the file that we are saving example hit data to (in case we're
	//generating training data).
	ofstream *pHitData;

	//Which player is currently active. 0=human player's turn, 1=AI's turn.
	int nActivePlayer;

	//The players' scores.
	double dScore[2];

	unsigned long ulShotNumber;
	double dAngularError;
	double dLastAngularError;
};

#endif