/* Copyright (C) Scott Wakeling, 2001.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Wakeling, 2001"
 */

#ifndef __CDTIBIN_H
#define __CDTIBIN_H

class CdtiBin
{

public:
	CdtiBin() : m_FP(NULL), m_bOpen(false), m_bLoading(false), m_bSaving(true) {};
	virtual ~CdtiBin() { Close(); };

	FILE*	Open( char* szFilename );
	bool	Close();

	bool	SetPos( const __int64 i64Offset );
	bool	GetPos( __int64& i64Offset );
	
	void	Stream( void* pData, int iByteCount );

	friend CdtiBin& operator<<( CdtiBin& bin, int& data )
	{
		bin.Stream( &data, sizeof(int) );
		return bin;
	}

	friend CdtiBin& operator<<( CdtiBin& bin, float& data )
	{
		bin.Stream( &data, sizeof(float) );
		return bin;
	}

	void	IsLoading() { m_bLoading = true; m_bSaving = false; }
	void	IsSaving() { m_bSaving = true; m_bLoading = false; }

protected:
	FILE*	m_FP;
	bool	m_bOpen;
	bool	m_bLoading;
	bool	m_bSaving;

};

FILE* CdtiBin::Open( char* szFilename )
{
	if ( m_bSaving )
	{
		// Create a new store
		m_FP = fopen( szFilename, "w+b" );
	}
	else
	{
		// Open an existing store to load from
		m_FP = fopen( szFilename, "r+b" );
	}
	
	if ( m_FP )
	{
		m_bOpen = true;
	}
	
	return m_FP;
}

bool CdtiBin::Close()
{
	if ( m_FP )
	{
		if ( !fclose( m_FP ) )
		{
			m_FP = NULL;
			m_bOpen = false;
			
			return true;
		}
	}

	return false;
}

bool CdtiBin::SetPos( const __int64 i64Offset )
{
	bool bSuccess = false;
		
	if ( m_FP && m_bOpen )
	{
		if ( fsetpos( m_FP, &i64Offset ) == 0 )
		{
			bSuccess = true;
		}
	}

	return bSuccess;
}

bool CdtiBin::GetPos( __int64& i64Offset )
{
	bool bSuccess = false;
		
	if ( m_FP && m_bOpen )
	{
		if ( fgetpos( m_FP, &i64Offset ) == 0 )
		{
			bSuccess = true;
		}
	}

	return bSuccess;
}

void CdtiBin::Stream( void* pData, int iByteCount )
{
	// This is our read/write routine, depending on m_bLoading/m_bSaving
	if ( m_bOpen )
	{
		if ( m_bSaving )
		{
			fwrite( pData, iByteCount, iByteCount, m_FP );	
		}
		else
		{
			fread( pData, iByteCount, iByteCount, m_FP );
		}
	}
}

#endif