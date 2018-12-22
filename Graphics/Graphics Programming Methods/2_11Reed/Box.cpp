//box.cpp
//source code for the box utility object


#include "box.h"
#include <iostream>
#include <string>

using namespace std;

Box::Box (double myLength,
		  double myWidth,
		  double myHeight,
		  double myPosX,
		  double myPosY,
		  double myPosZ)
{
	
	cout << "Box constructor" << endl;
	
	length	= myLength;
	width	= myWidth;
	height	= myHeight;
	posX	= myPosX;
	posY	= myPosY;
	posZ	= myPosZ;
};

Box::~Box()
{
	cout << "Box destructor" << endl;
};


string Box::createString()
{
	string s;
	char temp[50];

	s += "Box length:";		//This set creates the 3ds MAXscript string for the Box.
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