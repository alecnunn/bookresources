// GAPulseOscillator Implementation
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
#include "GAPulseOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAPulseOscillator() { return( new CGAPulseOscillator); }
const CClass CGAPulseOscillator::classCGAPulseOscillator(CID_GAPulseOscillator, CID_GAOscillator, "CGAPulseOscillator", CreateCGAPulseOscillator);
const IClass*	CGAPulseOscillator::GetClass() { return((const IClass*)&classCGAPulseOscillator); }


// CTOR
CGAPulseOscillator::CGAPulseOscillator()
{
	SetName("PulseOscillator");
//	m_fAccu = 0.0f;
	m_fPulseWidth = 0.5f;
	m_fAmplitude = 0.99f;

	AppendPin(PulseWidth);

}

// DTOR
CGAPulseOscillator::~CGAPulseOscillator()
{
	Terminate();
}

// explicit terminate baseclass
void CGAPulseOscillator::Terminate()
{

	CGAOscillator::Terminate();
}

// return 0 on success
// if an AM is connected, the internal amplitude is not used
UINT32 CGAPulseOscillator::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfPhase = GetSignalData(Phase);
	FLOAT32* pfWidth = GetSignalData(PulseWidth);

	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fAmplitude = m_fAmplitude;

	FLOAT fWidth = m_fPulseWidth;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		if(pfAM != NULL) { // amplitude modulation
			
			// mod range ( -1 to +1) -> 0 to 1
			FLOAT32 fMod = pfAM[i];
			fMod += 1.0f;
			fMod *= 0.5f;

			fAmplitude = fMod;
		}


		if(pfPhase != NULL) {
			
			// mod range ( -1 +1)

			FLOAT32 fMod = pfPhase[i]; 
			m_fPhase +=fMod ;

			if( m_fPhase >= 1.0f )
				m_fPhase -= 1.0f;
			else if( m_fPhase <= 0.0f )
				m_fPhase += 1.0f;

		}

		if(pfFM  != NULL) {
			
			FLOAT32 fFMAtt = m_fFMAttenuation * ATT_MAX;
			FLOAT32 fMod = pfFM[i]; 
			fMod *= (fFMAtt / 2);
			fMod += (fFMAtt / 2);
			fMod = pow(2,fMod);
			fMod /= fFMAtt;


			FLOAT32 fFreq = m_fBaseFreq * fMod; 
			fPhaseIncr = fFreq / fSR;
		}

		// pulse width modulation
		if(pfWidth  != NULL) {
			
			// convert mod range 
			FLOAT32 fGain = 0.97f; // max 1
			FLOAT32 fMod = pfWidth[i]; 
			fMod *= 0.5f; // -0.5 to +0.5

			fWidth = m_fPulseWidth + (fMod * fGain);
		//	if(i%100 == 0)
		//		GADUMP(fWidth);
		}

		if(m_fPhase < fWidth)
			m_fSample = 0.99f;
		else
			m_fSample = -0.99f;

		pfOut[i] = m_fSample * m_fAmplitude;	

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

	
	}


	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
