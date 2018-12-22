// Tokenizer.h: A class that tokenizes text data.
// Date: 12/21/02
// Created by: Aurelio Reis

#ifndef __AR__TOKENIZER__H__
#define __AR__TOKENIZER__H__

#include <vector>
using namespace std;


namespace AURELIO_REIS
{
	enum TOKENTYPE
	{
		TKT_UNKNOWN, TKT_DELIMITER, TKT_FLOAT, TKT_STRING,
		TKT_OPENBRACE, TKT_CLOSEDBRACE, TKT_QUOTE,
		TKT_OPENBRACKET, TKT_CLOSEDBRACKET,
	};

	enum
	{
		// Tell tokenizer to only output string tokens.
		TKFLAG_STRINGSONLY = 0x00000001,
	};


	class CToken
	{
	public:
		TOKENTYPE m_Type;

		// The current line (for error logging).
		int m_iCurLine;

		// Pure Virtual Destructor.
		virtual ~CToken() = 0 {}	
	};


	class CTokenFloat : public CToken
	{
	public:
		float m_fValue;
		CTokenFloat( const char *strText )
		{
			m_Type = TKT_FLOAT;
			m_fValue = (float)atof( strText );
		}
	};


	class CTokenString : public CToken
	{
	public:
		char *m_strValue;
		CTokenString( const char *strText )
		{
			m_Type = TKT_STRING;
			int iSize = (int)strlen( strText ) + 1;
			m_strValue = new char[iSize];
			_snprintf( m_strValue, iSize, strText );
		}
		~CTokenString()
		{
			if ( m_strValue )
			{
				delete [] m_strValue;
				m_strValue = NULL;
			}
		}
	};


	class CTokenOpenBrace : public CToken
	{
	public:
		CTokenOpenBrace() { m_Type = TKT_OPENBRACE; }
	};


	class CTokenClosedBrace : public CToken
	{
	public:
		CTokenClosedBrace() { m_Type = TKT_CLOSEDBRACE; }
	};


	class CTokenQuote : public CToken
	{
	public:
		CTokenQuote() { m_Type = TKT_QUOTE; }
	};


	class CTokenOpenBracket : public CToken
	{
	public:
		CTokenOpenBracket() { m_Type = TKT_OPENBRACKET; }
	};


	class CTokenClosedBracket : public CToken
	{
	public:
		CTokenClosedBracket() { m_Type = TKT_CLOSEDBRACKET; }
	};


	// TODO: Create a more powerful method of handling punctuation (i.e. '.', ';', '()').
	#define		DEFAULT_DELIMS			" \t\r\n,;"


	class CTokenizer
	{
	private:
		// The current token we're looking at (for GetNextToken()).
		int m_iCurrentToken;

	public:
		// Static Singleton Instance.
		static CTokenizer *m_pInstance;

		// The list of tokens.
		vector< CToken * > m_TokenList;

		// The current line (for error logging).
		int m_iCurLine;

		enum TOKSTATE
		{
			TKS_INWHITE, TKS_INTEXT, TKS_INQUOTES, TKS_INSINGLECOMMENT, TKS_INMULTICOMMENT,
		};

		// Constructor.
		CTokenizer();

		// Destructor.
		virtual ~CTokenizer();

		static CTokenizer *GetInstance()
		{
			if ( !m_pInstance )
			{
				m_pInstance = new CTokenizer();
			}

			return m_pInstance;
		}

		static void DestroyInstance()
		{
			if ( m_pInstance )
			{
				m_pInstance->DestroyTokens();
				delete m_pInstance;		m_pInstance = NULL;
			}
		}

		// Try to decide what kind of token this may be by it's first character (Note: Null Terminator is always a delimiter).
		TOKENTYPE IsDelimiter( char chTok, const char *strDelims, int iNumDelims );

		// Allocate a new token.
		CToken *AllocToken( TOKENTYPE Type, const char *strText = NULL );

		// Tokenize this String.
		bool TokenizeString( const char *strString, unsigned long ulLen, unsigned long ulFlags = 0, const char *strDelims = DEFAULT_DELIMS );

		// Tokenize the contents of this File.
		bool TokenizeFile( const char *strFileName, unsigned long ulFlags = 0, const char *strDelims = DEFAULT_DELIMS );

		// Destroy the current tokens list (freeing all memory).
		void DestroyTokens();

		// Determine what type of token this is.
		TOKENTYPE ClassifyToken( const char *strText, unsigned long ulFlags );

		// Finalize a token (determine it's type and make it) and add it to the list.
		TOKENTYPE FinalizeToken( const char *strText, unsigned long ulFlags );

		// Whether this token is of the type asked for.
		bool IsToken( CToken *pTok, TOKENTYPE Type );

		// Get's a tokens data and sends it back.
		bool GetToken( CToken *pTok, TOKENTYPE Type, void **pData = NULL );

		// Is this a special character already existing as one of our tokens (i.e. { or [ or ").
		TOKENTYPE IsSpecialCharacter( char chTok );

		// Get the next token we're parsing through (NULL if there is no next token). Also moves the current token up.
		CToken *GetNextToken();

		// Get the previous token. Also moves the current token back.
		CToken *GetPrevToken();

		// Get the current token.
		CToken *GetCurrentToken();

		// Find the first instance of a token type (returning it's index in the token list or TKT_UNKNOWN if not found).
		int FindTokenType( TOKENTYPE Type );

		// Find a token by name and return the token after it (starts search at the current token position).
		CToken *FindTokenNext( const char *strName );

		// Find a token by name and return it's current token index (starts search at the current token position).
		int FindTokenNum( const char *strName );

		// Set the current token number.
		void SetCurrentTokenNum( int iTok ) { m_iCurrentToken = iTok; }

		// Get the current token number.
		void GetCurrentTokenNum( int &iTok ) const { iTok = m_iCurrentToken; }

		// Get a token by index.
		CToken *GetToken( int iTok );

		// Expect a string.
		bool ExpectString( CToken *pTok, const char *strString )
		{
			if ( !pTok || TKT_STRING != pTok->m_Type ||
				 strcmp( ((CTokenString *) pTok)->m_strValue, strString ) != 0 )
			{
				return false;
			}

			return true;
		}

		// Expect a specific token type.
		bool ExpectTokenType( CToken *pTok, TOKENTYPE Type )
		{
			if ( !pTok || Type != pTok->m_Type )
			{
				return false;
			}

			return true;
		}
	};
};

#endif