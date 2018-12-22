/* Copyright (C) James Boer, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Boer, 2000"
 */
#include <list>
#include <iostream>

using namespace std;

class Foo
{
public:
	Foo(int i)				{  m_iData = i;	}
	void SetData(int i)		{  m_iData = i;  }
	int GetData()			{  return m_iData;  }
private:
	int m_iData;
};

// Typedef the container and iterator names for better readability
typedef list<Foo*> FooList;
typedef FooList::iterator FooListItor;

void main()
{
	// Create a list container of integers
	FooList c;

	// Fill the list with 3 different elements
	c.push_back(new Foo(1));
	c.push_back(new Foo(2));
	c.push_back(new Foo(3));

	// Iterator through the list
	for(FooListItor itor = c.begin(); itor != c.end();)
	{
		if((*itor)->GetData() == 2)
			// demonstrates proper method of removing an element
			// from the middle of the list.
		{
			delete (*itor);
			itor = c.erase(itor);
		}
		else
			++itor;
	}

	// Make sure to delete all the objects, since the list
	// destructor will not do this automatically for you
	for(FooListItor itor2 = c.begin(); itor2 != c.end();  ++itor2)
		delete (*itor2);
}
