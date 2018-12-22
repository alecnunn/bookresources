/* Copyright (C) James Boer, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Boer, 2000"
 */
#pragma warning(disable:4786)
#include <map>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

// This function object allows us to compare map containers
template <class F, class S>
class value_equals
{
private:
	S second;
public:
	value_equals(const S& s) : second(s)
	{}
	bool operator() (pair<const F, S> elem)
	{  return elem.second == second;  }
};


// Typedef the container and iterator names for better readability
typedef map<int, string> isMap;
typedef isMap::value_type isValType;
typedef isMap::iterator isMapItor;

void main()
{
	isMap c;

	// Insert key / value pairs
	c.insert(isValType(100, "One Hundred"));
	c.insert(isValType(3, "Three"));
	c.insert(isValType(150, "One Hundred Fifty"));
	c.insert(isValType(99, "Ninety Nine"));

	// display all the keys and values
	for(isMapItor itor = c.begin(); itor != c.end();  ++itor)
		cout << "Key = " << (*itor).first << ", Value = " << (*itor).second << endl;

	// You can also access the map like an associative array
	cout << "Key 3 displays value " << c[3].c_str() << endl;

	// Or insert like this as well
	c[123] = "One Hundred Twenty Three";

	// Find and remove a specific element based on the key
	isMapItor pos = c.find(123);
	if(pos != c.end())
		// erasing an element invalidates any iterators pointing to it.  Calling
		// pos++ now would result in undefined behavior.
		c.erase(pos);

	// Find and remove an element based on the value
	pos = find_if(c.begin(), c.end(), value_equals<int, string>("Ninety Nine"));
	if(pos != c.end())
		c.erase(pos);

	// If you must remove elements while iterating through the list...
	for(isMapItor itr = c.begin(); itr != c.end(); )
	{
		if(itr->second == "Three")
			c.erase(itr++);
		else
			++itr;
	}
}
