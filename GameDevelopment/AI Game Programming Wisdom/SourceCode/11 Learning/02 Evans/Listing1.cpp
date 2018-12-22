/* Copyright (C) Richard Evans, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Richard Evans, 2001"
 */

// So we need another way of defining objects, one which makes 
// it easy for us to iterate through the attributes:

class Attribute		
{
public:
	virtual int GetValueRange()=0
	virtual int GetValue()=0
	virtual char* GetName()=0
};

class Health : public Attribute
{
	...
};

class Energy : public Attribute
{
	...
};

class Object
{
protected:
	Attribute**	Attributes;
public:
	virtual int GetNumAttributes()=0;	
	...
};

class Villager : public Object
{
public:
	int GetNumAttributes();
	...
};

// Now it is easy to iterate through the attributes:

void Object::IterateThroughAttributes(void (*function)(Attribute*))
{
	for(int i=0; i<GetNumAttributes(); i++)
	{
		function(Attributes[i]);
	}
}

