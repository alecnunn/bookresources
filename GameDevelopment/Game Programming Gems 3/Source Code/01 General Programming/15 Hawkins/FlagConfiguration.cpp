// File:	FlagConfiguration.cpp
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	10/15/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Provide a initialization policy that controls flag initialization
//			using a configuration file.
////////////////////////////////////////////////////////////////////////////////

// Disable VC++ 6.0 warning about debug symbol length.
#pragma warning(disable:4786)

// Class definition.
#include "FlagConfiguration.h"

// Standard template library.
#include <utility>
#include <map>

// Local headers.
#include "Configuration.h"

// Desc:	Make available standard namespace functionality here for easier
//			reading in the rest of the file.
using std::string;
using std::pair;
using std::map;

// Desc:	Convert string to boolean.
bool t_StaticConfiguration::m_Convert(const string &i_name,
									  const string &i_value)
{
	// Use static associative container to match configuration with name.
	typedef map<string, t_Configuration> t_ConfigurationContainer;
	static t_ConfigurationContainer l_configurationContainer;

	// See if the configuration is already available.
	t_ConfigurationContainer::const_iterator l_iterator =
		l_configurationContainer.find(i_name);
	if(l_iterator != l_configurationContainer.end()) {
		// Check the existing configuration to determine if the
		// value is in the configuration.
		return(l_iterator->second.m_Contains(i_value));
	}

	// Insert a new configuration in the container.
	pair<t_ConfigurationContainer::iterator, bool> l_result =
		l_configurationContainer.insert(
		t_ConfigurationContainer::value_type(i_name,
		t_Configuration(i_name)));

	// Check for value in the new configuration.
	return(l_result.first->second.m_Contains(i_value));
}
