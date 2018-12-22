// Tokenizer.cpp: A class that tokenizes text data.
// Date: 07/02/04
// Created by: Aurelio Reis

#include "Tokenizer.h"
using namespace AURELIO_REIS;

#include <Windows.h>


CTokenizer *CTokenizer::m_pInstance = NULL;


// Constructor.
CTokenizer::CTokenizer() { m_iCurrentToken = NULL; m_iCurLine = 1; }

// Destructor.
CTokenizer::~CTokenizer()
{
	DestroyTokens();
}

// Try to decide what kind of token this may be by it's first character.
TOKENTYPE CTokenizer::IsDelimiter( char chTok, const char *strDelims, int iNumDelims )
{
	if ( chTok == '\n' )
	{
		m_iCurLine++;
	}

	// Figure out what kind of token this character might be.
	for ( int i = 0; i < iNumDelims; i++ )
	{
		if ( chTok == strDelims[i] )
		{
			return TKT_DELIMITER;
		}
	}

	// Last check, is this a null terminator?
	if ( chTok == '\0' )
	{
		return TKT_DELIMITER;
	}

	return TKT_UNKNOWN;
}

// Allocate a new token.
CToken *CTokenizer::AllocToken( TOKENTYPE Type, const char *strText )
{
	CToken *pTok;
	switch ( Type )
	{
		case TKT_FLOAT:
			if ( strText == NULL )
				return NULL;
			pTok = new CTokenFloat( strText );
			pTok->m_iCurLine = m_iCurLine;
			return pTok;

		case TKT_STRING:
			if ( strText == NULL )
				return NULL;
			pTok = new CTokenString( strText );
			pTok->m_iCurLine = m_iCurLine;
			return pTok;

		case TKT_OPENBRACE:
			pTok = new CTokenOpenBrace();
			pTok->m_iCurLine = m_iCurLine;
			return pTok;

		case TKT_CLOSEDBRACE:
			pTok = new CTokenClosedBrace();
			pTok->m_iCurLine = m_iCurLine;
			return pTok;

		case TKT_QUOTE:
			pTok = new CTokenQuote();
			pTok->m_iCurLine = m_iCurLine;
			return pTok;

		case TKT_OPENBRACKET:
			pTok = new CTokenOpenBracket();
			pTok->m_iCurLine = m_iCurLine;
			return pTok;

		case TKT_CLOSEDBRACKET:
			pTok = new CTokenClosedBracket();
			pTok->m_iCurLine = m_iCurLine;
			return pTok;
	}

	return NULL;
}

// Tokenize this String.
bool CTokenizer::TokenizeString( const char *strString, unsigned long ulLen, unsigned long ulFlags, const char *strDelims )
{
	int iNumDelims = (int)strlen( strDelims );

	// Destroy any tokens that may have already existed.
	DestroyTokens();
	m_iCurrentToken = 0;

	TOKSTATE CurrentState = TKS_INWHITE;
	CToken *pTok = NULL;
	int iCurPos = 0;
	char strTempBuff[ 512 ] = { 0 };
	int iTempBuffSize = 0;
	char byCurChar;
	char byNxtChar;
	bool bSpecialChar = false;

	while ( iCurPos < (int)ulLen )
	{
		byCurChar = strString[ iCurPos++ ];
		byNxtChar = strString[ iCurPos ];

		switch ( CurrentState )
		{
			// In Whitespace.
			case TKS_INWHITE:
				if ( IsDelimiter( byCurChar, strDelims, iNumDelims ) )
				{
					continue;
				}

				switch ( byCurChar )
				{
					// Is this the beginning of a comment?
					case '/':
						// Check the next character and see if it is a comment...
						if ( byNxtChar == '/' )
						{
							CurrentState = TKS_INSINGLECOMMENT;
						}
						// Check to see if this is a multi-line comment.
						else if ( byNxtChar == '*' )
						{
							CurrentState = TKS_INMULTICOMMENT;
						}
						continue;

					// If we're quotes...
					case '"':
						CurrentState = TKS_INQUOTES;
						continue;

					// If we're open brace...
					case '{':
						// Make the token and throw it into the list.
						pTok = AllocToken( TKT_OPENBRACE );
						if ( pTok )
						{
							m_TokenList.push_back( pTok );
						}
						continue;

					// If we're closed brace...
					case '}':
						// Make the token and throw it into the list.
						pTok = AllocToken( TKT_CLOSEDBRACE );
						if ( pTok )
						{
							m_TokenList.push_back( pTok );
						}
						continue;
				}

				// If we got here then this is not a delimiter, this is text...
				CurrentState = TKS_INTEXT;
				iCurPos--;	// Get this character in the next run.
				break;

			// In Text.
			case TKS_INTEXT:
				// Check for delimiters to stop getting text.
				bSpecialChar = false;
				if ( !!IsDelimiter( byCurChar, strDelims, iNumDelims ) || ( bSpecialChar = !!IsSpecialCharacter( byCurChar ) ) )
				{
					// Make the state machine come back to this special character.
					if ( bSpecialChar )
					{
						if ( strlen( strTempBuff ) > 0 )
						{
							iCurPos--;
						}
						else
						{
							strTempBuff[ 0 ] = byCurChar;
						}
					}

					FinalizeToken( strTempBuff, ulFlags );
					iTempBuffSize = 0;
					memset( strTempBuff, 0, sizeof( strTempBuff ) );
					CurrentState = TKS_INWHITE;

					continue;
				}
				if ( iTempBuffSize < 512 )
				{
					strTempBuff[ iTempBuffSize++ ] = byCurChar;
				}
				break;

			// In Quotations.
			case TKS_INQUOTES:
				// If we found the end of the quote...
				if ( byCurChar == '"'  )
				{
					// Make the token and throw it into the list.
					pTok = AllocToken( TKT_STRING, strTempBuff );
					if ( pTok )
					{
						m_TokenList.push_back( pTok );
					}
					iTempBuffSize = 0;
					memset( strTempBuff, 0, 512 );

					// NOTE: See note at previous quote token note.
					// Now make the next Quote Token.
					/*pTok = AllocToken( TKT_QUOTE );
					if ( pTok )
					{
						m_TokenList.push_back( pTok );
					}*/

					CurrentState = TKS_INWHITE;
					continue;
				}
				strTempBuff[ iTempBuffSize++ ] = byCurChar;
				break;

			// In single-line comment.
			case TKS_INSINGLECOMMENT:
				// Check to see if the comment has ended (we've reached the end of line).
				if ( byCurChar == '\n' )
				{
					CurrentState = TKS_INWHITE;
					m_iCurLine++;
				}
				break;

			// In multi-line comment.
			case TKS_INMULTICOMMENT:
				// Check to see if the comment is ending.
				if ( byCurChar == '/' )
				{
					// Check previous character.
					if ( strString[ iCurPos - 2 ] == '*' )
					{
						CurrentState = TKS_INWHITE;
					}
				}
				break;
		};
	}

	// We're done with the string but we're left over with a token.
	if ( iTempBuffSize )
	{
		FinalizeToken( strTempBuff, ulFlags );
	}

	return true;
}

// Tokenize the contents of this File.
bool CTokenizer::TokenizeFile( const char *strFileName, unsigned long ulFlags, const char *strDelims )
{
	unsigned char *pData = NULL;
	char *pFileData = NULL;
	DWORD dwFileSize = 0;

	// Open the file.
	HANDLE hFile = CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		
	if ( hFile == INVALID_HANDLE_VALUE ) 
	{ 
		return false;
	}

	// Grab all the file data.
	dwFileSize = GetFileSize( hFile, 0 );
	DWORD dwBytesRead = 0;
	pFileData = new char[dwFileSize + 1];
	ReadFile( hFile, pFileData, dwFileSize, &dwBytesRead, NULL );
	pFileData[dwFileSize] = '\0';

	// Close the file.
	CloseHandle( hFile );

	// Tokenize the file contents.
	bool bRet = TokenizeString( pFileData, dwFileSize, ulFlags, strDelims );

	// Release file data.
	delete [] pFileData;		pFileData = NULL;

	return bRet;
}

// Destroy the current tokens list (freeing all memory).
void CTokenizer::DestroyTokens()
{
	CToken *pTok = NULL;
	if ( m_TokenList.size() )
	{
		for ( vector<CToken *>::iterator iter = m_TokenList.begin(); iter != m_TokenList.end(); ++iter )
		{
			if ( (*iter) )
			{
				delete (*iter);
			}
		}
		m_TokenList.clear();
	}
}

// Determine what type of token this is.
TOKENTYPE CTokenizer::ClassifyToken( const char *strText, unsigned long ulFlags )
{
	TOKENTYPE TokenType;

	// See if this is a number (first character is a number or first is '-' and second is num).
	if ( !( ulFlags & TKFLAG_STRINGSONLY ) && ( isdigit( strText[0] ) ||
		  ( strlen( strText ) >= 2 && ( strText[0] == '-' && isdigit( strText[1] ) ) ) ) )
	{
		return TKT_FLOAT;
	}
	else if ( TokenType = IsSpecialCharacter( strText[ 0 ] ) )
	{
		return TokenType;
	}
	else
	{
		return TKT_STRING;
	}
}

// Finalize a token (determine it's type and make it) and add it to the list.
TOKENTYPE CTokenizer::FinalizeToken( const char *strText, unsigned long ulFlags )
{
	CToken *pTok = AllocToken( ClassifyToken( strText, ulFlags ), strText );

	// Throw the token into the list.
	if ( pTok )
	{
		m_TokenList.push_back( pTok );
		return pTok->m_Type;
	}

	return TKT_UNKNOWN;
}

// Whether this token is of the type asked for.
bool CTokenizer::IsToken( CToken *pTok, TOKENTYPE Type )
{
	if ( !pTok )
		throw( "INVALID TOKEN POINTER!" );

	if ( pTok->m_Type != Type )
		throw( "INVALID TOKEN TYPE!" );

	return true;
}

// Get's a tokens data and sends it back.
bool CTokenizer::GetToken( CToken *pTok, TOKENTYPE Type, void **ppData )
{
	if ( !IsToken( pTok, Type ) )
		return false;

	switch ( Type )
	{
		case TKT_FLOAT:
			if ( ppData == NULL )
				return false;
			(*(float *)ppData) = static_cast< CTokenFloat * >( pTok )->m_fValue;
			return true;

		case TKT_STRING:
			(*(char **)ppData) = static_cast< CTokenString * >( pTok )->m_strValue;
			return true;

		case TKT_OPENBRACE:
			return true;

		case TKT_CLOSEDBRACE:
			return true;

		case TKT_QUOTE:
			return true;

		case TKT_OPENBRACKET:
			return true;

		case TKT_CLOSEDBRACKET:
			return true;
	}

	return false;
}

// Is this a special character already existing as one of our tokens (i.e. { or [ or ").
TOKENTYPE CTokenizer::IsSpecialCharacter( char chTok )
{
	if ( chTok == '{' )
	{	
		return TKT_OPENBRACE;
	}

	if ( chTok == '}' )
	{	
		return TKT_CLOSEDBRACE;
	}

	if ( chTok == '"' )
	{	
		return TKT_QUOTE;
	}

	if ( chTok == '[' )
	{	
		return TKT_OPENBRACKET;
	}

	if ( chTok == ']' )
	{	
		return TKT_CLOSEDBRACKET;
	}

	return TKT_UNKNOWN;
}

// Get the next token we're parsing through (NULL if there is no next token).
CToken *CTokenizer::GetNextToken()
{
	// We're reached the end!
	if ( m_iCurrentToken >= (int)m_TokenList.size() )
	{
//		m_iCurrentToken = 0;	// Restart.
		return NULL;
	}

	return m_TokenList[ m_iCurrentToken++ ];
}

// Get the previous token. Also moves the current token back.
CToken *CTokenizer::GetPrevToken()
{
	if ( m_iCurrentToken < 1 )
	{
		return NULL;
	}

	m_iCurrentToken--;
	return m_TokenList[ m_iCurrentToken - 1 ];
}

// Get the current token.
CToken *CTokenizer::GetCurrentToken()
{
	return m_TokenList[ m_iCurrentToken - 1 ];
}

// Find the first instance of a token type (returning it's index in the token list or TKT_UNKNOWN if not found).
int CTokenizer::FindTokenType( TOKENTYPE Type )
{
	for ( int i = 0; i < (int)m_TokenList.size(); i++ )
	{
		if ( m_TokenList[ i ]->m_Type == Type )
			return i;
	}

	return TKT_UNKNOWN;
}

// Find a token by name and return the token after it (starts search at the current token position).
CToken *CTokenizer::FindTokenNext( const char *strName )
{
	for ( int i = m_iCurrentToken; i < (int)m_TokenList.size() - 1; i++ )
	{
		CToken *pTok = m_TokenList[ i ];
		if ( TKT_STRING == m_TokenList[ i ]->m_Type &&
			 stricmp( (( CTokenString *) m_TokenList[ i ])->m_strValue, strName ) == 0 )
		{
			return m_TokenList[ i + 1 ];
		}
	}

	return NULL;
}

// Find a token by name and return it's current token index.
int CTokenizer::FindTokenNum( const char *strName )
{
	for ( int i = m_iCurrentToken; i < (int)m_TokenList.size() - 1; i++ )
	{
		CToken *pTok = m_TokenList[ i ];
		if ( TKT_STRING == m_TokenList[ i ]->m_Type &&
			 stricmp( (( CTokenString *) m_TokenList[ i ])->m_strValue, strName ) == 0 )
		{
			return i;
		}
	}

	return -1;
}

// Get a token by index.
CToken *CTokenizer::GetToken( int iTok )
{
	if ( iTok < 0 || iTok >= (int)m_TokenList.size() )
	{
		return NULL;
	}

	return m_TokenList[ iTok ];
}