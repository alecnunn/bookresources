// GAAverager Implementation
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
#include "GAAverager.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAAverager() { return( new CGAAverager); }
const CClass CGAAverager::classCGAAverager(CID_GAAverager, CID_GAProcessor, "CGAAverager", CreateCGAAverager);
const IClass*	CGAAverager::GetClass() { return((const IClass*)&classCGAAverager); }


// CTOR
CGAAverager::CGAAverager()
{
	SetName("Averager");
   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut); // NOT inplace
	AppendPin(SampleIn);
	AppendPin(Duration); // average size

	m_fSum = 0.0f;
	m_fRecent = 0.0f;
	m_nDuration = 2; // in samples
	m_nSpanIndex = 0;

}

// DTOR
CGAAverager::~CGAAverager()
{
	Terminate();
}

// explicit terminate baseclass
void CGAAverager::Terminate()
{

	CGAProcessor::Terminate();
}




// set the delay in samples
void CGAAverager::SetDuration(UINT32 nDuration)
{
	if(nDuration > 1)
		m_nDuration = nDuration;
}

UINT32 CGAAverager::GetDuration()
{
	return(	m_nDuration );
}

void CGAAverager::SetDurationTime(FLOAT32 fDurationTime)
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nDuration = fSR * fDurationTime;

	SetDuration(nDuration);
	
}


FLOAT32 CGAAverager::GetDurationTime()
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fDurationTime = fSR / m_nDuration;
	return(	fDurationTime );
}

BOOL CGAAverager::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Duration") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetDurationTime(fValue);
		b = TRUE;
	}

	return(b);
}


UINT32 CGAAverager::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn = GetSignalData(SampleIn);

	FLOAT32 fSample = 0.0f;
	//UINT32 nDuration = 

	if(pfIn != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{

			fSample = pfIn[i];

			m_fSample = (m_fSum - m_fRecent + fSample ) / (FLOAT32)m_nDuration;
			

			if(m_nSampleCounter < m_nDuration)
				m_fRecent = 0.0f;
			else 
				m_fRecent = m_fSum;

			pfOut[i] = m_fSample;


			m_nSampleCounter++;

		}
	}

	return(0);

}





#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
