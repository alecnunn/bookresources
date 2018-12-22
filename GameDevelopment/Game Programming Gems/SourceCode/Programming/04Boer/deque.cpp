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
#include <deque>

using namespace std;

// Typedef the container and iterator names for better readability
typedef deque<int> IntDeque;
typedef IntDeque::reverse_iterator IntDequeRItor;

void main()
{
	// Create a deque container of integers
	IntDeque c;

	// Fill the deque with 3 different elements
	c.push_front(3);
	c.push_front(2);
	c.push_front(1);
	c.push_back(3);
	c.push_back(2);
	c.push_back(1);

	// Cycle BACKWARD through the list - special iterators and notation
	// is necessary to do this.
	for(IntDequeRItor ritor = c.rbegin(); ritor != c.rend(); ++ritor)
		cout << "Value = " << (*ritor) << endl;

	// remove the first and last elements
	c.pop_front();
	c.pop_back();

	// Accessing elements directly - if needed remember to check to see
	// the deque is not empty.  Accessing non-existant elements will
	// lead to undefined behavior; probably an access violation
	if(!c.empty())
	{
		cout << "Front = " << c.front() << endl;
		cout << "Back = " << c.back() << endl;
	}
}
