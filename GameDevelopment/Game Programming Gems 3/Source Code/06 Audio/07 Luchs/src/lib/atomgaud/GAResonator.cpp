// GAResonator Implementation
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
#include "GAResonator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAResonator() { return( new CGAResonator); }
const CClass CGAResonator::classCGAResonator(CID_GAResonator, CID_GAProcessor, "CGAResonator", CreateCGAResonator);
const IClass*	CGAResonator::GetClass() { return((const IClass*)&classCGAResonator); }


// CTOR
CGAResonator::CGAResonator()
{
	SetName("Resonator");
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	
	m_fFM=1.0f;
	m_fBaseFreq = 1000.0f; // center frequency
	m_fCurrFreq = m_fBaseFreq;
	m_fFMAttenuation = 1.0f;
	m_fBandWidth = 50.0f;
	
	m_fGainCorrection = 500.0f / m_fBandWidth;
	
	a0 = 0.0f;
	b1 = 0.0f;
	b2 = 0.0f;
	
	y1 = 0.0f;
	y2 = 0.0f;
	
	Update();
	
	// create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size
	
	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn);
	AppendPin(FM);
	AppendPin(AM);
	
}

// DTOR
CGAResonator::~CGAResonator()
{
	Terminate();
}

void CGAResonator::Terminate()
{
	CGAProcessor::Terminate();
}

// Called after a parameter has changed
void CGAResonator::Update()
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fHalfNyquist = fSR * 0.25f;

	if(m_fCurrFreq > fHalfNyquist) // limit
		m_fCurrFreq = fHalfNyquist;

    b2 = exp(-(2 * ATOM_PI) * (m_fBandWidth / fSR));
    b1 = (-4.0 * b2) / (1.0 + b2) * cos(2 * ATOM_PI * (m_fCurrFreq / fSR));
    a0 = (1.0 - b2) * sqrt(1.0 - (b1 * b1) / (4.0 * b2));
}

// set frequency in Hz, limited to half nyquist frequency
void CGAResonator::SetFrequency(FLOAT32 fFreqHz)
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fHalfNyquist = fSR * 0.25f;
	if(fFreqHz > fHalfNyquist)
		fFreqHz = fHalfNyquist;

	m_fBaseFreq = m_fCurrFreq = fFreqHz;
	Update();
}

// input 0 to 1
void CGAResonator::SetFMAttenuation(FLOAT32 fAttenuation)
{
	m_fFMAttenuation = fAttenuation; 
	Update();
}


BOOL CGAResonator::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if( pszValue != NULL) {
		if(strcmp(pszName, "Frequency") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetFrequency(fValue);
			b = TRUE;
		}
		else if(strcmp(pszName, "FMAttenuation") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetFMAttenuation(fValue);
			b = TRUE;
		}
	}

	return(b);
}


// return 0 on success
UINT32 CGAResonator::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfAM = GetSignalData(AM);

	m_fCurrFreq = m_fBaseFreq;
	FLOAT32 fAmount = 0.0f;
	FLOAT32 fAmplitude = 1.0f;

	if(pfIn != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{

		// bipolar amplitude modulation
		if(pfAM != NULL) {
			
			FLOAT32 fMod = pfAM[i];
			fMod += 1.0f;
			fMod *= 0.5f;

			fAmplitude = fMod;
		}

		// bipolar frequency modulation
			// 0.1 == 1 octave
			// freq is limited to half nyquist
			if(pfFM  != NULL) {
				
				FLOAT fMod = pfFM[i]; 

				if(fMod > 0.0f)
					fAmount = 1.0f * (1.0f + (fMod *  (m_fFMAttenuation * 10.0f)));
				else if(fMod < 0.0f)
					fAmount = 1.0f  / (1.0f - (fMod * (m_fFMAttenuation * 10.0f)));
				else
					fAmount = 1.0f;

				m_fCurrFreq = m_fBaseFreq * fAmount;
				Update();
			}


			FLOAT fOut = (a0 * pfIn[i] - b1 * y1 - b2 * y2);
			pfOut[i] = fOut * m_fGainCorrection * fAmplitude; // bandwidth depending gain correction

			y2 = y1;
			y1 = fOut;			
		}

		
	}

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------

