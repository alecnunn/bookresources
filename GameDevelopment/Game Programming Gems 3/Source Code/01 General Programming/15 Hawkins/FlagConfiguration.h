// File:	FlagConfiguration.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	10/15/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Provide a initialization policy that controls flag initialization
//			using a configuration file.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_FLAG_CONFIGURATION_HEADER__
#define __BMH_FLAG_CONFIGURATION_HEADER__

// Standard template library.
#include <string>

// Desc:	Default filename.
// Notes:	Defined as static member function for use as template argument
//			by flag configuration user.
struct t_DebugCfg
{
	// Desc:	Provide configuration file name.
	// Output:	String for configuration file name.
	static inline const std::string m_Name() { return("Debug.cfg"); }
};

// Desc:	Static conversion function for tracking configuration conversion
//			based on configuration filename and test string.
struct t_StaticConfiguration
{
	// Desc:	Convert string to boolean.
	// Input:	i_name - Configuration file name.
	//			i_value - String value to test against configuration.
	// Output:	Boolean result of conversion.
	static bool m_Convert(const std::string &i_name,
		const std::string &i_value);
};

// Desc:	String to boolean mutator policy.
// Input:	t_Name - Class providing configuration file name through
//						member function m_Name().
template <class t_Name = t_DebugCfg> class t_FlagConfigurationPolicy
{
public:

	// Desc:	Policy initialization type.
	typedef const std::string& t_Type;

	// Desc:	Conversion policy from mutator type to boolean.
	// Input:	i_value - Value to be converted to boolean.
	// Output:  Boolean result of conversion.
	static bool m_Convert(t_Type i_value)
	{
		return(t_StaticConfiguration::m_Convert(
			t_Name::m_Name(), i_value));
	}

}; // class t_FlagConfigurationPolicy

#endif  // __BMH_FLAG_CONFIGURATION_HEADER__
