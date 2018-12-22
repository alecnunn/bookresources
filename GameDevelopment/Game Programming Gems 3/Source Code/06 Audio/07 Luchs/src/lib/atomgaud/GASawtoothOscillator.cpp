// GASawtoothOscillator Implementation
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
#include "GASawtoothOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASawtoothOscillator() { return( new CGASawtoothOscillator); }
const CClass CGASawtoothOscillator::classCGASawtoothOscillator(CID_GASawtoothOscillator, CID_GAGenerator, "CGASawtoothOscillator", CreateCGASawtoothOscillator);
const IClass*	CGASawtoothOscillator::GetClass() { return((const IClass*)&classCGASawtoothOscillator); }


// CTOR
CGASawtoothOscillator::CGASawtoothOscillator()
{
	SetName("Sawtooth Oscillator");
	Update();
}

// DTOR
CGASawtoothOscillator::~CGASawtoothOscillator()
{

}

void CGASawtoothOscillator::Terminate()
{
	CGAGenerator::Terminate();
}


// recalc, when a parameter has changed
void CGASawtoothOscillator::Update()
{
//	FLOAT32 fSR = g_GASystem.GetSampleRate();
//	m_fPhaseIncr = m_fBaseFreq / fSR;
}


void CGASawtoothOscillator::Trigger()
{
	Update();
}


UINT32 CGASawtoothOscillator::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fNyquist = fSR * 0.5f;
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFrequency = GetSignalData(Frequency);

	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfAmp = GetSignalData(Amplitude);

	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT fFreqUni = 0.0f;
	FLOAT fFreqBip = 0.0f;
//	FLOAT fFrequency = m_fBaseFreq;
//	GADUMP(m_fBaseFreq);
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32 fAmount = 1.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{
		// unipolar amplitude modulation
		if(pfAmp != NULL) {
			
			FLOAT32 fMod = pfAmp[i];
			fAmplitude = fMod;
		}

		// bipolar amplitude modulation
		if(pfAM != NULL) {
			
			FLOAT32 fMod = pfAM[i];
			fMod += 1.0f;
			fMod *= 0.5f;

			fAmplitude = fMod;
		}

		// unipolar normalized frequency modulation
		if(pfFrequency != NULL) {
			
			FLOAT fMod = pfFrequency[i]; 
			if(fMod > 0.0f) {
				fFreqUni = fMod * fNyquist;
				fFreq = fFreqUni;
			}
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
				fPhaseIncr = fFreq / fSR;

			}


	//	fPhaseIncr = fFrequency / fSR;

		FLOAT32 fSample  = m_fPhase + m_fPhase; 
		if(m_fPhase > 0.5f)
			fSample -= 2.0f;


		m_fSample = fSample * fAmplitude;
		pfOut[i] = m_fSample ; 


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
