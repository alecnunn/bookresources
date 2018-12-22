#ifndef _COMMAND_LINE_PARSER_H_
#define _COMMAND_LINE_PARSER_H_

#include "stdafx.h"

// STL List
#include <list>

// Define the max size for a given token
#define MAX_CMDLINE_TOKEN_SIZE 4096

// Return types
enum ECommandLineResult
{
	CMDLINE_RESULT_EXISTS = 0,
	CMDLINE_RESULT_NOT_FOUND
};

// Class definition
class CommandLineParser
{
public:
	CommandLineParser();
	~CommandLineParser();

	size_t GetNumArgs() { return m_lstTokens.size(); }
	const char* Find(const char* szArg, ECommandLineResult& eResult);
	void Parse(int argc, char** argv);
	const char* GetCommandLine() { return m_stringCommandLine.c_str(); }

protected:
	typedef std::list<std::string> CommandLineTokenList;
	CommandLineTokenList m_lstTokens;
	std::string m_stringCommandLine;

};

#endif
