/* Copyright (C) James Boer, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Boer, 2000"
 */
#include <iostream>
#include <vector>

using namespace std;

// Typedef the container and iterator names for better readability
typedef vector<int> IntVector;
typedef IntVector::iterator IntVectItor;

void main()
{
	// Create a vector object of integers
	IntVector c;
	// Reserve room for 10 integers
	c.reserve(10);

	// Fill the vector with 3 different elements
	c.push_back(3);
	c.push_back(99);
	c.push_back(42);

	// Now loop and print out all the element values
	for(IntVectItor itor = c.begin(); itor != c.end(); ++itor)
		cout << "element value = " << (*itor) << endl;




	// Since the elements have been created, we can access or replace
	// them just like a normal array.
	c[0] = 12;
	c[1] = 32;
	c[2] = 999;
	for(int i = 0; i < c.size(); i++)
		cout << "element value = " << c[i] << endl;
}
