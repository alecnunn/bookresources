// GAClippedSineOscillator Implementation
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
#include "GAClippedSineOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAClippedSineOscillator() { return( new CGAClippedSineOscillator); }
const CClass CGAClippedSineOscillator::classCGAClippedSineOscillator(CID_GAClippedSineOscillator, CID_GAOscillator, "CGAClippedSineOscillator", CreateCGAClippedSineOscillator);
const IClass*	CGAClippedSineOscillator::GetClass() { return((const IClass*)&classCGAClippedSineOscillator); }


// CTOR
CGAClippedSineOscillator::CGAClippedSineOscillator()
{
	SetName("Clipped Sine Oscillator");
	AppendPin(Clipping);
	m_fClip = 1.0f;
	m_fDamping = 0.0f;
}

// DTOR
CGAClippedSineOscillator::~CGAClippedSineOscillator()
{
	Terminate();
}

// explicit terminate baseclass
void CGAClippedSineOscillator::Terminate()
{

	CGAOscillator::Terminate();
}



// return 0 on success
// if an AM is connected, the internal amplitude is not used
UINT32 CGAClippedSineOscillator::Process(IObject* pob)
{

	//	CHAR8 szBuffer[256];
	//	sprintf(szBuffer, "Patch - connect processors %d %d with pins %d %d", nFrom, nTo, nPinFrom, nPinTo); 

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfAmplitude = GetSignalData(Amplitude);

	FLOAT32* pfPhase = GetSignalData(Phase);
	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32	fAmount = 1.0f;

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

		m_fSample = (FLOAT32)sin((double)m_fPhase * ATOM_2PI) * fAmplitude;	

		if(m_fSample > m_fClip)
			m_fSample = m_fClip ;
		else if (m_fSample < -m_fClip)
			m_fSample = -m_fClip ;

		m_fClip *= m_fDamping;

		if(m_fClip < 0.0f)
			m_fClip = 0.0f;

		*pfOut++ = m_fSample;


		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f ) {
			m_fPhase -= 1.0f;
			m_fClip = 0.33f + 0.66f * ((FLOAT32)rand() / (FLOAT32)RAND_MAX);
			m_fDamping = 0.957f + fPhaseIncr; // + ((FLOAT32)rand() / ((FLOAT32)RAND_MAX/10));

			if(m_fDamping > 1.0f)
				m_fDamping = 1.0f;
		}
		
	}


	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
