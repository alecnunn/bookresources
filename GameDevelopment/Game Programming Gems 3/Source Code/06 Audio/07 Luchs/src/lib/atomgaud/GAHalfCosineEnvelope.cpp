// GAHalfCosineEnvelope Implementation
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
#include "GAHalfCosineEnvelope.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAHalfCosineEnvelope() { return( new CGAHalfCosineEnvelope); }
const CClass CGAHalfCosineEnvelope::classCGAHalfCosineEnvelope(CID_GAHalfCosineEnvelope, CID_GAEnvelope, "CGAHalfCosineEnvelope", CreateCGAHalfCosineEnvelope);
const IClass*	CGAHalfCosineEnvelope::GetClass() { return((const IClass*)&classCGAHalfCosineEnvelope); }


// CTOR
CGAHalfCosineEnvelope::CGAHalfCosineEnvelope()
{
	SetName("HalfCosineEnvelope");
	m_nMode=0;

}

// DTOR
CGAHalfCosineEnvelope::~CGAHalfCosineEnvelope()
{

}

// explicit terminate baseclass
void CGAHalfCosineEnvelope::Terminate()
{

	CGAEnvelope::Terminate();
}


// 0= descending 1= ascending
void CGAHalfCosineEnvelope::SetMode(UINT32 nMode)
{
	m_nMode = nMode;
}

BOOL CGAHalfCosineEnvelope::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAEnvelope::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Mode") == 0 && pszValue != NULL) {
		UINT32 nValue = atoi(pszValue);
		SetMode(nValue);
		b = TRUE;
	}

	return(b);
}


UINT32 CGAHalfCosineEnvelope::Process(IObject* pob)
{
	if(m_fDuration <= 0.0f)
		return(0);

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfTrigger = GetSignalData(TRIG);
	FLOAT32* pfDM = GetSignalData(Duration);

	FLOAT32 fPhaseIncr = 1.0f / (fSR * m_fDuration);
	FLOAT32 fDuration = m_fDuration;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		if(pfTrigger != NULL) {

			FLOAT32 fTrigger = pfTrigger[i] - m_fLastTrigger;
			if(fTrigger  > 0.5f ) {
				Trigger();
			}
			m_fLastTrigger = pfTrigger[i];
		}

		// bipolar duration modulation
		if(pfDM != NULL) {

			FLOAT32 fMod = pfDM[i];
			fDuration = m_fDuration + (m_fDuration * fMod * 0.9999f); // must not be 0
		}

		fPhaseIncr = 1.0f / (fSR * fDuration);

		m_fSample = (1.0f + cosf(m_fPhase*ATOM_PI)) *0.5f;
		pfOut[i] = m_fSample;
		
		m_fPhase +=fPhaseIncr ;
		if( m_fPhase >= 1.0f ) // stop phase and wait for reset
			m_fPhase = 1.0f;
	}


	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
