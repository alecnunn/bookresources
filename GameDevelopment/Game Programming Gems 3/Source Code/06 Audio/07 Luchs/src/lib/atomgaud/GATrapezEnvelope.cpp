// GATrapezEnvelope Implementation
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
#include "GATrapezEnvelope.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGATrapezEnvelope() { return( new CGATrapezEnvelope); }
const CClass CGATrapezEnvelope::classCGATrapezEnvelope(CID_GATrapezEnvelope, CID_GAEnvelope, "CGATrapezEnvelope", CreateCGATrapezEnvelope);
const IClass*	CGATrapezEnvelope::GetClass() { return((const IClass*)&classCGATrapezEnvelope); }


// CTOR
CGATrapezEnvelope::CGATrapezEnvelope()
{
	SetName("TrapezEnvelope");
	m_fSlope = 0.33f;
	AppendPin(Slope);

}

// DTOR
CGATrapezEnvelope::~CGATrapezEnvelope()
{
	Terminate();
}

// explicit terminate baseclass
void CGATrapezEnvelope::Terminate()
{

	CGAEnvelope::Terminate();
}


// you can change the envelopes shape by setting the slope:
// 0=rectangle
// 0.33= trapez
// 1=triangle
void CGATrapezEnvelope::SetSlope(FLOAT32 fSlope)
{
	m_fSlope = fSlope;
}




// m_fSlope is in range 0 to 1 and translates to 0 to half duration
// slope 0 forms a rectangle, slope 1 forms a triangle,
// values in between form a trapez
UINT32 CGATrapezEnvelope::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSample = 0.0f;

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfTrigger = GetSignalData(TRIG);
	FLOAT32* pfSlope = GetSignalData(Slope);

	FLOAT32	fSlope = m_fSlope;

	UINT32 nDuration =	m_fDuration * m_fScale * fSR;			// calc duration in samples
	UINT32 nSamplesInSlope = (nDuration / 2.0f) *  m_fSlope; 
	UINT32 np1 = nSamplesInSlope; // plateau begin
	UINT32 np2 = nDuration - nSamplesInSlope;		// plateau end
	FLOAT32 fSlopeIncr = nSamplesInSlope != 0 ? 1.0f / nSamplesInSlope : 0.0f;

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

			// recalc 
			nSamplesInSlope = (nDuration / 2.0f) *  fSlope; 
			np1 = nSamplesInSlope; // plateau begin
			np2 = nDuration - nSamplesInSlope;		// plateau end
			fSlopeIncr = nSamplesInSlope != 0 ? 1.0f / nSamplesInSlope : 0.0f;

		}

		pfOut[i] = m_fSample;

		if(m_nSamplePos < np1)
			m_fSample += fSlopeIncr;
		else if(m_nSamplePos < np2)
			m_fSample = 1.0f;
		else if(m_nSamplePos < (nDuration - 1))
			m_fSample -= fSlopeIncr;
		else
			m_fSample = 0.0f;
		
		if(m_nSamplePos < nDuration)
			m_nSamplePos++;
	}


	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------



