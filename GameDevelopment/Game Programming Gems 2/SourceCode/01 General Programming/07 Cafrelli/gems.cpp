/* Copyright (C) Charles Cafrelli, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Charles Cafrelli, 2001"
 */
// gems.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include "Property.h"

class GameObject : public PropertySet
{
	int m_speed;
	std::string m_name;
public:
	GameObject()
	{
		Register("name",&m_name);
		Register("speed",&m_speed);
	}
};


int main(int argc, char* argv[])
{

	GameObject starship;

	// set starship's name and speed...
	starship.Set("name","Enterprise");
	starship.Set("speed",4);

	// if you're unsure of the property's type, use the generic SetValue which will
	// attempt to do the conversion for you...
	starship.SetValue("speed","6");

	Property* speed_property = starship.Lookup("speed");
	int ispeed = speed_property->GetInt();

	ispeed += 2;

	speed_property->Set(ispeed);

	// this is a convenience function so that you don't have to go through the hoops
	// of doing the conversion to display the contents yourself.
	std::string speed = speed_property->GetValue();
	
	return 0;
}
