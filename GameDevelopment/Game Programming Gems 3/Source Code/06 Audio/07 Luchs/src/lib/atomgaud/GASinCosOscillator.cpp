// GASinCosOscillator Implementation
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
#include "GASinCosOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASinCosOscillator() { return( new CGASinCosOscillator); }
const CClass CGASinCosOscillator::classCGASinCosOscillator(CID_GASinCosOscillator, CID_GAOscillator, "CGASinCosOscillator", CreateCGASinCosOscillator);
const IClass*	CGASinCosOscillator::GetClass() { return((const IClass*)&classCGASinCosOscillator); }


// CTOR
CGASinCosOscillator::CGASinCosOscillator()
{
	SetName("SinCos Oscillator");

}

// DTOR
CGASinCosOscillator::~CGASinCosOscillator()
{
	Terminate();
}

// explicit terminate baseclass
void CGASinCosOscillator::Terminate()
{

	CGAOscillator::Terminate();
}

UINT32 CGASinCosOscillator::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fNyquist = fSR * 0.5;

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfFMU = GetSignalData(Frequency);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfAmplitude = GetSignalData(Amplitude);

	FLOAT32* pfPhase = GetSignalData(Phase);
	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32 fAmount = 1.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{
		// unipolar amplitude modulation
		if(pfAmplitude != NULL) {
			
			fAmplitude = pfAmplitude[i];

		}

		// bipolar amplitude modulation
		if(pfAM != NULL) {
			
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

	// unipolar  frequency modulation
		if(pfFMU != NULL) {
			
			FLOAT fMod = pfFMU[i]; 
			if(fMod > 0.0f) {
				fFreq = fMod * m_fBaseFreq * 10;
				fPhaseIncr = fFreq / fSR;
			}
		}


		// bipolar frequency modulation
		// 0.1 == 1 octave
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
				fPhaseIncr = fFreq / fSR;
			}

		if(m_fPhase >= 0.0f && m_fPhase < 0.25f)
			m_fSample = sinf(m_fPhase * ATOM_2PI);
		else if(m_fPhase >= 0.25f && m_fPhase < 0.5f)
			m_fSample = cosf(m_fPhase * ATOM_2PI) + 1.0f;
		else if(m_fPhase >= 0.5f && m_fPhase < 0.75f)
			m_fSample = cosf((m_fPhase-0.5f) * ATOM_2PI) - 1.0f;
		else if(m_fPhase >= 0.75f && m_fPhase < 1.0f)
			m_fSample = sinf(m_fPhase * ATOM_2PI);


		m_fSample *= fAmplitude;
		pfOut[i] = m_fSample;	


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
