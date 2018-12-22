//gengon.h
//header file for the gengon class
// a gengon is a 3d prism based on a regular polygon - it also has filleted sides


#ifndef GENGON_H
#define GENGON_H

#include "room.h"
#include "box.h"
#include <string>
using namespace std;

#include "constants.h"


class Gengon:public Room
{
	private:
		double sides,
			   radius, 
			   fillet, 
			   height, 
			   posX, 
			   posY, 
			   posZ;

		Box* inscribedBox;
		Box* boundingBox;

	public:
		//constructor
		Gengon(double sides, 
			   double radius,
			   double fillet,
			   double height,
			   double posX,
			   double posY,
			   double posZ);

		//destructor
		~Gengon();

		//accessor functions
		double getSides()		{return sides;};
		double getRadius()		{return radius;};
		double getFillet()		{return fillet;};
		double getHeight()		{return height;};
		double getPosX()		{return posX;};
		double getPosY()		{return posY;};
		double getPosZ()		{return posZ;};	

		void setSides	(double mySides)	{sides	= mySides;};
		void setRadius	(double myRadius)	{radius	= myRadius;};
		void setFillet	(double myFillet)	{fillet	= myFillet;};
		void setHeight	(double myHeight)	{height = myHeight;};
		void setPosX	(double myPosX)		{posX	= myPosX;
											inscribedBox->setPosX(myPosX);
											boundingBox->setPosX(myPosX);};

		void setPosY	(double myPosY)		{posY	= myPosY;
											inscribedBox->setPosY(myPosY);
											boundingBox->setPosY(myPosY);};

		void setPosZ	(double myPosZ)		{posZ	= myPosZ;
											inscribedBox->setPosZ(myPosZ + STDWALL);
											boundingBox->setPosZ(myPosZ);};


		Box* getInscribedBox();
		Box* getBoundingBox();
		
		//Create the script for making a room
		string createString();
		string createHollowingString();
};


#endif