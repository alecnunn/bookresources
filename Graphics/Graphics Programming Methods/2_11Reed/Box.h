//box.h  
//header file for the Box room
//
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


#ifndef BOX_H
#define BOX_H

#include "constants.h"
#include <string>
using namespace std;

class Box
{
	private:
		double length,
			   width, 
			   height,
			   posX,
			   posY, 
			   posZ;
		
	public:
		//constructor
		Box(double length,
			double width,
			double height,
			double posX,
			double posY,
			double posZ);

		//destructor
		~Box();

		//accessor functions
		double getLength()		{return length;};
		double getWidth()		{return width;};
		double getHeight()		{return height;};
		double getPosX()		{return posX;};
		double getPosY()		{return posY;};
		double getPosZ()		{return posZ;};
		void   setPosX(double myPosX)		{posX = myPosX;};	
		void   setPosY(double myPosY)		{posY = myPosY;};
		void   setPosZ(double myPosZ)		{posZ = myPosZ;};

		//Create the script for making a room
		//
		string createString();
	
		//The followingfunction has not been implemented 
		//but it might be useful as a debugging tool in a later version.
		//
		//string createHollowingString();
	

		
};


#endif