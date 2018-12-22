//main.cpp
//lev2 - Random creation of 3D level geometry through MAXScript code generation.

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "box.h"
#include "boxroom.h"
#include "cylinder.h"
#include "gengon.h"
#include "sphere.h"
using namespace std;


//Global variable declarations
// The convention is +X is Forward, +Y is Left and +Z is Up
enum Direction {Forward, Backward, Up, Down, Left, Right};

//Global function declarations
double randBetween(double, double);
Room* getRandomShape();
Direction getRandomDirection();
void positionRoom(Room* prev, Room* curr, Direction myDirection);
bool testForIntersection (Box* Box1, Box* Box2);


//Main routine begins here
int main ()
{
	srand(time(0));							//seed the random number generator

	ofstream fout;							//create file stream object
	fout.open ("c:\\level.ms");				//open file
											//note the file name and location!!!


	//Declare a matrix of pointers to Rooms and initialize to 0
	Room* myRoom[NUMROOMS] = {0};

	


	//---------------------------------------------------------------------------------
	// The first two rooms are placed manually - this makes the tests in the main loop 
	// somewhat simpler.
	//---------------------------------------------------------------------------------
	//Place first room at 0,0,0
	myRoom[0] = getRandomShape();
	//Write the first Room to the script file
	fout << "Room" << "0 = " <<  myRoom[0]->createString();
	fout << "Room" << 0 << ".name = \"Room" << 0 << "\"" << endl;

	//testing only - showing inner box
	//fout << "Inner" << 0 << " = " << myRoom[0]->getInscribedBox()->createString();
	//fout << "Inner" << 0 << ".name = \"Inner" << 0 << "\"" << endl;

	//Place second room
	myRoom[1] = getRandomShape();
	//Write the second room to the script file
	//Position room in one of six random, orthogonal directions
	positionRoom(myRoom[0], myRoom[1], getRandomDirection());

	fout << "Room" << "1 = " << myRoom[1]->createString();
	fout << "Room" << 1 << ".name = \"Room" << 1 << "\"" << endl;
	//testing only - showing inner box
//	fout << "Inner" << 1 << " = " << myRoom[1]->getInscribedBox()->createString();
	//fout << "Inner" << 1 << ".name = \"Inner" << 1 << "\"" << endl;
	
	//---------------------------------------------------------------------------------
	//Start "create and place room" loop with the third room
	//---------------------------------------------------------------------------------
	
	//i is the index of the room that is currently being created
	int i = 2;

	//j is the room that is being checked for intersection.
	int j = i - 2;

	//Set up boolean flag for testing intersection
	bool isOK = true;

	while (i < NUMROOMS)
	{
		//Create a room
		myRoom[i] = getRandomShape();

		//Position room in one of six random, orthogonal directions
		positionRoom(myRoom[i-1], myRoom[i], getRandomDirection());

		//compare i&j
		for (j=i-2 ; j>=0 ; j--)
		{
			isOK = testForIntersection(myRoom[i]->getBoundingBox(),myRoom[j]->getBoundingBox());
			if (isOK == false)
				break;

		}
		
		if (isOK == false)
		{
			continue;		//skip the rest of the while loop and re-create this room
		}
		
		//this room has been successful!! - go to the next room

		//Create string for new room
		fout << "Room" << i << " = " << myRoom[i]->createString();
		fout << "Room" << i << ".name = \"Room" << i << "\"" << endl;
		
		//testing only - showing inner box
		//fout << "Inner" << i << " = " << myRoom[i]->getInscribedBox()->createString();
		//fout << "Inner" << i << ".name = \"Inner" << i << "\"" << endl;

		i++;	
		
	}

	//hollow out the entire level
	for (i = 0; i<NUMROOMS; i++)
	{
		fout << "HollowRoom" << i << " = " << myRoom[i]->createHollowingString();
		fout << "Room" << i << " - " << "HollowRoom" << i << endl;

		if (i > 1)
			fout << "Room" << i-1 << " - " << "HollowRoom" << i << endl;
		
		if (i < NUMROOMS-1)
			fout << "Room" << i+1 << " - " << "HollowRoom" << i << endl;

		fout << "delete HollowRoom" << i << endl;
	}
	


	//clean-up
	for (i = 0; i < NUMROOMS ; i++)
	{
		delete myRoom[i];
	}
	
		
	fout.close();										//close file
	
	return 0;
};







//Global function definitions
double randBetween(double min, double max)
{
	//returns a random number between the max and the min  (including the max and the min)
	//note that this is inclusive of the min and the max
	double myRand = (min + (rand() % (static_cast<int>(max-min+1))));
	
	return myRand;
};



Room* getRandomShape()
{
	int f = (rand() % 4);
	
	Room* theRoom;

	enum Shapes {aBoxRoom, aSphere, aCylinder, aGengon};

	switch (f)
	{

	case aBoxRoom:
		{
			theRoom = new BoxRoom(randBetween(BOXROOM_LENGTH_MIN,BOXROOM_LENGTH_MAX),
								  randBetween(BOXROOM_WIDTH_MIN,BOXROOM_WIDTH_MAX),
								  randBetween(BOXROOM_HEIGHT_MIN,BOXROOM_HEIGHT_MAX),
								  0,0,0);
		}
		break;

	case aSphere:
		{
			theRoom = new Sphere (randBetween(SPHERE_RADIUS_MIN,SPHERE_RADIUS_MAX),
								  0, 0, 0);
		}
		break;

	case aCylinder:
		{
			theRoom = new Cylinder(randBetween(CYLINDER_SIDES_MIN,CYLINDER_SIDES_MAX),
								   randBetween(CYLINDER_HEIGHT_MIN,CYLINDER_HEIGHT_MAX),
								   randBetween(CYLINDER_RADIUS_MIN,CYLINDER_RADIUS_MAX),
								   0,0,0);
		}
		break;
		
	case aGengon:
		{
			theRoom = new Gengon(randBetween(GENGON_SIDES_MIN,GENGON_SIDES_MAX),
								 randBetween(GENGON_RADIUS_MIN, GENGON_RADIUS_MAX),
								 randBetween(GENGON_FILLET_MIN,GENGON_FILLET_MAX),
								 randBetween(GENGON_HEIGHT_MIN,GENGON_HEIGHT_MAX),
								 0,0,0);
		}
		break;
		
	default:
		{
			theRoom = new BoxRoom (1,1,1,0,0,0);
		}
	}

	return theRoom;

};

Direction getRandomDirection()
{
	int f = rand() % 6;
	return static_cast<Direction>(f);
};

void positionRoom(Room* prev, Room* curr, Direction myDirection)
{
	
	// The convention is +X is Forward, +Y is Left and +Z is Up
	//Also for a box length is Y, width is X and height is Z
	
	double temp;		// this variable is just used to make the calculations
						// a bit simpler.

	switch (myDirection)  
	{
	case Forward:
		{
			
			//Position in the X direction
			//Set the current x position at previous position + half the width of
			// the previous position
			curr->setPosX(prev->getPosX() + 
				         (prev->getInscribedBox()->getWidth()/2) +
						 (curr->getInscribedBox()->getWidth()/2));

			//Position in the Y direction
			//Set the y position at previous y + or -  prev length /2 
			//Create temporary variable holding Previous length of inscribed box divided by 2. 
			temp = prev->getInscribedBox()->getLength()/2;
			curr->setPosY(prev->getInscribedBox()->getPosY() + randBetween(-temp,temp));
	
			//Position in the Z direction			  
			//Set the z position at previous z + or -  prev height /2 
			// Create temporary variable holding previous height of inscribed box devided by 2.
			temp = prev->getInscribedBox()->getHeight()/2;
			curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				          temp - curr->getInscribedBox()->getHeight()/2 +
						  randBetween(-temp, temp));
			
			//If this is a sphere, you need to do an offset - since the SetPosZ - sets the position of the
			//room not the inscribed box
			if (curr->createString().at(0) == 'S')  //checks if it is a Sphere
			{
				curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				              temp - curr->getInscribedBox()->getHeight()/2 +
						      randBetween(-temp, temp)
						      - curr->getInscribedBox()->getHeight()*.366);
			}
			
			
		}
		break;

	case Backward:
		{
			//X direction
			curr->setPosX(prev->getPosX() -
				         (prev->getInscribedBox()->getWidth()/2) -
						 (curr->getInscribedBox()->getWidth()/2));

			//Y direction
			temp = prev->getInscribedBox()->getLength()/2;
			curr->setPosY(prev->getInscribedBox()->getPosY() + randBetween(-temp,temp));

			//Z direction
			temp = prev->getInscribedBox()->getHeight()/2;
			curr->setPosZ(prev->getInscribedBox()->getPosZ() +
					      temp - curr->getInscribedBox()->getHeight()/2 + 
						  randBetween(-temp,temp));


			//If this is a sphere, you need to do an offset - since the SetPosZ - sets the position of the
			//room not the inscribed box
			if (curr->createString().at(0) == 'S')  //checks if it is a Sphere
			{
				cout << "YES" << endl;
				curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				              temp - curr->getInscribedBox()->getHeight()/2 +
						      randBetween(-temp, temp)
						      - curr->getInscribedBox()->getHeight()*.366);
			}
		}
		break;

	case Up:
		{
			//X direction
			temp = prev->getInscribedBox()->getWidth()/2;
			curr->setPosX(prev->getInscribedBox()->getPosX() + randBetween(-temp, temp));

			//Y direction
			temp = prev->getInscribedBox()->getLength()/2;
			curr->setPosY(prev->getInscribedBox()->getPosY() + randBetween(-temp, temp));

			//Z direction
			curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				          prev->getInscribedBox()->getHeight()-STDWALL);
			
			//If this is a sphere, you need to do an offset - since the SetPosZ - sets the position of the
			//room not the inscribed box
			if (curr->createString().at(0) == 'S')  //checks if it is a Sphere
			{
				cout << "YES" << endl;
				curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				              prev->getInscribedBox()->getHeight() -
							  curr->getInscribedBox()->getHeight()*.366);

			}

		}
		break;

	case Down:
		{
			//X direction
			temp = prev->getInscribedBox()->getWidth()/2;
			curr->setPosX(prev->getInscribedBox()->getPosX() + randBetween(-temp, temp));

			//Y direction
			temp = prev->getInscribedBox()->getLength()/2;
			curr->setPosY(prev->getInscribedBox()->getPosY() + randBetween(-temp, temp));

			//Z direction
			curr->setPosZ(prev->getInscribedBox()->getPosZ() -
				          curr->getInscribedBox()->getHeight() - STDWALL);

			//If this is a sphere, you need to do an offset - since the SetPosZ - sets the position of the
			//room not the inscribed box
			if (curr->createString().at(0) == 'S')  //checks if it is a Sphere
			{
				cout << "YES" << endl;
				curr->setPosZ(prev->getInscribedBox()->getPosZ() -
				              curr->getInscribedBox()->getHeight() -
							  curr->getInscribedBox()->getHeight()*.366);

			}

		}
		break;

	case Left:
		{
			//X direction
			temp = prev->getInscribedBox()->getWidth()/2;
			curr->setPosX(prev->getInscribedBox()->getPosX() + randBetween(-temp, temp));

			//Y direction
			curr->setPosY(prev->getPosY() + 
						  prev->getInscribedBox()->getLength()/2 +
						  curr->getInscribedBox()->getLength()/2);

			//Z direction
			temp = prev->getInscribedBox()->getHeight()/2;
			curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				          temp - curr->getInscribedBox()->getHeight()/2 +
						  randBetween(-temp, temp));

			//If this is a sphere, you need to do an offset - since the SetPosZ - sets the position of the
			//room not the inscribed box
			if (curr->createString().at(0) == 'S')  //checks if it is a Sphere
			{
				cout << "YES" << endl;
				curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				              temp - curr->getInscribedBox()->getHeight()/2 +
						      randBetween(-temp, temp)
						      - curr->getInscribedBox()->getHeight()*.366);
			}
		}
		break;

	case Right:
		{
			//X direction
			temp = prev->getInscribedBox()->getWidth()/2;
			curr->setPosX(prev->getInscribedBox()->getPosX() + randBetween(-temp, temp));

			//Y direction
			curr->setPosY(prev->getInscribedBox()->getPosY() -
						  prev->getInscribedBox()->getLength()/2 -
						  curr->getInscribedBox()->getLength()/2);

			//Z direction
			temp = prev->getInscribedBox()->getHeight()/2;
			curr->setPosZ(prev->getInscribedBox()->getPosZ() + 
				          temp - curr->getInscribedBox()->getHeight()/2 + 
						  randBetween(-temp, temp));

			//If this is a sphere, you need to do an offset - since the SetPosZ - sets the position of the
			//room not the inscribed box
			if (curr->createString().at(0) == 'S')  //checks if it is a Sphere
			{
				cout << "YES" << endl;
				curr->setPosZ(prev->getInscribedBox()->getPosZ() +
				              temp - curr->getInscribedBox()->getHeight()/2 +
						      randBetween(-temp, temp)
						      - curr->getInscribedBox()->getHeight()*.366);
			}
		}
		break;

	default:
		{
			cout << "Why did you hit the default case?";
		}
		break;
	}
};


// This function is used to determine if two boxes intersect one another.

bool testForIntersection (Box* Box1, Box* Box2)
{

	
	double Box1Point[8][3]  = 
	{{Box1->getPosX()-Box1->getWidth()/2,	Box1->getPosY()-Box1->getLength()/2,	Box1->getPosZ()},
	{Box1->getPosX()-Box1->getWidth()/2,	Box1->getPosY()-Box1->getLength()/2,	Box1->getPosZ()+Box1->getHeight()},
	{Box1->getPosX()+Box1->getWidth()/2,	Box1->getPosY()-Box1->getLength()/2,	Box1->getPosZ()+Box1->getHeight()},
	{Box1->getPosX()+Box1->getWidth()/2,	Box1->getPosY()-Box1->getLength()/2,	Box1->getPosZ()},
	{Box1->getPosX()-Box1->getWidth()/2,	Box1->getPosY()+Box1->getLength()/2,	Box1->getPosZ()},
	{Box1->getPosX()-Box1->getWidth()/2,	Box1->getPosY()+Box1->getLength()/2,	Box1->getPosZ()+Box1->getHeight()},
	{Box1->getPosX()+Box1->getWidth()/2,	Box1->getPosY()+Box1->getLength()/2,	Box1->getPosZ()+Box1->getHeight()},
	{Box1->getPosX()+Box1->getWidth()/2,	Box1->getPosY()+Box1->getLength()/2,	Box1->getPosZ()}};


	double Box2Point[8][3]  = 
	{{Box2->getPosX()-Box2->getWidth()/2,	Box2->getPosY()-Box2->getLength()/2,	Box2->getPosZ()},
	{Box2->getPosX()-Box2->getWidth()/2,	Box2->getPosY()-Box2->getLength()/2,	Box2->getPosZ()+Box2->getHeight()},
	{Box2->getPosX()+Box2->getWidth()/2,	Box2->getPosY()-Box2->getLength()/2,	Box2->getPosZ()+Box2->getHeight()},
	{Box2->getPosX()+Box2->getWidth()/2,	Box2->getPosY()-Box2->getLength()/2,	Box2->getPosZ()},
	{Box2->getPosX()-Box2->getWidth()/2,	Box2->getPosY()+Box2->getLength()/2,	Box2->getPosZ()},
	{Box2->getPosX()-Box2->getWidth()/2,	Box2->getPosY()+Box2->getLength()/2,	Box2->getPosZ()+Box2->getHeight()},
	{Box2->getPosX()+Box2->getWidth()/2,	Box2->getPosY()+Box2->getLength()/2,	Box2->getPosZ()+Box2->getHeight()},
	{Box2->getPosX()+Box2->getWidth()/2,	Box2->getPosY()+Box2->getLength()/2,	Box2->getPosZ()}};
	

	//First loop - check that no points in Box1 are inside of Box2
	for (int i = 0; i <8; i++)
	{
		if ((Box1Point[i][0] > Box2Point[0][0]) && (Box1Point[i][0] < Box2Point[6][0]) &&
			(Box1Point[i][1] > Box2Point[0][1]) && (Box1Point[i][1] < Box2Point[6][1]) &&
			(Box1Point[i][2] > Box2Point[0][2]) && (Box1Point[i][2] < Box2Point[6][2]))
			return false;
	}

	//Second loop - check that no points in Box2 are inside of Box1
	for (i = 0; i <8; i++)
	{
		if ((Box2Point[i][0] > Box1Point[0][0]) && (Box2Point[i][0] < Box1Point[6][0]) &&
			(Box2Point[i][1] > Box1Point[0][1]) && (Box2Point[i][1] < Box1Point[6][1]) &&
			(Box2Point[i][2] > Box1Point[0][2]) && (Box2Point[i][2] < Box1Point[6][2]))
			return false;
	}

	//if we didn't get any conflicts, everything is ok, so return true.

	return true;
}



