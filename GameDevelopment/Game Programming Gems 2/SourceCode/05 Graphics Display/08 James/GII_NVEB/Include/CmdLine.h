/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Include
File:  CmdLine.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#include <vector>
#include <assert.h>
#include <SimpleString.h>
#include <map>

//  This class represents a set of command line options
//  of the form : /A B

//   The options are treated as SimpleStrings so they can be
//  easily manipulated and treated as ints, floats, etc

class CmdLine
{
	public :

	typedef std::pair< SimpleString, SimpleString > Option;
	typedef std::map< SimpleString, SimpleString > OptionMap;

	CmdLine()
	{

	}

	CmdLine( TCHAR* pCmdline )
	{
		mEXEPath = "";

		std::vector< SimpleString > aVector;

		Tokenize( pCmdline, aVector );

		int argc = aVector.size();

		mEXEPath = aVector[ 0 ];

		for ( int i = 1; i < argc; ++i )
		{
			SimpleString Simplearg1( aVector[ i ] );
			SimpleString Simplearg2;

			Simplearg1.MakeUpper();

			if ( i < ( argc - 1 ) )
			{
				Simplearg2 = aVector[ i + 1 ];
				Simplearg2.MakeUpper();
			}

			if ( Simplearg2( 0 ) == '/' )
			{
				mOptionMap.insert( Option( Simplearg1, Simplearg2 ) );
			}
			else
			if ( Simplearg2 != "" )
			{
				mOptionMap.insert( Option( Simplearg1, Simplearg2 ) );
				i += 1;
			}
		}
	}

	CmdLine( int argc, char* argv[] )
	{
		mEXEPath = argv[ 0 ];

		for ( int i = 1; i < argc; ++i )
		{
			SimpleString Simplearg1( argv[ i ] );
			SimpleString Simplearg2;

			Simplearg1.MakeUpper();

			if ( i < ( argc - 1 ) )
			{
				Simplearg2 = argv[ i + 1 ];
				Simplearg2.MakeUpper();
			}

			if ( Simplearg2( 0 ) == '/' )
			{
				mOptionMap.insert( Option( Simplearg1, Simplearg2 ) );
			}
			else
			if ( Simplearg2 != "" )
			{
				mOptionMap.insert( Option( Simplearg1, Simplearg2 ) );
				i += 1;
			}
		}
	}

	SimpleString GetEXEPath() const
	{
		return mEXEPath;
	}

	void Clear()
	{
		mEXEPath.Clear();
		mOptionMap.clear();
	}

	bool GetOption( const SimpleString& theOption, SimpleString& theSetting ) const
	{
		SimpleString theTemp( theOption );
		theTemp.MakeUpper();

		OptionMap::const_iterator iter = mOptionMap.find( theTemp );

		theSetting.Clear();

		if ( iter != mOptionMap.end() )
		{
			theSetting = (*iter).second;
			return true;
		}
		return false;
	}

	bool SetOption( const SimpleString& theOption, const SimpleString& theSetting )
	{
		SimpleString theTemp( theOption );
		SimpleString theSetting2( theSetting);

		theTemp.MakeUpper();
		theSetting2.MakeUpper();

		OptionMap::iterator iter = mOptionMap.find( theTemp );

		if ( iter != mOptionMap.end() )
		{
			(*iter).second = theSetting2;
			return true;
		}
		return false;
	}


	private :
		OptionMap mOptionMap;

		SimpleString mEXEPath;
};


#endif  _CMDLINE_H_