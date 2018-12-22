// GATokenizer Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GATokenizer.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGATokenizer() { return( new CGATokenizer); }
const CClass CGATokenizer::classCGATokenizer(CID_GATokenizer, CID_GAObject, "CGATokenizer", CreateCGATokenizer);
const IClass*	CGATokenizer::GetClass() { return((const IClass*)&classCGATokenizer); }


// CTOR
CGATokenizer::CGATokenizer()
{
	m_pszText = NULL;

	peek_pos = 0;
	last_line = 0;
	in_remark = false;
//	tokenize();
}

// DTOR
CGATokenizer::~CGATokenizer()
{
	Terminate();
}

// explicit terminate baseclass
void CGATokenizer::Terminate()
{

	CGAObject::Terminate();
}



bool CGATokenizer::IsOperator(ATOM_CHAR c)
{
	return
		c == ',' ||
		c == '=' ||
		c == '(' ||
		c == ')' ||
		c == ';' ||
		c == '{' ||
		c == '}' ||
		c == '<' ||
		c == '+' ||
		c == '-' ||
		c == '*' ||
		c == '/' ||
		c == '>';
}

bool CGATokenizer::IsSpace(ATOM_CHAR c)
{
	bool b = (
	
		c == ' ' ||
		c == '\t' ||
		c == 13 ||
		c == 10 ) ;

	return(b);
}

bool CGATokenizer::IsRemark(ATOM_CHAR c, int position)
{
	if (in_remark)
	{
		if (c == '\n') 
		{	
			in_remark = false;
		}
		return true;
	}
	else
	{
		if (c == '/')
		{
			if (
				position+1 < m_nSize &&
				m_pszText[position+1] == '/')
			{
				in_remark = true;
				return true;
			}
		}
		else if (c == '#')
		{
			in_remark = true;
			return true;
		}
	}
	return false;
}


/*
std::string CGATokenizer::GetNextToken(int *offset)
{
	peek_pos = 0;
	last_line = m_Tokens[0].line;
	std::string token = m_Tokens[0].token;
	if (offset != NULL) *offset = m_Tokens[0].offset;

	m_Tokens.pop_front();

	return token;
}

std::string CGATokenizer::PeekNextToken(int *offset)
{
	if (peek_pos >= (int) m_Tokens.size())
	{
		return "";
	}
	if (offset != NULL) *offset = m_Tokens[peek_pos].offset;
	return m_Tokens[peek_pos++].token;
}

void CGATokenizer::PopToPeek()
{
	while (peek_pos > 0)
	{
		peek_pos--;
		m_Tokens.pop_front();
	}
}
*/
void CGATokenizer::Dump()
{
	UINT32 nCount = GetTokenCount();

	for (UINT32 i=0; i < nCount; i++)
	{
		ATOM_CHAR*	psz = GetToken(i);
		GADUMP(psz);
	}
}

UINT32	CGATokenizer::GetTokenCount()
{
	return(m_Tokens.size());
}

ATOM_CHAR*	CGATokenizer::GetToken(UINT32 nIndex)
{
	ATOM_CHAR*	psz = NULL;
	UINT32 nCount = GetTokenCount();

	if(nIndex < nCount)
		psz = (ATOM_CHAR*)m_Tokens[nIndex]->GetName();

	return(psz);

}

// tokenize
// @ not optimized (doubled code), but working

void CGATokenizer::Process( ATOM_CHAR* pszText)
{

	m_pszText = pszText;
	m_nSize = _tcslen(m_pszText);
	ATOM_CHAR szBuffer[1024];
	szBuffer[0]=0;
	ATOM_CHAR ch = 0;
	bool bInQuote = false;
	CGAToken* pToken = NULL;
	UINT32 t=0; // tokenpos

	for(UINT32 i=0; i < m_nSize; i++) {

		ch = m_pszText[i];

		if(ch == '"') {
			bInQuote = !bInQuote;
			continue;
		}
		// allow space inside quotes
		if(bInQuote == false) {

			if(IsSpace(ch) || IsOperator(ch) ) {

				if (IsOperator(ch))  {

					if(szBuffer[0] != 0	) {
						pToken = new CGAToken(szBuffer); // store name/value terminated by operator
						m_Tokens.push_back(pToken);
						szBuffer[0] = 0;
						t=0; // reset tokenpos
					}

					szBuffer[t++] = ch;
					szBuffer[t] = 0;
					pToken = new CGAToken(szBuffer); // store operator
					m_Tokens.push_back(pToken);
					szBuffer[0] = 0;
					t=0; // reset tokenpos
				}	

				if(szBuffer[0] != 0	) {
					pToken = new CGAToken(szBuffer); // store name/value terminated by space
					m_Tokens.push_back(pToken);
					szBuffer[0] = 0;
					t=0; // reset tokenpos
				}
				continue;

			}
	
		}

		szBuffer[t++] = ch;
		szBuffer[t] = 0;

	

	}

}


/*
std::string CGATokenizer::write_error(std::string err_msg)
{
	CL_String err;
	err << "Error occured during read of config file '" <<
		filename.c_str() << "' (line " << last_line << "): " << err_msg.c_str();

	return err;
}
*/

/*
// tokenize
// @ not optimized (doubled code), but working

void CGATokenizer::Process( ATOM_CHAR* pszText)
{

	m_pszText = pszText;
	m_nSize = _tcslen(m_pszText);
	ATOM_CHAR szBuffer[1024];
	szBuffer[0]=0;
	ATOM_CHAR ch = 0;
	bool bInQuote = false;
	CGAToken* pToken = NULL;
	UINT32 t=0; // tokenpos

	for(UINT32 i=0; i < m_nSize; i++) {

		ch = m_pszText[i];

		if(ch == '"') {
			bInQuote = !bInQuote;
			continue;
		}
		// allow space inside quotes
		if(bInQuote == false) {

			if(IsSpace(ch) || IsOperator(ch) ) {

				if (IsOperator(ch))  {

					if(szBuffer[0] != 0	) {
						pToken = new CGAToken(szBuffer); // store name/value terminated by operator
						m_Tokens.push_back(pToken);
						szBuffer[0] = 0;
						t=0; // reset tokenpos
					}

					szBuffer[t++] = ch;
					szBuffer[t] = 0;
					pToken = new CGAToken(szBuffer); // store operator
					m_Tokens.push_back(pToken);
					szBuffer[0] = 0;
					t=0; // reset tokenpos
				}	

				if(szBuffer[0] != 0	) {
					pToken = new CGAToken(szBuffer); // store name/value terminated by space
					m_Tokens.push_back(pToken);
					szBuffer[0] = 0;
					t=0; // reset tokenpos
				}
				continue;

			}
	
		}

		szBuffer[t++] = ch;
		szBuffer[t] = 0;

	

	}

}
*/


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
 