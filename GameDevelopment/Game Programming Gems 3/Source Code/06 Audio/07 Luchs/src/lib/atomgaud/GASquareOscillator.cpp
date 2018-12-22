// GASquareOscillator Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GASquareOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASquareOscillator() { return( new CGASquareOscillator); }
const CClass CGASquareOscillator::classCGASquareOscillator(CID_GASquareOscillator, CID_GAGenerator, "CGASquareOscillator", CreateCGASquareOscillator);
const IClass*	CGASquareOscillator::GetClass() { return((const IClass*)&classCGASquareOscillator); }


// CTOR
CGASquareOscillator::CGASquareOscillator()
{
	SetName("Square Oscillator");

}

// DTOR
CGASquareOscillator::~CGASquareOscillator()
{
	Terminate();
}

void CGASquareOscillator::Terminate()
{
	CGAOscillator::Terminate();
}

// if an AM is connected, the internal amplitude is not used
UINT32 CGASquareOscillator::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfPhase = GetSignalData(Phase);
//	FLOAT32* pfCenter = GetSignalData(Center);
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32	fPhaseIncr = fFreq / fSR;
	FLOAT32 fAmount = 1.0f;	
	
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

		// bipolar frequency modulation
		// 0.1 == 1 octave
			if(pfFM  != NULL) {
				
				FLOAT fMod = pfFM[i]; 

				if(fMod > 0.0f)
					fAmount = 1.0f * (1.0f + (fMod *  (m_fFMAttenuation * 10.0f)));
				else if(fMod < 0.0f)
					fAmount = 1.0f  / (1.0f - (fMod * (m_fFMAttenuation * 10.0f)));
				else
					fAmount = 1.0f;

				fFreq = m_fBaseFreq * fAmount;

			}

		
		fPhaseIncr = fFreq / fSR;
		pfOut[i] = m_fSample;	

		
		if(m_fPhase >= 0.0f && m_fPhase < 0.5f) {
			m_fSample = fAmplitude;
	
		}
		else {
			m_fSample = -fAmplitude;
		}
		
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


/*

  		
		fPhaseIncr = fFreq / fSR;
		UINT32 nIncr = 4294967296.0 * fPhaseIncr;

		pfOut[i] = m_fSample;	

		
		if(m_fPhase >= 0.0f && m_fPhase < 0.5f) {
			m_fSample = fAmplitude;
	
		}
		else {
			m_fSample = -fAmplitude;
		}
		

		if(m_nPhase & ATOM_POW2[31])
			m_fSample = fAmplitude;
		else
			m_fSample = -fAmplitude;


		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		m_nPhase += nIncr;

  */