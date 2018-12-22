//------------------------------------------------------------------------------//
// 
// MODULE   : CommandLineParser.cpp
// 
// PURPOSE  : CommandLineParser - Implementation
// 
// CREATED  : 10/19/2004
// 
// (c) 2004 Touchdown Entertainment, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "stdafx.h"
#include "CommandLineParser.h"
#include "stringtokenizer.h"
#include "string.h"

//---------------------------------------------------------------------------
//	CommandLineParser::CommandLineParser()
//
//---------------------------------------------------------------------------
CommandLineParser::CommandLineParser()
{
}

//---------------------------------------------------------------------------
//	CommandLineParser::CommandLineParser(const char* szCommandLine)
//
//	Purpose: Parse the command line on spaces and quotes.
//
//---------------------------------------------------------------------------
void CommandLineParser::Parse(int argc, char** argv)
{
	for(int i = 0; i < argc; ++i)
	{
		m_lstTokens.push_back(argv[i]);
	}
}

//---------------------------------------------------------------------------
//	CommandLineParser::~CommandLineParser()
//
//---------------------------------------------------------------------------
CommandLineParser::~CommandLineParser()
{
	// Release our tokens
	m_lstTokens.clear();
}

//---------------------------------------------------------------------------
//	CommandLineParser::Find()
//
//	Purpose: Lookup parameters parsed by the command line
//
//	Returns NULL if the parameter is not found, or if szArg does not have a
//	value. 
//
//	eResult will be set to CMDLINE_RESULT_EXISTS if the param is found.
//	eResult will be set to CMDLINE_RESULT_NOT_FOUND if the param is not found.
//
//	Example: If the command line is "-CenterWorld -ProjectDir c:\temp", and
//	the developer called Find("-CenterWorld", result) -CenterWorld would exist,
//	but not have a value, so it would return NULL. eResult would be 
//	CMDLINE_RESULT_EXISTS.
//
//---------------------------------------------------------------------------
const char* CommandLineParser::Find(const char* szArg, ECommandLineResult& eResult)
{
	CommandLineTokenList::iterator iter = m_lstTokens.begin();
	while(iter != m_lstTokens.end())
	{
		if(strcmpi((*iter).c_str(), szArg) == 0)
		{
			// We want the very next token
			++iter;

			if(iter != m_lstTokens.end())
			{				
				if( ((*iter).c_str())[0] == '-' )
				{
					// If the next token is another param and not a value
					eResult = CMDLINE_RESULT_EXISTS;
					return NULL;
				}
				else
				{
					// We found a value for this param
					eResult = CMDLINE_RESULT_EXISTS;
					return (*iter).c_str();
				}
			}

			// Our search param was the last token in the command line.
			eResult = CMDLINE_RESULT_EXISTS;
			return NULL;
		}

		++iter;
	}

	eResult = CMDLINE_RESULT_NOT_FOUND;
	return NULL;
}
