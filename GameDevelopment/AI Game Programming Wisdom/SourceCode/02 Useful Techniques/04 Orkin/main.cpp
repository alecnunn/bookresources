#include "AnimController.h"

void main()
{
	//
	// ---- BEGIN STUB CODE -------
	//

	// Anim files should really be read from external data files, 
	// probably exported from 3D Studio Max or Maya.
	// Hardcode a few for demo purposes.

	AnimFileStruct animIdle;
	strcpy(animIdle.szFileName, "idle.anm");
	animIdle.eCategory = kACat_FullBody;
	animIdle.nPriority = 0;

	AnimFileStruct animWalk;
	strcpy(animWalk.szFileName, "walk.anm");
	animWalk.eCategory = kACat_FullBody;
	animWalk.nPriority = 1;

	AnimFileStruct animFidget;
	strcpy(animFidget.szFileName, "fidget.anm");
	animFidget.eCategory = kACat_LeftArm;
	animFidget.nPriority = 2;

	AnimFileStruct animAttack;
	strcpy(animAttack.szFileName, "attack.anm");
	animAttack.eCategory = kACat_UpperBody;
	animAttack.nPriority = 5;

	//
	// ---- END STUB CODE -------
	//

	
	// This demonstrates how the controller decides which
	// animations should be playing at any instant.
	// The Print() function displays the currently playing
	// animations in the order they would be processing
	// the skeleton.

	CAnimController animCtrl;

	animCtrl.PlayAnim(&animIdle);
	animCtrl.Print();

	animCtrl.PlayAnim(&animWalk);
	animCtrl.Print();

	animCtrl.PlayAnim(&animFidget);
	animCtrl.Print();

	animCtrl.PlayAnim(&animAttack);
	animCtrl.Print();
}
