// File:	Configuration.cpp
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	10/12/01
//
// Copyright 2001, all rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

// Class definition.
#include "Configuration.h"

// Standard template library.
#include <limits>
#include <algorithm>
#include <fstream>
#include <locale>
#include <vector>

// Desc:	Make available standard namespace functionality here for easier
//			reading in the rest of the file.
using std::ifstream;
using std::isspace;
using std::string;
using std::vector;
using std::sort;
using std::binary_search;

// Desc:	Private implementation class definition.
// Notes:	Based on the Pimpl Design Pattern.
class t_Configuration::t_Pimpl
{
public:

	// Desc:	Read the configuration file.
	// Input:	i_name - Configuration file name.
	//			i_comment - Comment character that may be used in the
	//						configuration file.
	t_Pimpl(const string &i_name, char i_comment);

	// Desc:	Test if string is in configuration.
	// Input:	i_test - String to test.
	// Output:	true if the string is in the configuration file,
	//			false otherwise.
	bool m_Contains(const string &i_test) const;

private:

	// Desc:	Sorted vector of values in configuration file.
	// Notes:	A sorted vector is used over an associative container
	//			because all inserts will happen on initialization
	//			and all subsequent operations will be lookups.
	vector<string> m_values;

}; // class t_ConfigurationPimpl

// Desc:	Read the configuration file.
t_Configuration::t_Pimpl::t_Pimpl(const string &i_name,
								  char i_comment)
{
	// Open configuration file.
	ifstream l_configurationFile(i_name.c_str());

	// If the file was successfully opened...
	if(l_configurationFile.is_open()) {
		// Process the file.
		while(!l_configurationFile.eof()) {
			// Check for comment or whitespace at the beginning of
			// the line, if either of these are found no string will
			// be extracted from this line.
			if((l_configurationFile.peek() != i_comment) &&
				(!isspace(l_configurationFile.peek(),
				l_configurationFile.getloc()))) {
				// Extract first string from line.
				string l_string;
				l_configurationFile >> l_string;

				// Add string to configuration.
				m_values.push_back(l_string);
			}

			// Advance to end of line.
			l_configurationFile.ignore(
				std::numeric_limits<int>::max(), '\n');
		}

		// Close the file.
		l_configurationFile.close();
	}

	// Sort values.
	sort(m_values.begin(), m_values.end());
}

// Desc:	Test if string is in configuration.
bool t_Configuration::t_Pimpl::m_Contains(const string &i_test) const
{
	// The values are guaranteed to be sorted so we use binary search
	// to determine if the string exists.  For a nice summary of which
	// search algorithms to use see Item 45 of Effective STL by Scott
	// Meyers.
	return(binary_search(m_values.begin(), m_values.end(), i_test));
}

// Desc:	Read the configuration file.
t_Configuration::t_Configuration(const string &i_name, char i_comment)
: m_pimpl(new t_Pimpl(i_name, i_comment))
{
}

// Desc:	Copy constructor.
t_Configuration::t_Configuration(const t_Configuration &i_rhs)
: m_pimpl(new t_Pimpl(*(i_rhs.m_pimpl)))
{
}

// Desc:	Default destructor.
t_Configuration::~t_Configuration()
{
}

// Desc:	Assignment operator.
t_Configuration &t_Configuration::operator=(const t_Configuration &i_rhs)
{
	// Create a copy of the configuration.
	t_Configuration l_copy(i_rhs);

	// Exchange the copy with our private implementation.  This will
	// destroy the current private implementation and exchange it
	// for the copies private implementation, while at the same time
	// setting the copies private implementation to NULL.  Do not
	// use the copy after this operation.  This copy/swap method
	// should be safe for exceptions and pathological self assignment.
	m_pimpl = l_copy.m_pimpl;

	// Return reference to self.
	return(*this);
}

// Desc:	Test if string is in configuration.
bool t_Configuration::m_Contains(const string &i_test) const
{
	// Call the private implementation.
	return(m_pimpl->m_Contains(i_test));
}
