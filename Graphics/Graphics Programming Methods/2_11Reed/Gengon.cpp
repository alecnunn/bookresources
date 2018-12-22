//gengon.cpp
//source code for the gengon room object
#include "constants.h"
#include "gengon.h"
#include <cmath>
#include <iostream>
#include <string>

using namespace std;

Gengon::Gengon (double mySides, 
				double myRadius, 
				double myFillet, 
				double myHeight, 
				double myPosX, 
				double myPosY, 
				double myPosZ)
{
	
	cout << "Gengon constructor" << endl;

	sides	= mySides;
	radius	= myRadius;
	fillet	= myFillet;
	height	= myHeight;
	posX	= myPosX;
	posY	= myPosY;
	posZ	= myPosZ;


	//tan [360/(2n)] = 0.5s/a, 
	//where n = # sides of the polygon, 
	//s = the length of its side and 
	//a = the length of its apothem.
	
	// rearranging a = 0.5s/(tan [360/(2n))

	double factor1 = 0.5 * radius * radius * sin((2*pi/sides));
	double factor2 = tan(pi/sides);
	double factor3 = sqrt(factor1/factor2);

	inscribedBox = new Box(2*factor3/1.414, 2*factor3/1.414, height-(STDWALL*2), posX,posY,posZ + STDWALL);
	boundingBox  = new Box(2*radius,        2*radius,        height,         posX,posY,posZ);

};

Gengon::~Gengon()
{
	//To avoid memory leaks - remember to delete the inscribed and bounding boxes when a Gengon is destroyed
	cout << "Gengon destructor" << endl;
	delete inscribedBox;
	delete boundingBox;
};

string Gengon::createString()
{
	string s;
	char temp[50];

	s += "Gengon sides:";		//This set creates the 3ds MAXscript string for the outer gengon
	gcvt(sides,sigDigits,temp);
	s += temp;
	
	s += " radius:";			
	gcvt (radius,sigDigits,temp);		
	s += temp;

	s += " fillet:";			
	gcvt (fillet,sigDigits,temp);		
	s += temp;

	s += " height:";				
	gcvt(height,sigDigits,temp);		
	s += temp;					
								
	s += " pos:[";				
	gcvt(posX,sigDigits,temp);			
	s += temp;					
								
	s += ",";					
	gcvt(posY,sigDigits,temp);		
	s += temp;					
	s += ",";					
								
	gcvt(posZ,sigDigits,temp);		
	s += temp;					
	s += "]";
	
	s += "\n";

	return s;
};

string Gengon::createHollowingString()
{
	string s;
	char temp[50];
	
	s += "Gengon sides:";		//This set creates the 3ds MAXscript string for the inner gengon
	gcvt(sides,sigDigits,temp);			//This is the portion to be removed.
	s += temp;
	
	s += " radius:";			
	gcvt (radius - (STDWALL*2),sigDigits,temp);		
	s += temp;

	s += " fillet:";			
	gcvt (fillet,sigDigits,temp);		
	s += temp;

	s += " height:";				
	gcvt(height - (STDWALL*2),sigDigits,temp);		
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

Box* Gengon::getInscribedBox()
{
	//This function simply returns the pointer to the inscribed Box
	//Using the box function you can then determine any info you want
	//e.g.  myGengon->getInscribedBox()->getLength();
	return inscribedBox;
};

Box* Gengon::getBoundingBox()
{
	//This function simply returns the pointer to the bounding Box
	//Using the box function you can then determine any info you want
	//e.g.  myGengon->getBoundingBox()->getLength();
	return boundingBox;
};