// GASphereEnvelope Implementation
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
#include "GASphereEnvelope.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASphereEnvelope() { return( new CGASphereEnvelope); }
const CClass CGASphereEnvelope::classCGASphereEnvelope(CID_GASphereEnvelope, CID_GAEnvelope, "CGASphereEnvelope", CreateCGASphereEnvelope);
const IClass*	CGASphereEnvelope::GetClass() { return((const IClass*)&classCGASphereEnvelope); }


// CTOR
CGASphereEnvelope::CGASphereEnvelope()
{
	SetName("SphereEnvelope");

}

// DTOR
CGASphereEnvelope::~CGASphereEnvelope()
{
	Terminate();
}

// explicit terminate baseclass
void CGASphereEnvelope::Terminate()
{

	CGAEnvelope::Terminate();
}



void CGASphereEnvelope::Trigger()
{
	m_nSamplePos = 0;	// reset to restart line 
	m_fSample = 0.0f;
	Update();
}



UINT32 CGASphereEnvelope::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();

//	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfTrigger = GetSignalData(TRIG);

	UINT32 nDuration = m_fDuration * fSR; // duration in samples
	FLOAT32 fIncr = 1/nDuration;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		if(pfTrigger != NULL) {

			FLOAT32 fTrigger = pfTrigger[i] - m_fSample;
			if(fTrigger  > 0.5f ) {

				Trigger();

			}

		//	m_fSample = pfTrigger[i];

		}



		if(m_nSamplePos < nDuration) {

			FLOAT32 fUnip = m_nSamplePos / nDuration;
			FLOAT32 fBip = fUnip * 2 - 1.0f;

			m_fSample = sqrt(1.0f - (fBip*fBip));

		}

		pfOut[i] = m_fSample;
		m_nSamplePos++;		// reset by trigger
	}


	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
