// GAWavetableOscillator Implementation
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
#include "GAWavetableOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAWavetableOscillator() { return( new CGAWavetableOscillator); }
const CClass CGAWavetableOscillator::classCGAWavetableOscillator(CID_GAWavetableOscillator, CID_GAOscillator, "CGAWavetableOscillator", CreateCGAWavetableOscillator);
const IClass*	CGAWavetableOscillator::GetClass() { return((const IClass*)&classCGAWavetableOscillator); }


// CTOR
CGAWavetableOscillator::CGAWavetableOscillator()
{
	SetName("Wavetable Oscillator");
	m_pfWavetable = NULL;
	m_nSampleCount = 0;
	m_nSamplePos=0;
	m_fBaseFreq = 1.0f; // base freq is 1.0, not in Hz units !!
	m_nCycles = UINT_MAX; // (UINT32)-1; // -1 loops forever, use 1 for one-shot playback
	m_nCurrCycle = 0;
	m_fLastImpulse = 0.0f; // trigger

	AppendPin(TRIG);
	AppendPin(StartTime); // startsample can be modulated

}

// DTOR
CGAWavetableOscillator::~CGAWavetableOscillator()
{
	Terminate();
}

void CGAWavetableOscillator::Terminate()
{
	CGAOscillator::Terminate();
}


void CGAWavetableOscillator::Reset()
{
	CGAOscillator::Reset();
	m_nCurrCycle = 0;
	m_nSamplePos=0;
//	m_fPhase = 0.0f;
}


BOOL CGAWavetableOscillator::SetWavetable( FLOAT32* pSamples, UINT32 nFrames)
{
	BOOL b = FALSE;
	if(pSamples != NULL) {
		m_pfWavetable = pSamples;
		m_nSampleCount = nFrames;
		b = TRUE;
	}
	return(b);
}

BOOL CGAWavetableOscillator::SetWavetable( IGASignal* pSignal)
{
	BOOL b = FALSE;
	if(pSignal != NULL) {
		m_pfWavetable = pSignal->GetData();
		m_nSampleCount = pSignal->GetCount();
		b = TRUE;
	}
	return(b);
}

// set the repeat count: 1 for one shot,n cycles , or -1 for loop mode
void CGAWavetableOscillator::SetRepeat( UINT32 nCycles)
{
	m_nCycles = nCycles;
}


void CGAWavetableOscillator::SetStartTime( UINT32 nSamples)
{
	m_nStartTime = nSamples;
}

BOOL CGAWavetableOscillator::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAOscillator::SetParameter(pszName, pszValue);

	if(pszValue != NULL) {
		if(strcmp(pszName, "WaveTable")==0) {
			// find the signal by name
			IGASignals* pSignals = g_GASystem.GetSignals();
			IGASignal* pSignal = pSignals->Find(pszValue);
			SetWavetable( pSignal) ;
			b = pSignal != NULL ? TRUE : FALSE;
		}
		else if(strcmp(pszName, "Repeat")==0) {
			UINT32 nValue = atoi(pszValue);
			SetRepeat(nValue);
			b = TRUE;
		}
	}
	return(b);
}

// Linear interpolation between A and B:
#define LIP( T, A, B )   ( (A) + (T) * ( (B) - (A) ) )


// return 0 on success
UINT32 CGAWavetableOscillator::Process(IObject* pob)
{
	if(m_nSampleCount == 0 || m_pfWavetable == NULL)
		return(0);

  	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = m_pfWavetable;
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);			// bipolar frequency modulation
	FLOAT32* pfUniFM = GetSignalData(Frequency);  // unipolar frequency modulation
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfUAM = GetSignalData(Amplitude);
	FLOAT32* pfTrigger = GetSignalData(TRIG);
	FLOAT32* pfPhase = GetSignalData(Phase);
	FLOAT32* pfStartTime = GetSignalData(StartTime);

	FLOAT32 fPhaseIncr = m_fBaseFreq / m_nSampleCount; // base freq is 1.0 !!
	FLOAT32 fFract = 0.0f;
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32	fAmount = 1.0f;

	if(pfIn != NULL) {

		for( UINT32 i=0; i< nFrames; i++ )
		{

			if(pfTrigger != NULL) {

				FLOAT32 fTrigger = pfTrigger[i] - m_fLastImpulse;
				if(fTrigger  > 0.5f ) 
					Reset();

				m_fLastImpulse = pfTrigger[i];

			}
			// write silence, if cyclecount is reached
			// after done that, inorder to start playing again you have to reset the osc
			if(m_nCurrCycle >= m_nCycles) {

				pfOut[i] = 0.0f; // silence

				
			}
			else {


				// unipolar amplitude modulation
				if(pfUAM != NULL) {
					
					fAmplitude = pfUAM[i];
				}
				// bipolar amplitude modulation
				if(pfAM != NULL) { 
					
					// mod range ( -1 to +1) -> 0 to 1
					FLOAT32 fMod = pfAM[i];
					fMod += 1.0f;
					fMod *= 0.5f;

					fAmplitude = fMod;
				}

				/*
				// unipolar frequency modulation
				// 0.1 == 1 octave
				if(pfUniFM  != NULL) {
					
					m_fBaseFreq = pfUniFM[i];

				}
				*/

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
						fPhaseIncr = (fFreq / m_nSampleCount); 

				}
				
				if(pfPhase != NULL) {
					
					m_fPhaseOffset = pfPhase[i] * m_fPMAttenuation; 
					
				}


				// truncating
				FLOAT32 fPos = (m_fPhase * (FLOAT32)m_nSampleCount);
				m_nSamplePos = (UINT32)fPos;
				fFract = fPos - m_nSamplePos;

				// interpolation
				UINT32 nNextPos = m_nSamplePos+1;
				if(nNextPos >= m_nSampleCount)
					nNextPos = 0; // wrap

				FLOAT32 fa = pfIn[m_nSamplePos];
				fa *= (1.0f - fFract); 
				FLOAT32 fb = pfIn[nNextPos];
				fb *= fFract;

				
				pfOut[i] = (fa + fb)* fAmplitude;
			}

			m_fPhase +=fPhaseIncr;
			if(m_fPhase >= 1.0f) {
				m_nCurrCycle++;

			}

			while(m_fPhase >= 1.0f) {
				m_fPhase -= 1.0f ;
				m_fPhase += m_fPhaseOffset; // start point
			}

		}
	} // if wavetable in

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------


/*
// return 0 on success
UINT32 CGAWavetableOscillator::Process(IObject* pob)
{

  	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = m_pfWavetable;
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);

	FLOAT fPhaseIncr = m_fBaseFreq / m_nSampleCount; // base freq is 1.0 !!

	if(pfIn != NULL) {

		for( UINT32 i=0; i< nFrames; i++ )
		{

			
			if(pfFM != NULL) {
				
				// convert mod range ( -1 +1)---> amount ( 0.25 --> 4.0 )

				FLOAT fMod = pfFM[i]; // assume same frame count!
				fMod *= (m_fFMGain / 2);
				fMod += (m_fFMGain / 2);
				fMod = pow(2,fMod);
				fMod /= m_fFMGain;
			
				fPhaseIncr = m_fBaseFreq / m_nSampleCount * fMod;

			}


			pfOut[i] = pfIn[m_nSamplePos] * m_fAmplitude;
			m_fPhase +=fPhaseIncr ;
			m_nSamplePos = (m_fPhase * m_nSampleCount);

			if(  m_nSamplePos >= m_nSampleCount ) {

				m_fPhase -= 1.0f; // (FLOAT32)m_nSamplePos;
				m_nSamplePos = 0; //(UINT32) m_fPhase;

			}
		
	
		}
	} // if wavetable in

	return(0);

}*/