// GAFile Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GAFile.h"



#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAFile() { return( new CGAFile); }
const CClass CGAFile::classCGAFile(CID_GAFile, CID_GAObject, "CGAFile", CreateCGAFile);
const IClass*	CGAFile::GetClass() { return((const IClass*)&classCGAFile); }


// CTOR
CGAFile::CGAFile()
{
	m_Handle = NULL;
}

// DTOR
CGAFile::~CGAFile()
{
	Terminate();

}

void CGAFile::Terminate()
{
	Close();
	CGAObject::Terminate();
}	


BOOL CGAFile::Open(const ATOM_CHAR* pszPathName, UINT32 nMode)
{
	BOOL bSuccess = FALSE;

	if(m_Handle != NULL)
		Close(); // when reusing this object


	if( pszPathName != NULL) {

		SetName(pszPathName);

		m_Handle = _topen(pszPathName, nMode);
		if(m_Handle != -1)
			bSuccess = TRUE;

		// set new file to write permission
		if(nMode == IGAFile::CreateBin)
			_tchmod( pszPathName, _S_IWRITE ) ;

	}


	return(bSuccess);
}




void CGAFile::Close()
{
	if(m_Handle) {
		 _close(m_Handle);
		m_Handle = NULL;
	}
}




INT32 CGAFile::Read(void* pBuffer, INT32 nBytesToRead)
{
	INT32 nBytesRead = 0;

	if(pBuffer != NULL &&
		nBytesToRead > 0 && 
		m_Handle != NULL ) {
	
		nBytesRead = _read(m_Handle, pBuffer, nBytesToRead);

	}
	return(nBytesRead);
}




INT32 CGAFile::Write(void* pBuffer, INT32 nBytesToWrite)
{
	INT32 nBytesWritten = 0;

	if(pBuffer != NULL &&
		nBytesToWrite > 0 && 
		m_Handle != NULL ) {

		nBytesWritten = _write(m_Handle, pBuffer, nBytesToWrite);
	}

	return(nBytesWritten);

}

 //returns the offset, in bytes, of the new position from the beginning of 
INT32 CGAFile::Seek(INT32 nPos, UINT32 nMode)
{
	BOOL bSuccess = FALSE;
	INT32 nOffset = -1;

	if(m_Handle) 
		  nOffset = _lseek(m_Handle, nPos, nMode );

	return(nOffset);
}

// @mfunc
// works only for 'small' sizes (INT32)
INT32 CGAFile::GetSize()
{
	INT32 nSize=0;
	if(m_Handle) 
		nSize = _filelength(m_Handle);
		
		return(nSize);
}


// @mfunc
	// lPos == -1 means error
UINT32 CGAFile::GetPosition()
{
	INT32 nPos =  _tell ( m_Handle ); 
   return (nPos);
}


// @mfunc
// check if the next data segment of size lBytes equals pData
// return TRUE if equal
BOOL CGAFile::Compare( const INT8 *pData, INT32 nBytes )
{
	if(pData == NULL || m_Handle == NULL)
		return(FALSE);
	
	INT8 nValue;
	
	for(INT32 i=0;i < nBytes; i++ )
	{
		BOOL b = ReadINT8(&nValue);
		if(b == FALSE || ( nValue != pData[i] ))
			return(FALSE);
		
	}
	
	return (TRUE);
}


// @mfunc
// read the next data segment into a long value
BOOL CGAFile::ReadINT32( INT32* pnValue )
{
	if(pnValue == NULL || m_Handle == NULL)
		return(FALSE);

	BOOL b = FALSE;
	*pnValue = 0; // zero out return value

	INT32 nValue = 0;
	INT32 nRead = _read(m_Handle, &nValue, sizeof(INT32));

	if(nRead == sizeof(INT32)) {
		 b = TRUE;
		*pnValue = nValue;
	}

   return (b);
}

// @mfunc
// read the next data segment into a long value
BOOL CGAFile::ReadINT16( INT16* pnValue )
{
	if(pnValue == NULL || m_Handle == NULL)
		return(FALSE);

	BOOL b = FALSE;
	*pnValue = 0; // zero out return value

	INT16 nValue = 0;
	INT32 nRead = _read(m_Handle, &nValue, sizeof(INT32));

	if(nRead == sizeof(INT16)) {
		 b = TRUE;
		*pnValue = nValue;
	}

   return (b);
}

// @mfunc
// read the next data segment into a long value
BOOL CGAFile::ReadINT8( INT8* pnValue )
{
	if(pnValue == NULL || m_Handle == NULL)
		return(FALSE);

	BOOL b = FALSE;
	*pnValue = 0; // zero out return value

	INT8 nValue = 0;
	INT32 nRead = _read(m_Handle, &nValue, sizeof(INT8));

	if(nRead == sizeof(INT8)) {
		 b = TRUE;
		*pnValue = nValue;
	}

   return (b);
}

// @mfunc
BOOL CGAFile::WriteINT8( INT8 nValue )
{
	if( m_Handle == NULL)
		return(FALSE);

	BOOL b = FALSE;

	INT32 nWrite = _write(m_Handle, &nValue, sizeof(INT8));

	if(nWrite == sizeof(INT8)) {
		 b = TRUE;
	}

   return (b);
}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------


/*
// -- splitpath methods ----------------------------------------------------------

void CAtomCoreFile::GetDrive(CAtomCoreString& str) const
{
   TCHAR szDrive[_MAX_DRIVE];
   _tsplitpath(m_szPathName, szDrive, NULL, NULL, NULL);
   str = szDrive;
}

void CAtomCoreFile::GetDir(CAtomCoreString& str) const
{ 
   TCHAR szDir[_MAX_DIR];
   _tsplitpath(m_szPathName, NULL, szDir,NULL, NULL);
   str = szDir;
}

void CAtomCoreFile::GetTitle(CAtomCoreString& str) const
{
   TCHAR szName[_MAX_FNAME];
   _tsplitpath(m_szPathName, NULL, NULL, szName, NULL);
   str = szName;
}

void CAtomCoreFile::GetExt(CAtomCoreString& str) const
{
   TCHAR szExt[_MAX_EXT];

   _tsplitpath(m_szPathName, NULL, NULL, NULL, szExt);
   str = szExt;
}
*/


