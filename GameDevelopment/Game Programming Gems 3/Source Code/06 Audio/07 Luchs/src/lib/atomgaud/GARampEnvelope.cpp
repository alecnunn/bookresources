// GARampEnvelope Implementation
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
#include "GARampEnvelope.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGARampEnvelope() { return( new CGARampEnvelope); }
const CClass CGARampEnvelope::classCGARampEnvelope(CID_GARampEnvelope, CID_GAEnvelope, "CGARampEnvelope", CreateCGARampEnvelope);
const IClass*	CGARampEnvelope::GetClass() { return((const IClass*)&classCGARampEnvelope); }


// CTOR
CGARampEnvelope::CGARampEnvelope()
{
	SetName("RampEnvelope");

}

// DTOR
CGARampEnvelope::~CGARampEnvelope()
{
	Terminate();
}

// explicit terminate baseclass
void CGARampEnvelope::Terminate()
{

	CGAEnvelope::Terminate();
}


void CGARampEnvelope::Trigger()
{
	m_nSamplePos = 0;	// reset to restart line 
	m_fSample = 0.0f;
	Update();
}



UINT32 CGARampEnvelope::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfTrigger = GetSignalData(TRIG);
	FLOAT32* pfDuration = GetSignalData(Duration);
	UINT32 nDuration = m_fDuration * fSR; // duration in samples
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

		if(pfDuration != NULL) {

			// convert mod range ( -1 +1)---> amount ( 0.25 --> 4.0 )
			FLOAT32 fAmount = 4.0f;
			FLOAT fMod = pfDuration[i]; // assume same frame count!
			fMod += 1.0f;
			fMod *= 0.5f;
			fMod = pow(2,fMod * fAmount);
			fMod /= fAmount;

			fDuration = m_fDuration * fMod; 
		}


		nDuration = fDuration * fSR; // duration in samples

		if(m_nSamplePos < nDuration) {

			m_fSample = (FLOAT32) m_nSamplePos / nDuration;
			if(m_nSignalMode == ATOM_BIPOLAR)
				 m_fSample = m_fSample * 2 -1;
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
