/*
  FingersOfDeath.cpp

  This file contains the algorithms used and described 
  in the Game Programming Gems 4 Chapter "The 10 Fingers of Death"
  
  Code written by Dr. Roger Smith and Donald Stoner
	rdsmith@titan.com and dstoner@titan.com

  NOTE:The algorithms in this Gem are coded for clarity and understanding by the
  reader.  Programmers who adopt them should streamline them for runtime
  efficiency.
*/

#include "FingersOfDeath.h"
#include "stdafx.h"

//Finger #1 "Hitting a Ribbon"
/*The first finger of death presents a simple algorithm for determining whether a 
shooter will hit a ribbon target like a road, long convoy of vehicles, or a long 
bridge.  The probability of hitting such a target is dependent only upon the width 
of the target and the standard deviation in the shot pattern of the shooter.  The 
assumption is that the target is so long that it is not possible to overshoot or 
undershoot it in length.*/
//
//function parameters:
//double dTargetWidth		The width of the target being attacked
//double dBulletDispersion	The value is the standard devision of the normal distribution of the bullets.
double HittingARibbon(double dTargetWidth, double dBulletDispersion)
{
	//Initializing the Probability of Hit variable
	double dProbabilityOfHit = 0.0;

	//Probability of Hit calculation
	dProbabilityOfHit = ((2 * dTargetWidth) / (sqrt(2 * PI * dBulletDispersion)));

	return dProbabilityOfHit;
}

//Finger #2 "Hitting the Bullseye"
/*The second finger of death describes the simple math and probability of hitting 
a round target that you are aiming at.  This algorithm can be used to determine the 
accuracy of an AI bot that has a defined shot pattern distribution.  It is based on 
the fact that all shooters, human and machine alike, have built-in variations in 
every shot fired.  These variations come from the quality of the weapon, the 
construction of the bullets, steadiness of the shooter, and wind conditions.*/
//
//function parameters:
//double dRadius			The radius of the target being attacked
//double dBulletDispersion	The value is the standard devision of the normal distribution of the bullets.
double HittingTheBullseye(double dRadius, double dBulletDispersion)
{
	//Initializing the Probability of Hit variable
	double dProbabilityOfHit = 0.0;

	//Probability of Hit calculation
	dProbabilityOfHit = 1.0 - exp(-((pow(dRadius,2)) / (2 * pow(dBulletDispersion,2))));

	return dProbabilityOfHit;
}

//Finger #3 "Hitting a Rectangle"
/*Most targets are not shaped like bullseyes, so we need a more flexible algorithm 
to shoot with.  If the target is rectangular like a vehicle or a group of rectangles 
like the torso, there is a squared off version of the previous equation that can be 
used.*/
//
//function parameters:
//double dLength			The length of the target being attacked
//double dWidth				The width of the target being attacked
//double dBulletDispersionX	The X value of the standard devision of the normal distribution of the bullets.
//double dBulletDispersionY	The Y value of the standard devision of the normal distribution of the bullets.
double HittingARectangle(double dLength, double dWidth, double dBulletDispersionX, double dBulletDispersionY)
{
	//Initializing the Probability of Hit variables
	double dProbabilityOfHit = 0.0;
	double dPHitX = 0.0;
	double dPHitY = 0.0;

	//Determines the probability of hit along the x axis
	dPHitX = 1.0 - exp(-((2 * pow(dLength,2)) / (PI * pow(dBulletDispersionX,2))));

	//Determines the probability of hit along the y axis
	dPHitY = 1.0 - exp(-((2 * pow(dWidth,2)) / (PI * pow(dBulletDispersionY,2))));

	//Determines the overall probability of hit
	dProbabilityOfHit = sqrt(dPHitX * dPHitY);

	return dProbabilityOfHit;
}

//Finger #4 "Shotgunning a Small Target"
/*Some weapons unleash a barrage of rockets, bomblets, or explosive munitions all at 
once in an attempt to totally overwhelm the target and blow it to smithereens instantly.  
When this happens, there are much faster ways of determining the killing effect than 
calculating the impact point and lethality of each rocket separately.  When this type 
of attack is directed at the player and explodes all around him, one calculation can be 
done to determine whether one or more of the munitions hit the target. */
//
//function parameters:
//int nNumberOfRounds		The number of rounds fired at the target
//float fLethalArea			The lethal area of each round
//double dBulletDispersion	The value is the standard devision of the normal distribution of the bullets.
double ShotgunningASmallTarget(int nNumberOfRounds, float fLethalArea, double dBulletDispersion)
{
	//Initializing the Probability of Hit variable
	double dProbabilityOfKill = 0.0;

	//Probability of Hit calculation
	dProbabilityOfKill = (1.0 - exp(-((nNumberOfRounds * fLethalArea)/(2 * PI * pow(dBulletDispersion,2)))));

	return dProbabilityOfKill;
}

//Finger #5 "Death by Walking Artillery"
/*Artillery rounds are often adjusted by a spotting team that radios corrections back 
to the firing battery and allows them to place the next round closer to the target.  
When this occurs, the lethality of the barrage is higher than the previous simultaneous 
barrage method.  The lethality of this adjusted fire, or walking artillery, is calculated 
through a summation series in the exponent.*/
//
//function parameters:
//int nNumberOfRounds		The number of rounds fired at the target
//float fLethalArea			The lethal area of each round
//double dBulletDispersion	The value is the standard devision of the normal distribution of the bullets.
double DeathByWalkingArtillery(int nNumberOfRounds, float fLethalArea, double dBulletDispersion)
{
	//Initializing the Probability of Hit variable
	double dProbabilityOfKill = 0.0;
	double dSumOfRounds = 0.0;

	//Summation of the rounds to calculate lethality
	for(int i = 2 ; i <= nNumberOfRounds ; i++)
	{
		dSumOfRounds += ((double)i-1)/(double)i;
	}

	//Probability of Hit calculation
	dProbabilityOfKill = (1.0 - exp(-((pow(fLethalArea,2)/(2 * pow(dBulletDispersion,2))) * dSumOfRounds)));

	return dProbabilityOfKill;
}

//Finger #6 "Kills Come in Four Flavors"
/*To quote a famous pig, "all kills are not equal, some are more equal than others". Military 
simulations usually represent four different types of kills that are most often found in real-world 
combat. The first flavor is a mobility kill in which the target is no longer able to move, but 
remains alive enough to fire its weapon or communicate with other vehicles.  The second is a 
firepower kill in which the weapon is damaged, but the vehicle or person is still able to move.  
The third is a mobility and firepower kill in which the vehicle or person is still alive, but cannot 
move or use its weapon.  This target may still be able to observe enemy operations, communicate, and 
consume supplies.  In some simulations, this type of damage may also motivate a rescue operation.  
The final kill type is the catastrophic kill, often pictured as an aircraft exploding into a million 
pieces, a flaming tank turret spinning through the air, or a person being turned into fresh chunks 
of meat. */
//
//function parameters:
//double dProbabilityOfMobilityOrFirePowerKill		The probability of receiving a mobility or fire power kill
Killtype KillsComeInFourFlavors(double dProbabilityOfMobilityOrFirePowerKill)
{
	//Killtype enumeration to return what killtype was made
	Killtype eKillType = nokill;

	//calculations of the various killtypes to determine random kill factor
	double dProbabilityOfNoKill = 1.0 - dProbabilityOfMobilityOrFirePowerKill;
	double dProbabilityOfCatastrophicKill = 0.5 * dProbabilityOfMobilityOrFirePowerKill;
	double dProbabilityOfMobilityKill = dProbabilityOfMobilityOrFirePowerKill - 0.9 * dProbabilityOfMobilityOrFirePowerKill;
	double dProbabilityOfFirePowerKill = dProbabilityOfMobilityOrFirePowerKill - 0.9 * dProbabilityOfMobilityOrFirePowerKill;
	double dProbabilityOfMobilityAndFirePowerKill = dProbabilityOfMobilityOrFirePowerKill - dProbabilityOfMobilityKill - dProbabilityOfFirePowerKill - dProbabilityOfCatastrophicKill;

	//seeds the random number generator with the current time.
	//This helps to ensure different random numbers each cycle
	srand( (unsigned)time( NULL ) );

	//Generates a random number to determine what type of kill was made
	double dRandomValue = ((rand() % 100) * .01);

	//Statement to handle No Kill
	if(dRandomValue < dProbabilityOfNoKill)
	{
		eKillType = nokill;
	}
	//Statement to handle Mobility Kill
	else if(dRandomValue < (dProbabilityOfNoKill + dProbabilityOfMobilityKill))
	{
		eKillType = mobility;
	}
	//Statement to handle Firepower Kill
	else if(dRandomValue < (dProbabilityOfNoKill + dProbabilityOfMobilityKill + dProbabilityOfFirePowerKill))
	{
		eKillType = firepower;
	}
	//Statement to handle Mobility and Firepower Kill
	else if(dRandomValue < (dProbabilityOfNoKill + dProbabilityOfMobilityKill + dProbabilityOfFirePowerKill + dProbabilityOfMobilityAndFirePowerKill))
	{
		eKillType = mobilityandfirepower;
	}
	//Statement to handle Catastrophic Kill
	else
	{
		eKillType = catastrophic;
	}	

	return eKillType;
}

//Finger #7 "Chemicals, Fireballs, and Area Magic"
/*No one is more interested in the effects of gaseous area weapons than the military.  So it should 
be no surprise that many models have been created to determine when a vehicle or human is within the 
lethal area of such a weapon.  The following simple algorithm calculates the probability of a kill based 
on the volume of chemical released and the distance that the release occurs from the target.  For games, 
this algorithm could be used for damage from an expanding fireball, area magic, or any other exotic and 
evil form of attack. */
//
//function parameters:
//int nNumberOfRounds			The number of rounds fired at the target
//double dWeightOfChemical		The weight of the chemical being dispersed
//double dDistanceFromTheTarget	The distance of the impact from the target
double AreaEffectWeapons(int nNumberOfRounds, double dWeightOfChemical, double dDistanceFromTheTarget)
{
	//Initializing the Probability of Hit variable
	double dProbabilityOfKill = 0.0;

	//The Equation is broken down into 3 sub equations
	double dEquation1Results = pow((nNumberOfRounds * dWeightOfChemical),.333)/sqrt(2 * PI);
	double dEquation2Results = .00134625 * (pow(dDistanceFromTheTarget,2)/(nNumberOfRounds * dWeightOfChemical));
	double dEquation3Results = exp(-0.5*(pow(dEquation2Results,2)));

	//Final Probability of Kill calculation
	dProbabilityOfKill = dEquation1Results * dEquation3Results;

	return dProbabilityOfKill;
}

//Finger #8 "The Shrapnel Wedge"
/*When an aircraft is shot down with a missile it is seldom accomplished by the missile flying directly 
into the aircraft.  More often, the missile reaches a "point of closest approach" and explodes near the 
aircraft.  The shrapnel from the missile then spreads out in a donut shaped pattern from the point of 
explosion and hopefully, the aircraft is caught in that shrapnel pattern and destroyed.  There are many 
variations on the probability of killing a aircraft in this manner that include the speed of both objects, 
their closing velocity, and the size of the target.  The algorithm shown here is a relatively simple version 
that should be applicable to many types of exploding projectiles, fireballs, and magic targeted at aircraft, 
dragons, and spacebugs.   */
//
//function parameters:
//int nNumberOfProjectiles		The number of projectiles fired at the target
//double dVulnerableArea		The weight of the chemical being dispersed
//double dRangeToTarget			The distance of the impact from the target
//double dAngleToNearEdgeOFTarget	The angle to the closest edge of the target from the projectile
//double dAngleToFarEdgeOfTarget	The angle to the farthest edge of the target from the projectile
double ShrapnelWedge(int nNumberOfProjectiles, double dVulnerableArea, double dRangeToTarget, double dAngleToNearEdgeOfTarget, double dAngleToFarEdgeOfTarget)
{
	//Initializing the Probability of Hit variable
	double dProbabilityOfKill = 0.0;

	//Probability of Kill calculation
	dProbabilityOfKill = (1.0 - exp(-((nNumberOfProjectiles * dVulnerableArea)/(2*PI*pow(dRangeToTarget,2)*(cos(dAngleToNearEdgeOfTarget)-cos(dAngleToFarEdgeOfTarget))))));

	return dProbabilityOfKill;
}

//Finger #9 "Beating the Bushes"
/*Some engagements involve teams of hunters searching the terrain or bushes for hidden prey.  When a large 
group of hunters is looking for a large group of prey, it is possible to model the capture or kill of the 
prey in an aggregate form, rather than representing the individual movement and line-of-sight of every hunter 
and every prey. As before, this approach is very valuable when the hunting and killing is being conducted by 
AI controlled hunters and especially when it is happening off the player's screen. The algorithm is structured 
to calculate the change in the population of the prey based on the number and efficiency of the hunters.  It 
also accounts for different types of prey and hunter animals - e.g. small rodents, medium-sized wolves, and 
large elephants.  If all hunters and prey are of the same type, there is no need to use this additional 
flexibility. */
//
//NOTE: This function must be run on each type of prey to determine total number of prey killed.  The function
//only returns the number killed of the given type.
//
//function parameters:
//int nNumberOfTypePrey			The number of prey of a given type
//double dHardnessOfPrey		The hardness, or ability to elude the hunters, of the given prey
//int nTotalNumOfPrey			The total number of prey in the search area
//std::vector<double> vProbabilityOfDetectionByHunterType	The probability of each type of hunter to detect the given prey type
//std::vector<int> vNumberOfHuntersByType)					The number of hunters by each type
double BeatingTheBushes(int nNumberOfTypePrey, double dHardnessOfPrey, int nTotalNumOfPrey, std::vector<double> vProbabilityOfDetectionByHunterType, std::vector<int> vNumberOfHuntersByType)
{
	//Variable initialization
	double dNumberKilled = 0.0;
	double dTotalProbabilityOfHunterDetection = 0.0;

	//Summation calculation for total probability of hunter detection of given prey type
	for(int i = 0 ; i < vNumberOfHuntersByType.size() ; i++)
	{
		dTotalProbabilityOfHunterDetection = vProbabilityOfDetectionByHunterType.at(i) * vNumberOfHuntersByType.at(i);
	}

	//Calculation of total number of prey of given type killed
	dNumberKilled = (nNumberOfTypePrey * (1.0 - exp(-((dHardnessOfPrey/nTotalNumOfPrey) * (dTotalProbabilityOfHunterDetection)))));

	return dNumberKilled;
}

//Finger #10 "Beating the Bushes with Prey Spacing"
/*Mathematicians and analysts noticed that the Beating the Bushes algorithm did not account for differences in the 
density of prey hiding in the bushes.  It is clearly much easier to find and kill prey when there are a hundred of 
them in the search area than if there are just two or three.  Therefore, they created a variation known as the 
Lulejian model in which the spacing between the prey is an important factor. The visual picture for this algorithm 
is the same as that above, but the mathematics differ to account for the spacing of prey. The definition of kj also 
varies slightly in that Lilejian defines kj as the average destruction of the hunters on prey type j.  */
//
//NOTE: This function must be run on each type of prey to determine total number of prey killed.  The function
//only returns the number killed of the given type.
//
//function parameters:
//int nNumberOfTypePrey			The number of prey of a given type
//double dAverageSpacing		The average spacing of prey within the search area
//int nTotalNumOfPrey			The total number of prey in the search area
//std::vector<double> vAverageDestructionOfPreyByType	The probability of the given prey type being destroyed by each hunter type
//std::vector<int> vNumberOfHuntersByType)				The number of hunters by each type
double BeatingTheBushesWithPreySpacing(int nNumberOfTypePrey, double dAverageSpacing, int nTotalNumOfPrey, std::vector<double> vAverageDestructionOfPreyByType, std::vector<int> vNumberOfHuntersByType)
{
	//Variable Initialization
	double dNumberKilled = 0.0;
	double dTotalProbabilityOfPreyDestruction = 0.0;

	//Summation calculation for total probability of given prey destruction by hunters of all types
	for(int i = 0 ; i < vNumberOfHuntersByType.size() ; i++)
	{
		dTotalProbabilityOfPreyDestruction = vAverageDestructionOfPreyByType.at(i) * vNumberOfHuntersByType.at(i);
	}

	//Calculation of the total number of prey of given type killed
	dNumberKilled = (nNumberOfTypePrey * (1.0 - exp(-((dTotalProbabilityOfPreyDestruction)/(dAverageSpacing * nTotalNumOfPrey)))));

	return dNumberKilled;
}

