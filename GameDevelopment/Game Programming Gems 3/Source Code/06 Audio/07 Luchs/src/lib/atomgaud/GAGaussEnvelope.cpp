// GAGaussEnvelope Implementation
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
#include "GAGaussEnvelope.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAGaussEnvelope() { return( new CGAGaussEnvelope); }
const CClass CGAGaussEnvelope::classCGAGaussEnvelope(CID_GAGaussEnvelope, CID_GAEnvelope, "CGAGaussEnvelope", CreateCGAGaussEnvelope);
const IClass*	CGAGaussEnvelope::GetClass() { return((const IClass*)&classCGAGaussEnvelope); }


// CTOR
CGAGaussEnvelope::CGAGaussEnvelope()
{
	SetName("GaussEnvelope");
	m_fSlope = 1.0f / 6.0f;
}

// DTOR
CGAGaussEnvelope::~CGAGaussEnvelope()
{
	Terminate();
}

// explicit terminate baseclass
void CGAGaussEnvelope::Terminate()
{

	CGAEnvelope::Terminate();
}



UINT32 CGAGaussEnvelope::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSample = 0.0f;

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfTrigger = GetSignalData(TRIG);
	FLOAT32* pfSlope = NULL; // GetSignalData(Slope);

	FLOAT32	fSlope = m_fSlope;


	UINT32 nDuration =	m_fDuration * m_fScale * fSR;			// calc duration in samples
	UINT32 nSlope = nDuration * fSlope;
	UINT32 nMean = nDuration * 0.5f; // plateau begin
	UINT32 nSamplesSlope = nDuration * fSlope;

	for( UINT32 i=0; i< nFrames; i++ )
	{
		if(pfTrigger != NULL) {

			FLOAT32 fTrigger = pfTrigger[i] - m_fLastTrigger;
			if(fTrigger  > 0.5f ) {

				Trigger();

			}

			m_fLastTrigger = pfTrigger[i];

		}

		// slope modulation
		// if modulation is connected, it replaces the internal slope 
		if(pfSlope != NULL) { 
			
			FLOAT32 fMod = pfSlope[i];	// -1 to +1
			fMod += 1.0f;				// 0 to 2
			fMod *= 0.5f;				// 0 to 1

			fSlope = fMod;

		}

		pfOut[i] = m_fSample;

		FLOAT32 fPhase =  (FLOAT32)m_nSamplePos / nDuration;
		FLOAT32 fMean = fabs(fPhase - 0.5f);
		 
		m_fSample = pow(ATOM_E, -(pow(fMean,2))/(2*pow(fSlope, 2)));

		if(m_nSamplePos < nDuration)
			m_nSamplePos++;
	}


	return(0);
}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
 