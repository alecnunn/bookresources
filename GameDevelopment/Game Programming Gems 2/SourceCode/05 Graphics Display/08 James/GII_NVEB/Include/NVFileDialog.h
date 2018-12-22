/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _NVFILEDIALOG_H_
#define _NVFILEDIALOG_H_

#include <string>

////////////
// Helper class to assist in loading files
//
// Usage :
//
// Just create a NV*FileDialog object on the stack, and call Open
//
//  NVXFileDialog aDialog;
//
//  std::string theFileName;
//  
//  if ( aDialog.Open( theFileName ) )
//  {
//      // open the filename and read it in
//  }
//
//  // That's it !
//
// Use the NVTextureFileDialog for texture files,
//
// or use the NVFileDialog to do arbitrary filters
//

class NVFileDialog
{
	private :

		OPENFILENAME mOpenFileName;

		std::vector< std::string > mFilterNames;
		std::vector< std::string > mFilterStrings;

		std::string mString;

		void Init()
		{
			memset( &mOpenFileName, 0x00, sizeof( mOpenFileName ) );
			mOpenFileName.lStructSize = sizeof( mOpenFileName );

			OSVERSIONINFO osinfo;
			memset( &osinfo, 0x00, sizeof( osinfo ) );
			BOOL bSuccess = ::GetVersionEx( &osinfo );

			if ( osinfo.dwMajorVersion >= 0x0500 )
			{
				mOpenFileName.lStructSize += ( 3 * sizeof( DWORD ) );
			}

			mString.erase( mString.begin(), mString.end() );

			mOpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_SHAREAWARE;

			mOpenFileName.nFilterIndex = 1;

			for ( int i = 0; i < mFilterNames.size(); ++i )
			{
				mString += mFilterNames[ i ];
				mString += TCHAR(0x00);
				mString += mFilterStrings[ i ];
				mString += TCHAR(0x00);
			}

			// Last element must be double terminated
			mString += TCHAR(0x00);

			mOpenFileName.lpstrFilter = mString.c_str();
		}

	public :

	~NVFileDialog(){;}

	NVFileDialog()
	{
		mFilterNames.push_back("*.*");
		mFilterStrings.push_back("");
	}

	void SetFilters( const std::vector< std::string >& theFilterNames,
					 const std::vector< std::string >& theFilterStrings )
	{
		assert( mFilterNames.size() == theFilterStrings.size() );

		mFilterNames   = theFilterNames;
		mFilterStrings = theFilterStrings;
	}

	void SetFilter( const std::string& theFilterName )
	{
		mFilterNames.clear();
		mFilterStrings.clear();

		mFilterNames.push_back( theFilterName );
		mFilterStrings.push_back( theFilterName );
	}

	virtual bool Open( std::string& theResult )
	{
		Init();

		theResult.resize(1024);
		theResult[0] = 0;
		mOpenFileName.lpstrFile  = &theResult[ 0 ];
		mOpenFileName.nMaxFile   = 1024;

		BOOL bSuccess = ::GetOpenFileName( &mOpenFileName );

		if ( !bSuccess )
		{
			DWORD theFlag = ::CommDlgExtendedError();
			std::string anError;
			anError += theFlag;
			OutputDebugString( anError.c_str() );
			MessageBox( 0, anError.c_str(), "Error", MB_OK );
		}
		return ( bSuccess == TRUE );
	}

};

class NVXFileDialog : public NVFileDialog
{
	public :

	NVXFileDialog()
	{
		SetFilter("*.x");
	}

};


class NVTextureFileDialog : public NVFileDialog
{
	public :

	NVTextureFileDialog()
	{
		SetFilter("*.bmp;*.tga;*.dds");
	}

};

#endif  _NVFILEDIALOG_H_

