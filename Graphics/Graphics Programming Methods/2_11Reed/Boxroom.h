//BoxRoom.h  
//header file for the BoxRoom room

//--------------------------------------------------------------------------
//Please note the distinction between Box and Boxroom.
//
//Box - a utility class that all the primitives use.  Boxes are used for
//      the inscribedBox and the boundingBox for each primitive.  This class 
//      is not derived from any other.
//		
//Boxroom - a room class, like other room classes, cylinder, sphere, gengon.  
//          except it is box shaped.
//--------------------------------------------------------------------------


#ifndef BOXROOM_H
#define BOXROOM_H

#include "room.h"
#include "box.h"
#include "constants.h"
#include <string>
using namespace std;


class BoxRoom: public Room
{
	private:
		double length,
			   width, 
			   height,
			   posX,
			   posY, 
			   posZ;

		Box* inscribedBox;
		Box* boundingBox;
		
	public:
		//constructor
		BoxRoom(double length,
				double width,
				double height,
				double posX,
				double posY,
				double posZ);

		//destructor
		~BoxRoom();

		//accessor functions
		double getLength()		{return length;};
		double getWidth()		{return width;};
		double getHeight()		{return height;};
		double getPosX()		{return posX;};
		double getPosY()		{return posY;};
		double getPosZ()		{return posZ;};
		void setLength	(double myLength)	{length	= myLength;};
		void setWidth	(double myWidth)	{width	= myWidth;};
		void setHeight	(double myHeight)	{height = myHeight;};

		//Note that positions are absolute, not relative.
		//When you move the Boxroom - you must also move the equivalent
		//inscribedBox and boundingBox.)

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

		//Create the script for making the room that is used to hollow a given room.
		//The "hollowing" room is the same shape, but slightly smaller.
		//When this room is subtracted from the first room - VOILA! you have created
		//a hollow room.
		string createHollowingString();

		
};


#endif