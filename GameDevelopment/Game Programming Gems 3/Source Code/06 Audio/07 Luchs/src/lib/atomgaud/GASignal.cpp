// GASignal Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GASignal.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASignal() { return( new CGASignal); }
const CClass CGASignal::classCGASignal(CID_GASignal, CID_GAObject, "CGASignal", CreateCGASignal);
const IClass*	CGASignal::GetClass() { return((const IClass*)&classCGASignal); }


// CTOR
CGASignal::CGASignal(UINT32 nCount)
{
	m_pfData = NULL;
	m_nCount = 0;
	
	SetCount(nCount);
}

// DTOR
CGASignal::~CGASignal()
{
	Terminate();
}

// explicit terminate baseclass
void CGASignal::Terminate()
{
	ATOM_FREE(m_pfData);
	CGAObject::Terminate();
}



	
void CGASignal::SetCount(UINT32 nCount)
{
	if(nCount > 0) {
		ATOM_FREE(m_pfData);
		m_nCount = nCount;
		m_pfData = (FLOAT32*)calloc(nCount, sizeof(FLOAT32));
	}
}

UINT32 CGASignal::GetCount()
{
	return(m_nCount);
}

FLOAT32* CGASignal::GetData()
{
	return(m_pfData);
}

// read from source into internal buffer
UINT32 CGASignal::Read(FLOAT32* pSamples, UINT32 nCount)
{
	if(pSamples != NULL && nCount > 0) {

		ATOM_FREE(m_pfData);
		m_nCount = nCount;
		m_pfData = (FLOAT32*)calloc(nCount, sizeof(FLOAT32));

		// copy into buffer
		for(UINT32 i=0;i < m_nCount;i++)
			m_pfData[i] = pSamples[i];
	}

	return(m_nCount);
}

// reads in a file 
// @todo, this currently only handles simple wave files
UINT32 CGASignal::ReadFile( CHAR8* pszFileName)
{
	CGAWaveFile file;
	ATOM_WAVEFILEHEADER* pHeader = NULL;
	UINT32 nCount = 0;

	if(pszFileName != NULL) {

		// make media path, if availble:
		ATOM_CHAR szBuffer[256];
		szBuffer[0]=0;

		IDataLocations* pLocations = g_GASystem.GetDataLocations();
		if(pLocations != NULL) {
			IDataLocation* pLocation = pLocations->FindByType("Audio");

			if(pLocation != NULL) {
				ATOM_CHAR* pszPath = pLocation->GetLocation();
				_tcscpy(szBuffer,pszPath); // todo: make sure last char is '\'
			}
		}

		_tcscat(szBuffer, pszFileName);

		BOOL bSuccess = file.Open(szBuffer); // open read

		if(bSuccess == TRUE) {
			pHeader = file.ReadHeader();

			UINT32 nBytes = pHeader->nDataSize;
			UINT32 nSampleCount = nBytes / 2; //  16bit only @todo:

			if(nSampleCount > 0) {
				FLOAT32* pfSamples = (FLOAT32*)calloc(nSampleCount, sizeof(FLOAT32));
				INT16* pData = (INT16*)calloc(nSampleCount, sizeof(INT16));
				file.Read(pData ,nBytes); // read samples

				// convert to float
				for(UINT32 is=0;is < nSampleCount; is++)
					pfSamples[is] = (FLOAT32) pData[is] / SHRT_MAX;

				// append to systems signal list
				nCount = Read(pfSamples, nSampleCount);

				// free temporary buffers
				free(pData);
				free(pfSamples);
			}
		}
	}


	return(nCount);
}



BOOL CGASignal::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAObject::SetParameter(pszName, pszValue);
	if(pszValue != NULL) {
		if(strcmp(pszName, "File") == 0 ) {
			// read samples from file
			UINT32 n= ReadFile(pszValue);
			
			b = n > 0 ? TRUE : FALSE;
		}
	}
	return(b);
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
 