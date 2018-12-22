//sphere.h
//header file for the sphere class

#ifndef SPHERE_H
#define SPHERE_H

#include "room.h"
#include "box.h"
#include <string>
using namespace std;

#include "constants.h"

//Spheres are slightly different than the other primitives because they are created
//in 3ds max with the origin at their center - instead of at the bottom center
// To make up for this the createString and createHollowingString adjust them such that the 
//posX, posY, posZ location "thinks" that the origin is at the bottom center (like
//all the other primitives.)


class Sphere:public Room
{
	private:
		double radius,
			   posX,
			   posY,
			   posZ;

		Box* inscribedBox;
		Box* boundingBox;
			
	public:
		//constructor
		Sphere(double radius,
			   double posX, 
			   double posY, 
			   double posZ);

		//destructor
		~Sphere();

		//accessor functions
		double getRadius()		{return radius;};
		double getPosX()		{return posX;};
		double getPosY()		{return posY;};
		double getPosZ()		{return posZ;};

		void setRadius	(double myRadius)	{radius	= myRadius;};
		void setPosX	(double myPosX)		{posX	= myPosX;
											 inscribedBox->setPosX(myPosX);
											 boundingBox->setPosX(myPosX);};
		void setPosY	(double myPosY)		{posY	= myPosY;
											 inscribedBox->setPosY(myPosY);
											 boundingBox->setPosY(myPosY);};
		//.42265 = - (radius*factor/2) + radius) where factor = 2/(sqrt(3))) = 1.1547
		//see sphere.cpp for more info		
		void setPosZ	(double myPosZ)		{posZ	= myPosZ;
											 inscribedBox->setPosZ(myPosZ + radius * .42265); //.42265 = - (radius*factor/2) + radius)
											 boundingBox->setPosZ(myPosZ);}; 

		Box* getInscribedBox();
		Box* getBoundingBox();

		//Create the script for making a room
		string createString();
		string createHollowingString();
		//This function gets the box that can be fully inscribed in the sphere
		//This is used to ensure that a given "room" intersect fully with only one other room.
		
};


#endif