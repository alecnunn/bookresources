/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _SIMPLE_FILE_H_
#define _SIMPLE_FILE_H_

#include <stdio.h>
#include <SimpleString.h>

class SimpleFile
{
	private :
		SimpleString mFileName;

		FILE*   mpFile;

	public :

	SimpleFile( const TCHAR* pFileName = 0 ) : mFileName( pFileName ) , mpFile( 0 )
	{;}

	SimpleFile& operator=( const SimpleFile& rhs );

	bool IsOpen() const
	{
		return ( mpFile != 0 );
	}

	bool OpenForWrite( const SimpleString& aFileName )
	{
		if ( IsOpen() ) 
		{
			Close();
		}
		mFileName = aFileName;
		mpFile = fopen( mFileName, "wb" );

		if ( mpFile )
		{
			fseek( mpFile, 0, 0 );
		}
		return IsOpen();
	}

	bool OpenForRead( const SimpleString& aFileName )
	{
		if ( IsOpen() ) 
		{
			Close();
		}
		mFileName = aFileName;
		mpFile = fopen( mFileName, "rb" );

		if ( mpFile )
		{
			fseek( mpFile, 0, 0 );
		}
		return IsOpen();
	}

	bool Close()
	{
		if ( mpFile )
		{
			int i = fclose( mpFile ), mpFile = 0;
			return( i == 0 );
		}
		return false;
	}

	bool Seek( const unsigned int& thePos )
	{
		if ( !IsOpen() ) return false;
		return ( fseek( mpFile, thePos, 0 ) == 0 ); 
	}

	bool Read( SimpleString& aString )
	{
		if ( !IsOpen() ) return false;

		unsigned int theLength = 0;
		unsigned int theAmount = fread( &theLength, sizeof( unsigned char ), sizeof( theLength ), mpFile );
		
		char* pData = new char[ theLength ];

		theAmount = fread( pData,sizeof( unsigned char ), theLength, mpFile );

		aString = SimpleString( pData );

		delete []pData;

		return true;
	}	

	bool Write( const SimpleString& aString )
	{
		if ( !IsOpen() ) return false;

		unsigned int theLength = aString.Size();
		unsigned int theAmount = fwrite( &theLength, sizeof( unsigned char ), sizeof( theLength ), mpFile );
		fwrite( &( aString( 0 ) ), sizeof( unsigned char ), aString.Size(), mpFile );
		return true;
	}	

	bool Read( void* pData, const unsigned int& theLength )
	{
		if ( !IsOpen() || !pData ) return false;

		unsigned int theAmount = fread( pData, sizeof( unsigned char ), theLength, mpFile );

		return ( theAmount == theLength );
	}

	bool Write( const void* pData, const unsigned int& theLength )
	{
		if ( !IsOpen() || !pData ) return false;

		unsigned int theAmount = fwrite( pData, sizeof( unsigned char ), theLength, mpFile );
		return ( theAmount == theLength );
	}

	virtual ~SimpleFile()
	{
		Close();
	}
};

#endif  _SIMPLE_FILE_H_
