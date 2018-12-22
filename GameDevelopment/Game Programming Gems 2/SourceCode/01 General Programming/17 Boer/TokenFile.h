//=============================================================================
//
// 	TokenFile.h   
//
// 	Author:	 James Boer
//
//	Copyright (C) 2001 James R. Boer  All Rights Reserved.
//	
//=============================================================================

/*

Some general notes and limitations to be aware of...

In order to minimize file size, and thus loading times, I've made certain
optimizations to the data storage.  The biggest and most significant optimization
is the use of a string table at the beginning of the file to store keywords, 
operators, and variables.  These are stored in string format in the tokens, and
since they are often repeating, it makes sense to use a lookup table for huge
space savings.  Each token type is stored in it's own table, and the tokens are
stored as indices into those tables instead of storing the actual strings.  A
further optimization is made if there are fewer than 256 entries in a particular
table - the index value is stored as a byte instead of a word.

Note that since all the index values are stored at a maximum value of a word, the
maximum allowed number of unique keywords, operators, or variables in a single
compiled file is 65535.  If this ends up being a limitation, the code can easily
be adjusted to switch to storing the value in a dword instead of word.  Or, better
yet, make it a three-way switch to determine the size of the storage chunk of
the indices.

Strings are also stored as a length, followed by a sequence of bytes.  The index is
a word, so strings are also limited to 65K characters.

*/


#pragma once

#ifndef __TOKEN_FILE_H
#define __TOKEN_FILE_H

#pragma warning(disable:4786)
#pragma warning(disable:4251)

#include <queue>
#include "Token.h"

#define TF_CASE_SENSITIVE	0x0001

typedef std::queue<unsigned char> ByteQueue;

class TokenFile 
{
public:
	TokenFile()				{  Clear();  }
	virtual ~TokenFile()	{  Destroy();  }

	void Clear();
	void Destroy();

	bool Write(std::ostream &Output, TokenList* pTokenList, DWORD dwFlags = 0);
	bool Write(std::string sFilename, TokenList* pTokenList, DWORD dwFlags = 0);

	bool Read(std::istream &Input, TokenList* pTokenList);
	bool Read(std::string sFilename, TokenList* pTokenList);

// Internal functions
private:

	// Output functions

	// Write the header containing the file id and version information
	void WriteHeader();
	// Iterate through the list and pre-generate string-based tokens (except strings)
	void GenerateTables();
	// Output the string-based tables
	void WriteTables();
	// Output the token stream
	void WriteTokens();

	// Basic types
	void WriteString(std::string sData);
	void WriteDword(DWORD dwData);
	void WriteWord(WORD wData);
	void WriteByte(BYTE byteData);
	void WriteDouble(double dData);
	void WriteGuid(GUID &guidData);

	// Input functions

	bool ReadHeader();
	bool ReadTables();
	bool ReadTokens();

	// Basic types
	std::string ReadString();
	DWORD ReadDword();
	WORD ReadWord();
	BYTE ReadByte();
	double ReadDouble();
	GUID ReadGuid();

	bool HandleError(const char* lpszFormat, ...);
	std::string GetLastErrorMessage()	{  return m_sErrorMessage;  }

// Member data
private:
	
	TokenList*		m_pTokenList;

	StringVector	m_vecKeyword;
	StringVector	m_vecOperator;
	StringVector	m_vecVariable;

	ByteQueue		m_queBuffer;

	bool			m_bCaseSensitive;

	bool			m_bWordKeywordIndex;
	bool			m_bWordOperatorIndex;
	bool			m_bWordVariableIndex;

	// last stored error message
	std::string		m_sErrorMessage;
};



#endif // __TOKEN_FILE_H