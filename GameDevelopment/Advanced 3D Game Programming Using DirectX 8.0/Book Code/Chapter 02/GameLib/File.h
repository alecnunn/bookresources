/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: File.h
 *    Desc: File class, automates some tasks for file i/o
 *          simple wrapper for the c file calls
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _FILE_H
#define _FILE_H

#include <windows.h>
#include <assert.h>

#include <queue>
#include <string>

//==========--------------------------  Exceptions thrown by the cFile class

class cFileError
{
protected:
	std::string m_filename;
public:
	cFileError( const char* filename )
	{
		m_filename = std::string( filename );
	}
	const char* Name()
	{
		return m_filename.c_str();
	}
};

// Thrown by Open.
class cFileNotFound : public cFileError
{
public:
	cFileNotFound( const char* filename ) : 
	  cFileError( filename )
	{	
	}
};

class cFileCannotCreate : public cFileError
{
public:
	cFileCannotCreate( const char* filename ) : 
	  cFileError( filename )
	{	
	}
};

class cFileEOF { };

class cFileReadError { };

class cFileWriteError { };

//==========--------------------------  the cFile class

class cFile  
{
protected:

	FILE* m_fp;
	std::string m_filename;

public:

	cFile();
	~cFile();

	// Discard if exists
	void Create( const char* filename );

	// Append if exists, fail if not
	void Append( const char* filename );

	// Read if exists, fail if not
	void Open( const char* filename );

	// Close the file
	void Close();

	// Check to see if a file exists
	static bool Exists( const char* filename );

	// Read directly out of the file 
	void ReadBuff( void* pBuffer, int buffSize );

	// Write directly out to the file 
	void WriteBuff( void* pBuffer, int buffSize );

	// Read in a line of text from the file, assuming we're passed a big enough buffer
	// this can be dangerous.  buffers passed in should be >= 255 in length, to be safe
	void ReadLine( char* pBuffer );
	std::string ReadLine();

	// Convenience function.  filles 'pBuffer' with the next line that doesn't start 
	// with the provided comment character
	void ReadNonCommentedLine( char* pBuffer, char commentChar );

	// Takes the next line of non-commented text and parses it using " \t\n" 
	// as delimiters
	void TokenizeNextNCLine( std::queue< std::string >* pList, char commentChar = '#' );

	// so basic structures can be automatically read and written
	template <class type>
	void Read( type* data )
	{
		ReadBuff( data, sizeof( type ) );
	}

	template <class type>
	void Write( type& data )
	{
		WriteBuff( &data, sizeof( type ) );
	}
};

#endif //_FILE_H
