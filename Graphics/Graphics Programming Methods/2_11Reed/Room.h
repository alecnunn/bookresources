//room.h
//header file for the base room class

#ifndef ROOM_H
#define ROOM_H

#include "box.h"
#include <string>
using namespace std;

class Room
{
	public:

		virtual ~Room(){};
		//pure virtual functions to be overridden in the derived classes
		virtual Box* getInscribedBox() = 0;
		virtual Box* getBoundingBox() = 0;
		virtual string createString() = 0;
		virtual string createHollowingString() = 0;

		virtual double getPosX()		= 0;
		virtual double getPosY()		= 0;
		virtual double getPosZ()		= 0;	

		virtual void setPosX	(double myPosX)	 =0;
		virtual void setPosY	(double myPosY)	 =0;
		virtual void setPosZ	(double myPosZ)	 =0;
		
};

#endif