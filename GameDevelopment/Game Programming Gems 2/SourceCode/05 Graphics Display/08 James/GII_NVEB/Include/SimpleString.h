/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _SIMPLESTRING_H_
#define _SIMPLESTRING_H_

#include <vector>
#include <assert.h>

///////////////////////////////////////////////////////////
// This class represents a growable array of uppercase characters 
// It always appends a 0x00 to the end of the string for use
// In functions that want a char*							
// It will delete the data when the Simple String is deleted

class SimpleString
{
	private :	
		std::vector< TCHAR > mData;

	public :
		SimpleString()
		{
			mData.reserve( 4 );
			mData.push_back( 0x00 );
		}
		SimpleString( const SimpleString& rhs )
		{
			(*this) = rhs;
		}
		SimpleString& operator=( const SimpleString& rhs )
		{
			if ( this != &rhs )
			{
				mData = rhs.mData;
			}
			return *this;
		}

		void MakeUpper()
		{
			for ( int i = 0; i < mData.size() - 1; ++i )
			{
				mData[ i ] = toupper( mData[ i ] );
			}
		}

		void MakeLower()
		{
			for ( int i = 0; i < mData.size() - 1; ++i )
			{
				mData[ i ] = tolower( mData[ i ] );
			}
		}

		bool EndsWith( const SimpleString& rhs ) const 
		{
			if ( rhs.Size() > Size() ) return false;

			for ( unsigned int i = 0;
			      i <= rhs.Size();
			      ++i )
			{
				if ( rhs.mData[ rhs.Size() - i ] != mData[ Size() - i ] ) 
				{
					return false;
				}
			}
			return true;
		}

		unsigned int Size() const
		{
			return ( mData.size() - 1 );
		}

		SimpleString( const TCHAR* pChar )
		{
			mData.reserve( 4 );
			if ( pChar )
			{
				for ( const TCHAR* pC = pChar;
				      *pC != 0x00;
					  ++pC )
				{
					mData.push_back( toupper( *pC ) );
				}
			}
			mData.push_back( 0x00 );
		}

		explicit SimpleString( const TCHAR& aChar )
		{
			mData.reserve( 4 );
			mData.push_back( toupper( aChar ) );
			mData.push_back( 0x00 );
		}

		SimpleString& operator+=( const SimpleString& rhs )
		{
			for ( unsigned int i = 0; i < rhs.CharacterCount(); ++i )
			{
				(*this) += rhs[ i ];
			}
			return (*this);
		}

		SimpleString& operator+=( const TCHAR& aChar )
		{
			mData[ mData.size() - 1 ] = toupper( aChar );
			mData.push_back( 0x00 );
			return (*this);
		}
		SimpleString operator+( const SimpleString& rhs ) const
		{
			SimpleString temp( *this );
			temp += rhs;
			return temp;
		}
		SimpleString operator+( const TCHAR& rhs ) const
		{
			SimpleString temp( *this );
			temp += rhs;
			return temp;
		}

		int CharacterCount() const
		{
			return mData.size() - 1;
		}
		void Clear()
		{
			mData.clear();
			mData.push_back( 0x00 );
		}

		operator TCHAR*()
		{
			return &( mData[ 0 ] );
		}

		operator const TCHAR*() const 
		{
			return &( mData[ 0 ] );
		}

		float AsFloat() const
		{
			return atof( &(mData[ 0 ]) );
		}
		int AsInt() const
		{
			return atoi( &(mData[ 0 ]) );
		}

		long AsLong() const
		{
			return atol( &(mData[ 0 ]) );
		}

		bool IsNull() const
		{
			return ( mData[ 0 ] == 0x00 );
		}

		const TCHAR& operator()( const unsigned int& anInt ) const
		{
			assert( anInt < mData.size() );
			return mData[ anInt ];
		}

		const bool operator!=( const SimpleString& rhs ) const
		{
			return !( *this == rhs );
		}
		bool operator!() const
		{
			return ( mData[ 0 ] != 0x00 );
		}

		bool operator!=( const TCHAR* const rhs ) const
		{
			for ( unsigned int i = 0; ( i < mData.size() ) && ( rhs[ i ] != 0x00 ); ++i )
			{
				if ( mData[ i ] != rhs[ i ] ) return true;
			}
			return ( i != Size() );
		}


		bool operator==( const TCHAR* const rhs ) const
		{
			if ( !rhs ) return false;

			for ( unsigned int i = 0; ( i < mData.size() ) && ( rhs[ i ] != 0x00 ); ++i )
			{
				if ( mData[ i ] != rhs[ i ] ) return false;
			}
			return (  i == Size() );
		}

		bool operator==( const SimpleString& rhs ) const
		{
			if ( mData.size() == rhs.mData.size() )
			{
				for ( unsigned int i = 0; i < mData.size(); ++i )
				{
					if ( mData[ i ] != rhs.mData[ i ] ) return false;
				}
				return true;
			}
			return false;
		}

		bool operator<( const SimpleString& rhs ) const
		{
			unsigned int theMin = min( mData.size(), rhs.mData.size() );

			for ( unsigned int i = 0; i < theMin; ++i )
			{
				if ( mData[ i ] > rhs.mData[ i ] ) return false;
				if ( mData[ i ] < rhs.mData[ i ] ) return true;
			}
			return false;
		}
};

inline
void Tokenize( TCHAR* pCmdLine, std::vector< SimpleString >& aStringVector )
{
	aStringVector.clear();

	SimpleString aString;

	const char* pChar = pCmdLine;

	while ( *pChar != 0x00 )
	{
		while ( ( (*pChar) != ' ' ) && ( *pChar != 0x00 ) )
		{
			aString += (*pChar);
			++pChar;
		}

		while ( ( (*pChar) == ' ' ) && ( *pChar != 0x00 ) )
		{
			++pChar;
		}
		if ( aString.Size() != 0 )
		{
			aStringVector.push_back( aString );
			aString.Clear();
		}
	}
}


#endif  _SIMPLESTRING_H_
