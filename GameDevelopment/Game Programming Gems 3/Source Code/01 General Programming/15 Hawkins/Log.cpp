// File:	Log.cpp
// Author:	Brian Hawkins (winterdark@sprynet.com)
// Date:	10/16/01
//
// Copyright 2001, all rights reserved.
//
// Desc:	Generic policy-based logging.
//			For more information on the concept of policies and other generic
//			programming techniques see Modern C++ Design.
////////////////////////////////////////////////////////////////////////////////

// Class definition.
#include "Log.h"

// Standard template library.
#include <iostream>

// Desc:	Make available standard namespace functionality here for easier
//			reading in the rest of the file.
using std::string;
using std::cout;
using std::cerr;

// Desc:	Dispatch string.
void t_StandardOutDispatchPolicy::m_Dispatch(const string &i_string)
{
	cout << i_string;
}

// Desc:	Dispatch string.
void t_StandardErrorDispatchPolicy::m_Dispatch(const string &i_string)
{
	cerr << i_string;
}
