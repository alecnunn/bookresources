// File:	Configuration.h
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	10/12/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Supports a simple configuration file format.
//
//			Each line that does not begin with whitespace or a comment
//			character is considered to be a configuration entry.  The
//			string up to the first whitespace or the end of the line,
//			whichever is first, is stored.  To test if a string is
//			in the configuration file call the m_Contains member function.
////////////////////////////////////////////////////////////////////////////////

// Include only once per compile.
#pragma once
#ifndef __BMH_CONFIGURATION_HEADER__
#define __BMH_CONFIGURATION_HEADER__

// Disable VC++ 6.0 warning about debug symbol length.
#pragma warning(disable:4786)

// Standard template library.
#include <memory>
#include <string>

// Desc:	Class definition.
class t_Configuration
{
public:

	// Desc:	Read the configuration file.
	// Input:	i_name - Configuration file name.
	//			i_comment - Comment character that may be used in the
	//						configuration file.
	t_Configuration(const std::string &i_name, char i_comment = '#');

	// Desc:	Copy constructor.
	// Input:	i_rhs - Instance to copy.
	// Notes:	Perform deep copy of private implementation.
	t_Configuration(const t_Configuration &i_rhs);

	// Desc:	Default destructor.
	// Notes:	Defined to remain consistent with the need for a copy
	//			constructor and assignment operator.
	~t_Configuration();

	// Desc:	Assignment operator.
	// Input:	i_rhs - Instance to copy.
	// Output:	Reference to self.
	// Notes:	Perform deep copy of private implementation.
	t_Configuration &operator=(const t_Configuration &i_rhs);

	// Desc:	Test if string is in configuration.
	// Input:	i_test - String to test.
	// Output:	true if the string is in the configuration file,
	//			false otherwise.
	bool m_Contains(const std::string &i_test) const;

private:

	// Desc:	Forward declaration.
	// Notes:	Minimize compilation dependencies.  For more information see
	//			Item 34 of Effective C++ by Scott Meyers.
	class t_Pimpl;

	// Desc:	Private implementation.
	// Notes:	Based on the Pimpl Design Pattern.
	std::auto_ptr<t_Pimpl> m_pimpl;

}; // class t_Configuration

#endif  // __BMH_CONFIGURATION_HEADER__
