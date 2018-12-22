//sphere.cpp
//source code for the sphere room object

#include "sphere.h"
#include <iostream>
#include <string>

using namespace std;

Sphere::Sphere (double myRadius,
				double myPosX,
				double myPosY,
				double myPosZ)
{
	
	cout << "Sphere constructor" << endl;

	radius	= myRadius;
	posX	= myPosX;
	posY	= myPosY;
	posZ	= myPosZ;
	
	//the length of a side of an inscribed box in a sphere is (2/(sqrt(3))) * r 
	//You can find this in most high school geometry books.
	// note (2/(sqrt(3))) = 1.1547
	const double factor = 1.1547;
	inscribedBox =	new Box(radius*factor,		//length
							radius*factor,		//width
							radius*factor,		//height
							posX,				
							posY,
							posZ - (radius*factor/2) + radius); //fixed this.


	boundingBox =   new Box(2*radius,
						    2*radius,
							2*radius,
							posX,
							posY,
							posZ);   

};

Sphere::~Sphere()
{
	
	//To avoid memory leaks - remember to delete the inscribed and bounding boxes when a Sphere is destroyed
	cout << "Sphere destructor" << endl;
	delete inscribedBox;
	delete boundingBox;
};

string Sphere::createString()
{
	string s;
	char temp[50];

	s += "Sphere radius:";		//This set creates the 3ds MAXscript string for the outer Sphere
	gcvt (radius,sigDigits, temp);		
	s += temp;
							
	s += " smooth:on pos:[";				
	gcvt(posX,sigDigits, temp);			
	s += temp;					
								
	s += ",";					
	gcvt(posY, sigDigits, temp);		
	s += temp;					
	s += ",";					
								
	gcvt(posZ + radius, sigDigits, temp);		
	s += temp;					
	s += "]";
	
	s += "\n";

	return s;

};

string Sphere::createHollowingString()
{
	string s;
	char temp[50];

	s += "Sphere radius:";			//This set creates the 3ds MAXscript string for the inner Sphere
	gcvt (radius - (STDWALL*2),sigDigits, temp);	//This is the portion to be removed.
	s += temp;
							
	s += "  smooth:on pos:[";				
	gcvt(posX,sigDigits, temp);			
	s += temp;					
								
	s += ",";					
	gcvt(posY, sigDigits, temp);		
	s += temp;					
	s += ",";					
								
	gcvt(posZ + radius + STDWALL, sigDigits, temp);		
	s += temp;					
	s += "]";

	s += "\n";

	return s;
};


Box* Sphere::getInscribedBox()
{
	//This function simply returns the pointer to the inscribed Box
	//Using the box function you can then determine any info you want
	//e.g.  mySphere->getInscribedBox()->getLength();
	return inscribedBox;
};

Box* Sphere::getBoundingBox()
{
	//This function simply returns the pointer to the inscribed Box
	//Using the box function you can then determine any info you want
	//e.g.  mySphere->getBoundingBox()->getLength();
	return boundingBox;
};