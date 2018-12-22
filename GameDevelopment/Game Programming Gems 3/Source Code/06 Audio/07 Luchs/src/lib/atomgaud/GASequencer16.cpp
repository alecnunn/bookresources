// GASequencer16 Implementation
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
#include "GASequencer16.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASequencer16() { return( new CGASequencer16); }
const CClass CGASequencer16::classCGASequencer16(CID_GASequencer16, CID_GAOscillator, "CGASequencer16", CreateCGASequencer16);
const IClass*	CGASequencer16::GetClass() { return((const IClass*)&classCGASequencer16); }


// CTOR
CGASequencer16::CGASequencer16()
{
	SetName("Sequencer16");
	m_nSampleCounter = 0; 
	m_fBaseFreq = 1.0f;
	m_nSteps = 0;
	m_nCurrStep = 0;
	m_nCyclePos=0;
}

// DTOR
CGASequencer16::~CGASequencer16()
{
	Terminate();
}

// explicit terminate baseclass
void CGASequencer16::Terminate()
{

	CGAOscillator::Terminate();
}

// set the step mask, trigger on each bit set
void CGASequencer16::SetSteps(UINT32 nSteps)
{
	m_nSteps = nSteps;
}

// set the step mask from a text pattern xoooxoooxoooxooo
void CGASequencer16::SetSteps(CHAR8* pszValue)
{
	m_nSteps = 0;
	for(int i=0;i <16;i++)
		if(pszValue[i] == 'x' || pszValue[i] == 'X')
			m_nSteps += ATOM_POW2[i];
}


BOOL CGASequencer16::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAOscillator::SetParameter(pszName, pszValue);

	if(pszValue != NULL) {
		if(strcmp(pszName, "Steps") == 0 ) {

			if(strlen(pszValue) == 16){
				SetSteps(pszValue); // set pattern
				b = TRUE;
			}
			else {

				UINT32 nValue = atoi(pszValue);
				
				SetSteps(nValue);
				b = TRUE;
			}
		}
	}
	return(b);
}

// generate a single impulse if step was set
UINT32 CGASequencer16::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);

	FLOAT32 fPhaseIncr = m_fBaseFreq / fSR;
	UINT32 nSamplesPerCycle =  fSR / 16.0f;

	UINT32 nBit=0;
	static UINT32 nOldPhase= 15;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32 fAmount = 1.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{

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
/*
		// bipolar frequency modulation
		// 0.1 == 1 octave
		if(pfFM  != NULL) {
			
			FLOAT fMod = pfFM[i];
			FLOAT32 fFreqRatio = m_fBaseFreq;
			fMod *= (m_fFMAttenuation * 10.0f);

			if(fMod > 0.0f)
				fFreqRatio = (m_fBaseFreq * (1.0f + fMod));
			else if(fMod < 0.0f)
				fFreqRatio = (m_fBaseFreq / (1.0f -fMod));
			else
				fFreqRatio = 1.0f;

			fPhaseIncr = m_fBaseFreq / fSR * fFreqRatio; // fFreqRatio / fSR;
		}
*/
		pfOut[i] = 0.0f;	

		UINT32 nPhase = m_fPhase * 16.0f;

		if(nPhase != nOldPhase) {

			UINT32 n = m_nSteps >> nPhase;
			BOOL bTrigger =  n & 1;
			if(bTrigger == TRUE )  
				pfOut[i] = 0.99f;

			nOldPhase = nPhase;
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

  // generate a single impulse if step was set
UINT32 CGASequencer16::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);

	FLOAT32 fPhaseIncr = m_fBaseFreq / fSR;
	UINT32 nSamplesPerCycle =  fSR / 16.0f;

	UINT32 nBit=0;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		// bipolar frequency modulation
		// 0.1 == 1 octave
		if(pfFM  != NULL) {
			
			FLOAT fMod = pfFM[i];
			FLOAT32 fFreqRatio = m_fBaseFreq;
			fMod *= (m_fFMAttenuation * 10.0f);

			if(fMod > 0.0f)
				fFreqRatio = (m_fBaseFreq * (1.0f + fMod));
			else if(fMod < 0.0f)
				fFreqRatio = (m_fBaseFreq / (1.0f -fMod));
			else
				fFreqRatio = 1.0f;

			fPhaseIncr = m_fBaseFreq / fFreqRatio;
			nSamplesPerCycle =  fSR / 16.0f * fFreqRatio;
//			if(nSamplesPerCycle < 1)
//				nSamplesPerCycle = 1;
		}

		pfOut[i] = 0.0f;	

		m_nCyclePos++;

		if(m_nCyclePos == nSamplesPerCycle) {

			UINT32 n = m_nSteps >> m_nCurrStep;
			BOOL bTrigger = n & 1;
			if(bTrigger == TRUE )  
				pfOut[i] = 0.99f;

			m_nCurrStep++;
			if(m_nCurrStep > 16)
				m_nCurrStep = 0;
			m_nCyclePos=0;

		}



	}


	return(0);

}
*/