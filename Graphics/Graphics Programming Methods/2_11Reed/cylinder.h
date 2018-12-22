//cylinder.h
//header file for the cylinder class

#ifndef CYLINDER_H
#define CYLINDER_H

#include "room.h"
#include "box.h"
#include <string>
using namespace std;

#include "constants.h"


class Cylinder:public Room
{
	private:
		double sides,
			   height,
			   radius,
			   posX,
			   posY,
			   posZ;

		Box* inscribedBox;
		Box* boundingBox;

	public:
		//constructor
		Cylinder(double sides,
				 double height,
				 double radius,
				 double posX,
				 double posY,
				 double posZ);

		//destructor
		~Cylinder();

		//accessor functions
		double getSides()		{return sides;};
		double getHeight()		{return height;};
		double getRadius()		{return radius;};
		double getPosX()		{return posX;};
		double getPosY()		{return posY;};
		double getPosZ()		{return posZ;};

		void setSides	(double mySides)	{sides	= mySides;};
		void setHeight	(double myHeight)	{height = myHeight;};
		void setRadius	(double myRadius)	{radius	= myRadius;};
		void setPosX	(double myPosX)		{posX	= myPosX;
											 inscribedBox->setPosX(myPosX);
										 	 boundingBox->setPosX(myPosX);};

		void setPosY	(double myPosY)		{posY	= myPosY;
											inscribedBox->setPosY(myPosY);
											boundingBox->setPosY(myPosY);};

		void setPosZ	(double myPosZ)		{posZ	= myPosZ;
											inscribedBox->setPosZ(myPosZ + STDWALL);
											boundingBox->setPosZ(myPosZ);};

		virtual Box* getInscribedBox();
		virtual Box* getBoundingBox();

		//Create the script for making a room
		string createString();
		string createHollowingString();
};


#endif