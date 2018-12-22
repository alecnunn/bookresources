//cylinder.cpp
//source code for the cylinder room object

#include "cylinder.h"
#include <iostream>
#include <string>

using namespace std;

Cylinder::Cylinder (double mySides, 
					double myHeight,
					double myRadius,
					double myPosX,
					double myPosY,
					double myPosZ)
{
	
	cout << "Cylinder constructor" << endl;

	sides	= mySides;
	height	= myHeight;
	radius	= myRadius;
	posX	= myPosX;
	posY	= myPosY;
	posZ	= myPosZ;

	//An inscribed box has a length and width equal to the radius x sqrt(2)
	const double factor = 1.414;

	//constructor for Box: length,        width,         height,               posX, posY, posZ
	inscribedBox = new Box(radius*factor, radius*factor, height-(STDWALL*2),   posX, posY, posZ + STDWALL);
	boundingBox =  new Box(2*radius,      2*radius,      height,               posX, posY, posZ);
};

Cylinder::~Cylinder()
{
	//To avoid memory leaks - remember to delete the inscribed and bounding boxes when a Cylinder is destroyed
	cout << "Cylinder destructor" << endl;
	delete inscribedBox;
	delete boundingBox;
};

string Cylinder::createString()
{
	string s;
	char temp[50];

	s += "Cylinder smooth:on sides:";		//This set creates the 3ds MAXscript string for the outer Cylinder
	gcvt(sides,sigDigits,temp);
	s += temp;
	
	s += " height:";				
	gcvt(height,sigDigits,temp);		
	s += temp;					
								
	s += " radius:";			
	gcvt (radius,sigDigits,temp);		
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

string Cylinder::createHollowingString()
{
	string s;
	char temp[50];

	s += "Cylinder smooth:on sides:";		//This set creates the 3ds MAXscript string for the inner Cylinder
	gcvt(sides,	sigDigits,temp);				//This is the portion to be removed.
	s += temp;
	
	s += " height:";				
	gcvt(height - (STDWALL*2),sigDigits,temp);		
	s += temp;					
								
	s += " radius:";			
	gcvt (radius - (STDWALL*2),sigDigits,temp);		
	s += temp;					
								
	s += " pos:[";				
	gcvt(posX,sigDigits,temp);			
	s += temp;					
								
	s += ",";					
	gcvt(posY,sigDigits,temp);		
	s += temp;					
	s += ",";					
								
	gcvt(posZ + STDWALL,sigDigits,temp);		
	s += temp;					
	s += "]";

	s += "\n";

	return s;

}


Box* Cylinder::getInscribedBox()
{
	//This function simply returns the pointer to the inscribed Box
	//Using the box function you can then determine any info you want
	//e.g.  myCylinder->getBoundingBox()->getLength();
	//These functions become very useful in determining if two room primitives
	//intersect with one another.
	// 
	return inscribedBox;
};

Box* Cylinder::getBoundingBox()
{
	//This function simply returns the pointer to the outscribed Box
	//Using the box function you can then determine any info you want
	//e.g.  myCylinder->getOutscribedBox()->getLength();
	
	return boundingBox;
};