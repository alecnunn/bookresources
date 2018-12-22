//BoxRoom.cpp
//source code for the BoxRoom room object

#include "BoxRoom.h"
#include <iostream>
#include <string>

using namespace std;


//constructor
BoxRoom::BoxRoom (double myLength,
		          double myWidth,
		          double myHeight,
		          double myPosX,
		          double myPosY,
		          double myPosZ)
{
	
	cout << "BoxRoom constructor" << endl;
	
	length	= myLength;
	width	= myWidth;
	height	= myHeight;
	posX	= myPosX;
	posY	= myPosY;
	posZ	= myPosZ;

	inscribedBox = new Box(length - (STDWALL*2),
						   width  - (STDWALL*2),
						   height - (STDWALL*2),
						   posX,
						   posY,
						   posZ + STDWALL);

	boundingBox = new Box (length, width, height, posX, posY, posZ);


};

//destructor
BoxRoom::~BoxRoom()
{

	//To avoid memory leaks - remember to delete the inscribed and bounding boxes when a Boxroom is destroyed
		cout << "BoxRoom destructor" << endl;
	delete inscribedBox;
	delete boundingBox;
};


string BoxRoom::createString()
{
	string s;
	char temp[50];

	s += "Box length:";		//This set creates the 3ds MAXscript string for the outer BoxRoom
	gcvt(length,sigDigits,temp);
	s += temp;
	
	s += " width:";				
	gcvt(width,sigDigits,temp);		
	s += temp;					
								
	s += " height:";			
	gcvt (height,sigDigits,temp);		
	s += temp;					
								
	s += " pos:[";				
	gcvt(posX,sigDigits,temp);			
	s += temp;					
								
	s += ",";					
	gcvt(posY, sigDigits,temp);		
	s += temp;					
	s += ",";					
								
	gcvt(posZ, sigDigits,temp);		
	s += temp;					
	s += "]";
	
	s += "\n";

	return s;

};


//This set creates the 3ds MAXscript string for the room that 
//is used to hollow the room created in the function above.
//The room is the same shape but slight smaller.

string BoxRoom::createHollowingString()
{
	string s;
	char temp[50];

	s += "Box length:";		
	gcvt(length - (STDWALL*2),sigDigits,temp);		
	s += temp;
	
	s += " width:";				
	gcvt(width - (STDWALL*2),sigDigits,temp);		
	s += temp;					
								
	s += " height:";			
	gcvt (height - (STDWALL*2),sigDigits,temp);		
	s += temp;					
								
	s += " pos:[";				
	gcvt(posX,sigDigits,temp);			
	s += temp;					
								
	s += ",";					
	gcvt(posY, sigDigits,temp);		
	s += temp;					
	s += ",";					
								
	gcvt(posZ + STDWALL, sigDigits,temp);		
	s += temp;					
	s += "]";

	s += "\n";

	return s;
};


Box* BoxRoom::getInscribedBox()
{
	return inscribedBox;
};

Box* BoxRoom::getBoundingBox()
{
	return boundingBox;
};