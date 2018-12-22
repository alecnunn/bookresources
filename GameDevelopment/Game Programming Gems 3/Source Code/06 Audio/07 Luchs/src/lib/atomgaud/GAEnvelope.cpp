// GAEnvelope Implementation
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
#include "GAEnvelope.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAEnvelope() { return( new CGAEnvelope); }
const CClass CGAEnvelope::classCGAEnvelope(CID_GAEnvelope, CID_GAProcessor, "CGAEnvelope", CreateCGAEnvelope);
const IClass*	CGAEnvelope::GetClass() { return((const IClass*)&classCGAEnvelope); }


// CTOR
CGAEnvelope::CGAEnvelope()
{
	m_fDuration = 1.0f;			// base duration in sec
	m_fScale = 1.0f;
	m_fPhase = 0.0f;

	m_fDMAttenuator = 1.0f;		
	m_fAmplitude= 1.0f;			// base amplitude
	m_fAMAttenuator= 1.0f;
	m_nSignalMode = ATOM_UNIPOLAR;			// default range 0 to 1

	m_nSamplePos=0;		// current sample pos in envelope
	m_fSample = 0.0f;	// last sample value
	m_fLastTrigger = 0.0f;

	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut",  pSampleOut);
//	AppendPin(SampleIn);
	AppendPin(Duration);
//	AppendPin(AM);
	AppendPin(TRIG);


}

// DTOR
CGAEnvelope::~CGAEnvelope()
{

}

// explicit terminate baseclass
void CGAEnvelope::Terminate()
{

	CGAProcessor::Terminate();
}

void CGAEnvelope::SetSignalMode(UINT32 nMode)
{
	m_nSignalMode = nMode;
}


void CGAEnvelope::Trigger()
{
	m_fSample = 0.0f;
	m_nSamplePos = 0; // @todo, remove and use phase
	m_fPhase = 0.0f;
	
}

void CGAEnvelope::Start()
{
	Trigger();
}


// base duration in sec
void CGAEnvelope:: SetDuration(FLOAT32 fDuration)
{
	m_fDuration = fDuration;			
}


void CGAEnvelope::SetScale(FLOAT32 fScale)
{
	m_fScale = fScale;
}

BOOL CGAEnvelope::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "SignalMode") == 0 && pszValue != NULL) {
		UINT32 nValue = atoi(pszValue);
		SetSignalMode(nValue);
		b = TRUE;
	}
	else if(strcmp(pszName, "Duration") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetDuration(fValue);
		b = TRUE;
	}
	return(b);
}


UINT32 CGAEnvelope::Process(IObject* pob)
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);

	for( UINT32 i=0; i< nFrames; i++ )
	{

		pfOut[i] = pfIn[i];
		
	}


	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
