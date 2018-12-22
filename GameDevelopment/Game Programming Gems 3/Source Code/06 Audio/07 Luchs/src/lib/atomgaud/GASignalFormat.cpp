// GASignal Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	array for objects of class GASignal
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
#include "GASignalFormat.h"


namespace atomos
{
// metaclass implementation
void* __stdcall CreateCGASignalFormat() { return( new CGASignalFormat); }
const CClass CGASignalFormat::classCGASignalFormat(CID_GASignalFormat, CID_GAObject, "CGASignalFormat", CreateCGASignalFormat);
const IClass*	CGASignalFormat::GetClass() { return((const IClass*)&classCGASignalFormat); }


// CTOR
CGASignalFormat::CGASignalFormat()
{
	m_fSampleRate = 44100.0f;
	m_nFrameCount = 256;
	m_nChannelCount = 1;
}

// DTOR
CGASignalFormat::~CGASignalFormat()
{
	Terminate();
}

	
// explicit terminate baseclass
void CGASignalFormat::Terminate()
{
	CGAObject::Terminate();
}

void CGASignalFormat::SetSampleRate(FLOAT32 fSampleRate)
{
	m_fSampleRate = fSampleRate;
}

FLOAT32	CGASignalFormat::GetSampleRate()
{
	return(m_fSampleRate);
}

void CGASignalFormat::SetFrameCount(UINT32 nFrameCount)
{
	m_nFrameCount = nFrameCount;
}

// return frames per buffer in call of Process
UINT32 CGASignalFormat::GetFrameCount()
{
	return(m_nFrameCount);
}

void CGASignalFormat::SetChannelCount(UINT32 nChannelCount)
{
	m_nChannelCount = nChannelCount;
}


UINT32 CGASignalFormat::GetChannelCount()
{
	return(m_nChannelCount);
}


BOOL CGASignalFormat::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAObject::SetParameter(pszName, pszValue);

	if( pszValue != NULL) {
		if(strcmp(pszName, "SampleRate") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetSampleRate(fValue);
			b = TRUE;
		}
		else if(strcmp(pszName, "Frames") == 0) {
			UINT32 nValue = atoi(pszValue);
			SetFrameCount(nValue);
			b = TRUE;
		}
		else if(strcmp(pszName, "Channels") == 0) {
			UINT32 nValue = atoi(pszValue);
			SetChannelCount(nValue);
			b = TRUE;
		}
	}
	return(b);
}





} // namespace atomos
// --------------------------------------------------------------------------------------------------------
