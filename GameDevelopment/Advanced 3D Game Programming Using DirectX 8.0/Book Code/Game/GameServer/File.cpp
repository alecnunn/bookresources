/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: File.cpp
 *    Desc: File class, automates some tasks for file i/o
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "File.h"

using namespace std;

cFile::cFile() :
	m_fp( NULL )
{
	// nothing to do
}

cFile::~cFile()
{
	// double check to make sure the user closed the file.
	if( m_fp != NULL )
	{
		Close();
	}
}

void cFile::Create( const char* filename )
{
	// Create the file handle
	m_fp = fopen( filename, "w+" );

	// make sure everything went well
	if( m_fp == NULL )
	{
		throw cFileCannotCreate( filename );
	}

	m_filename = string( filename );
}

void cFile::Append( const char* filename )
{
	// Create the file handle
	m_fp = fopen( filename, "ab" );

	// make sure everything went well
	if( m_fp == NULL )
	{
		throw cFileCannotCreate( filename );
	}

	m_filename = string( filename );
}

void cFile::Open( const char* filename )
{
	// Create the file handle
	m_fp = fopen( filename, "r+b" );

	// make sure everything went well
	if( m_fp == NULL )
	{
		throw cFileCannotCreate( filename );
	}

	m_filename = string( filename );

}


void cFile::Close()
{
	if( m_fp == NULL )
		return; // the file is already closed

	if( 0 != fclose( m_fp ) )
	{
		OutputDebugString("Error in cFile::Close\n");
	}

	m_fp = NULL;
}



bool cFile::Exists( const char* filename )
{
	// try to open the file.  if we can't, it must not exist
	cFile temp;

	temp.m_fp = fopen( filename, "r" );

	// make sure everything went well
	if( temp.m_fp == NULL )
	{
		return false;
	}
	temp.Close();
	return true;
}


void cFile::ReadBuff( void* pBuffer, int buffSize )
{
	if( m_fp == NULL )
		return; // error

	//int numRead = fread( pBuffer, buffSize, 1, m_fp );
	int numRead = fread( pBuffer, 1, buffSize, m_fp );

	if( numRead != buffSize )
	{
		if( 0 != feof( m_fp ) )
		{
			throw cFileEOF();
		}
		else if( ferror( m_fp ) )
		{
			throw cFileReadError();
		}
		else
		{
			int foo = 0;
		}
	}
}


void cFile::WriteBuff( void* pBuffer, int buffSize )
{
	if( m_fp == NULL )
		return; // error

	int numWritten = fwrite( pBuffer, buffSize, 1, m_fp );

	if( numWritten != buffSize )
	{
		throw cFileWriteError();
	}
}


/**
 * Not overrun safe
 */
void cFile::ReadLine( char* pBuffer )
{
	char currChar;
	bool done = false;

	int nRead = 0;
	while( !done )
	{
		try
		{
			ReadBuff( &currChar, 1 );
			nRead++;
		}
		catch( cFileEOF )
		{
			// break from the loop, we reached the end-of-file.
			if( nRead == 0 )
			{
				/**
				 * We started at the EOF and can't read a single line.
				 */
				throw;
			}
			break;
		}

		
		if( currChar == '\0' || currChar == '\n' )
		{
			break;
		}
		*pBuffer++ = currChar;
	}
	// end the string with a \n\0
	*pBuffer++ = '\n';
	*pBuffer++ = '\0';
}

/**
 * Overrun safe
 */
string cFile::ReadLine()
{
	char currChar;
	bool done = false;

	int nRead = 0;

	string out;

	while( !done )
	{
		try
		{
			ReadBuff( &currChar, 1 );
			nRead++;
		}
		catch( cFileEOF )
		{
			// break from the loop, we reached the end-of-file.
			if( nRead == 0 )
			{
				/**
				 * We started at the EOF and can't read a single line.
				 */
				throw;
			}
			break;
		}

		if( currChar == '\0' || currChar == '\n' )
		{
			break;
		}

		out += currChar;
	}
	return out;
}


void cFile::ReadNonCommentedLine( char* pBuffer, char commentChar )
{
	char buff[1024];
	buff[0] = 0;
	while( 1 )
	{
		ReadLine( buff );
		if( buff[0] != commentChar )
			break;
	}
	strcpy( pBuffer, buff );
}


void cFile::TokenizeNextNCLine( queue< string >* pList, char commentChar )
{
	string str;
	while(1)
	{
		str = ReadLine();
		if( str[0] != commentChar )
			break;
	}

	// now curr has our string.
	// first, strip off any comments on the end.
	unsigned int commentLoc = str.find( commentChar, 0);
	if( commentLoc != str.npos )
	{
		str = str.erase( commentLoc );
	}

	char sep[] = " \t\n\r";

	unsigned int tokStart = str.find_first_not_of( (char*)sep, 0 );
	unsigned int tokEnd;
	while( tokStart != str.npos )
	{
		tokEnd = str.find_first_of( (char*)sep, tokStart );
		string token = str.substr( tokStart, tokEnd-tokStart );
		pList->push( token );

		tokStart = str.find_first_not_of( (char*)sep, tokEnd );
	}
}
