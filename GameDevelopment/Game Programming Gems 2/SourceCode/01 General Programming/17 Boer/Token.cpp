//=============================================================================
//
// 	Token.cpp   
//
// 	Author:	 James Boer
//
//	Copyright (C) 2001 James R. Boer  All Rights Reserved.
//	
//=============================================================================

#include "Token.h"
#include <string>
#include <assert.h>
#include <iostream>

#pragma warning(disable:4800)		// forcing bool to true or false

using namespace std;

Token::Token(const Token& tok)
{
	Clear();

#ifdef TOKEN_DEBUGGING_INFO
	m_iLineNumber = tok.m_iLineNumber;
	m_iFileIndex = tok.m_iFileIndex;
#endif // TOKEN_DEBUGGING_INFO

	m_Type = tok.m_Type;

	switch(m_Type)
	{
	case STRING:
		m_pszString = new char[strlen(tok.m_pszString) + 1];
		strcpy(m_pszString, tok.m_pszString);
		break;
	case KEYWORD:
		m_pszKeyword = new char[strlen(tok.m_pszKeyword) + 1];
		strcpy(m_pszKeyword, tok.m_pszKeyword);
		break;
	case OPERATOR:
		m_pszOperator = new char[strlen(tok.m_pszOperator) + 1];
		strcpy(m_pszOperator, tok.m_pszOperator);
		break;
	case VARIABLE:
		m_pszVariable = new char[strlen(tok.m_pszVariable) + 1];
		strcpy(m_pszVariable, tok.m_pszVariable);
		break;
	case BOOLEAN:
		m_bBoolean = tok.m_bBoolean;
		break;
	case INTEGER:
		m_iInteger = tok.m_iInteger;
		break;
	case REAL:
		m_dReal = tok.m_dReal;
		break;
	case T_GUID:
		m_pguidGuid = new GUID;
		memcpy(m_pguidGuid, tok.m_pguidGuid, sizeof(GUID));
		break;
	};
		
}

void Token::operator =( const Token& tok )
{
	Destroy();

#ifdef TOKEN_DEBUGGING_INFO
	m_iLineNumber = tok.m_iLineNumber;
#endif
	m_Type = tok.m_Type;

	switch(m_Type)
	{
	case STRING:
		m_pszString = new char[strlen(tok.m_pszString) + 1];
		strcpy(m_pszString, tok.m_pszString);
		break;
	case KEYWORD:
		m_pszKeyword = new char[strlen(tok.m_pszKeyword) + 1];
		strcpy(m_pszKeyword, tok.m_pszKeyword);
		break;
	case OPERATOR:
		m_pszOperator = new char[strlen(tok.m_pszOperator) + 1];
		strcpy(m_pszOperator, tok.m_pszOperator);
		break;
	case VARIABLE:
		m_pszVariable = new char[strlen(tok.m_pszVariable) + 1];
		strcpy(m_pszVariable, tok.m_pszVariable);
		break;
	case BOOLEAN:
		m_bBoolean = tok.m_bBoolean;
		break;
	case INTEGER:
		m_iInteger = tok.m_iInteger;
		break;
	case REAL:
		m_dReal = tok.m_dReal;
		break;
	case T_GUID:
		m_pguidGuid = new GUID;
		memcpy(m_pguidGuid, tok.m_pguidGuid, sizeof(GUID));
		break;
	};
		
}


void Token::Clear()
{
	m_Type = UNKNOWN_TOKEN;
#ifdef TOKEN_DEBUGGING_INFO
	m_iLineNumber = 0;
	m_iFileIndex = 0;
#endif // TOKEN_DEBUGGING_INFO
	m_pszString = NULL;
}


void Token::Destroy()
{
	switch(m_Type)
	{
	case STRING:
		delete[] m_pszString;
		m_pszString = 0;
		break;
	case KEYWORD:
		delete[] m_pszKeyword;
		m_pszKeyword = 0;
		break;
	case OPERATOR:
		delete[] m_pszOperator;
		m_pszOperator = 0;
		break;
	case VARIABLE:
		delete[] m_pszVariable;
		m_pszVariable = 0;
		break;
	case T_GUID:
		delete[] m_pguidGuid;
		m_pguidGuid = 0;
		break;
	};
	Clear();
}


const char* Token::GetDescriptiveString() const
{
	static char pszBuffer[256];
	static string sBuffer;

	switch(GetType())
	{
	case Token::INTEGER:
		sBuffer = "Integer: ";
		itoa(GetInteger(), pszBuffer, 10);
		sBuffer += pszBuffer;
		break;
	case Token::REAL:
		sBuffer = "Real: ";
		_gcvt(GetReal(), 10, pszBuffer);
		sBuffer += pszBuffer;
		break;
	case Token::STRING:
		sBuffer = "String: ";
		sBuffer += GetString();
		break;
	case Token::KEYWORD:
		sBuffer = "Keyword: ";
		sBuffer += GetKeyword();
		break;
	case Token::VARIABLE:
		sBuffer = "Variable: ";
		sBuffer += GetVariable();
		break;
	case Token::T_GUID:
		{
			sBuffer = "Guid: ";
			sBuffer += "{";
			sprintf(pszBuffer, "%.8x", m_pguidGuid->Data1);
			sBuffer += pszBuffer;
			sBuffer += "-";
			sprintf(pszBuffer, "%.4x", m_pguidGuid->Data2);
			sBuffer += pszBuffer;
			sBuffer += "-";
			sprintf(pszBuffer, "%.4x", m_pguidGuid->Data3);
			sBuffer += pszBuffer;
			sBuffer += "-";
			sprintf(pszBuffer, "%.2x", m_pguidGuid->Data4[0]);
			sBuffer += pszBuffer;
			sprintf(pszBuffer, "%.2x", m_pguidGuid->Data4[1]);
			sBuffer += pszBuffer;
			sBuffer += "-";
			for(int i = 2; i < 8; i++)
			{
				sprintf(pszBuffer, "%.2x", m_pguidGuid->Data4[i]);
				sBuffer += pszBuffer;
			}
			sBuffer += "}";
		}
		break;
	case Token::OPERATOR:
		sBuffer = "Operator: ";
		if(string("\n") == GetOperator())
			sBuffer += "<newline>";
		else
			sBuffer += GetOperator();
		break;
	case Token::BOOLEAN:
		if(GetBoolean())
		{  sBuffer = "Boolean: true";  }
		else
		{  sBuffer = "Boolean: false";  }
		break;
	};

	return sBuffer.c_str();
}

/*
//-----------------------------------------------------------
// Mighty stream operators
//-----------------------------------------------------------
ostream& operator<<( std::ostream& os, Token& tok )
{
	// start with line number and token type
#ifdef TOKEN_DEBUGGING_INFO
	os << tok.GetLineNumber() << ' ' 
#endif
	os << tok.GetType() << endl;
	// now, write the type
	switch( tok.GetType() )
	{
	case Token::KEYWORD:
		os << tok.GetKeyword() << endl;
		break;
	case Token::OPERATOR:
		os << tok.GetOperator() << endl;
		break;
	case Token::VARIABLE:
		os << tok.GetVariable() << endl;
		break;
	case Token::STRING:
		os << tok.GetString() << endl;
		break;
	case Token::INTEGER:
		os << tok.GetInteger() << endl;
		break;
	case Token::REAL:
		os << tok.GetReal() << endl;
		break;
	case Token::BOOLEAN:
		os << (int)(tok.GetBoolean()) << endl;
		break;
	case Token::T_GUID:
		os << tok.GetGuid().Data1 << ' ' << tok.GetGuid().Data2 << ' ' 
		   << tok.GetGuid().Data3 << ' ' << tok.GetGuid().Data4 << endl;
		break;
	default:
		assert(0);	// weird
		os << endl;	// skip a line anyway
	}

	return os;
}

istream& operator>>( std::istream& is, Token& tok )
{
	// this is a bit of a pain because I can't figure out how to get the friend declaration to work
	// for stream operators :-(  so we must use accessors.  Would be better to have direct access
	// as lots of string copying would go away!

	// clear anything out of existing token
	tok.Destroy();
	tok.Clear();

	// start with line number and token type
	int iTemp;
	char c;
//	Token::TOKEN_TYPE	eType;

	is >> iTemp;
	tok.SetLineNumber(iTemp);
	is >> iTemp;
	is.get(c);	// get rid of newline character
	assert(c == '\n');	// um, it was a newline character, right?
//	eType = ;

	static char buf[256];		// slightly bad here that we have a size limit, but hard to do otherwize with stream operations

	switch( (Token::TOKEN_TYPE)iTemp )
	{
	case Token::KEYWORD:
		is.getline(buf, 256);
		tok.CreateKeyword(buf);
		break;
	case Token::OPERATOR:
		is.getline(buf, 256);
		tok.CreateOperator(buf);
		break;
	case Token::VARIABLE:
		is.getline(buf, 256);
		tok.CreateVariable(buf);
		break;
	case Token::STRING:
		is.getline(buf, 256);
		tok.CreateString(buf);
		break;
	case Token::INTEGER:
		is >> iTemp;
		tok.CreateInteger(iTemp);
		break;
	case Token::REAL:
		{
			double fTemp;
			is >> fTemp;
			tok.CreateReal(fTemp);
		}
		break;
	case Token::BOOLEAN:
		is >> iTemp;
		tok.CreateBoolean((bool)iTemp);
		break;
	case Token::T_GUID:
		{
			GUID guid;
			is >> guid.Data1 >> guid.Data2 >> guid.Data3 >> guid.Data4;
			tok.CreateGuid(guid);
			// get to end of line (for some reason is is left pointing at the start of Data4 - ??)
			is.getline(buf, 256);
		}
		break;
	default:
		assert(0);	// weird
	}

	return is;
}
*/
string TokenList::GetDescriptiveString(TokenListItor itr) const
{
	string sBuffer(itr->GetDescriptiveString());

#ifdef TOKEN_DEBUGGING_INFO
	if(itr->GetLineNumber() != 0)
	{
		char cBuffer[64];
		sBuffer += " at line ";
		itoa(itr->GetLineNumber(), cBuffer, 10);
		sBuffer += cBuffer;
		sBuffer += " in file ";
		sBuffer += m_aFileRef[itr->GetFileIndex()];
	}
#endif

	return sBuffer;
}

/*
string TokenList::GetFileName(TokenListItor itr) const
{
	string sBuffer;

	return sBuffer;
}
*/

string TokenList::GetFileName(int index) const
{
	return m_aFileRef[index];
}

#ifdef TOKEN_DEBUGGING_INFO
int TokenList::GetLineNumber(TokenListItor itr) const
{
	return itr->GetLineNumber();
}
#endif

int TokenList::GetFileIndex(std::string sFileName)
{
	for(int i = 0; i < m_aFileRef.size(); i++)
	{
		if(m_aFileRef[i] == sFileName)
			return i;
	}
	m_aFileRef.push_back(sFileName);
	return m_aFileRef.size() - 1;
}

int TokenList::GetNumFileRefs() const
{
	return m_aFileRef.size();
}

void TokenList::clear()
{
	m_aFileRef.clear();
	list<Token>::clear();
}
