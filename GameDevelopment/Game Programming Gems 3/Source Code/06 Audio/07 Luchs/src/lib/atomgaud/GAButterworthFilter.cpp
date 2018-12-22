// GAButterworthFilter Implementation
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
#include "GAButterworthFilter.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAButterworthFilter() { return( new CGAButterworthFilter); }
const CClass CGAButterworthFilter::classCGAButterworthFilter(CID_GAButterworthFilter, CID_GAProcessor, "CGAButterworthFilter", CreateCGAButterworthFilter);
const IClass*	CGAButterworthFilter::GetClass() { return((const IClass*)&classCGAButterworthFilter); }


// CTOR
CGAButterworthFilter::CGAButterworthFilter()
{

	SetName("ButterworthFilter");

   m_fBaseFreq = 1000.0f; // freq in hz
   m_fCurrFreq = m_fBaseFreq;
   m_bw = 50.0f; // bw;
   m_buf_x0 = 0.0f;
   m_buf_x1 = 0.0f;
   m_buf_y0 = 0.0f;
   m_buf_y1 = 0.0f;

   m_fFMAttenuation =  1.0f;

   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn);
	AppendPin(FM);
	AppendPin(AM);

	 Update();

}

// DTOR
CGAButterworthFilter::~CGAButterworthFilter()
{

}

void CGAButterworthFilter::Terminate()
{
	CGAProcessor::Terminate();
}

// a cutoff value of 1.0 is half nyquist frequency
void CGAButterworthFilter::SetCutoff(FLOAT32 fCut)
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fHalfNyquist = fSR * 0.25f;
	m_fCurrFreq = fHalfNyquist * fCut;
	Update();
}

// set frequency in Hz, limited to half nyquist frequency
void CGAButterworthFilter::SetFrequency(FLOAT32 fFreqHz)
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fHalfNyquist = fSR * 0.25f;
	if(fFreqHz > fHalfNyquist)
		fFreqHz = fHalfNyquist;

	m_fBaseFreq = fFreqHz;
	Update();
}

// input 0 to 1
void CGAButterworthFilter::SetFMAttenuation(FLOAT32 fAttenuation)
{
	m_fFMAttenuation = fAttenuation; 
	Update();
}

// call this when a parameter has changed
void CGAButterworthFilter::Update()
{
	// to implement in derived class

}


BOOL CGAButterworthFilter::SetParameter(CHAR8* pszName, CHAR8* pszValue)
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
UINT32 CGAButterworthFilter::Process(IObject* pob)
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fHalfNyquist = fSR * 0.25f;
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32 fFMB = 1.0f;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32 fAmount = 1.0f;

	if(pfIn != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
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

			FLOAT32 fIn = pfIn[i];

			m_fSample = m_a[0] * fIn + m_a[1] * m_buf_x0 + m_a[2] * m_buf_x1
					   - m_b[0] * m_buf_y0 - m_b[1] * m_buf_y1;
			 
			 if(m_fSample > 1.0f)	// limit
				 m_fSample = 1.0f;
			 else if(m_fSample< -1.0f)
				 m_fSample= -1.0f;


			 m_buf_x1 = m_buf_x0;
			 m_buf_x0 = fIn;
			 m_buf_y1 = m_buf_y0;
			 m_buf_y0 = m_fSample;		


			 pfOut[i] = m_fSample * 0.9f;// reduce

		}

		
	}

	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------


/*

			if(fMod > 0.0f)
					m_fCurrFreq = m_fBaseFreq * (1.0f + fMod);
				else if(fMod < 0.0f)
					m_fCurrFreq = m_fBaseFreq / (1.0f - fMod);
				else
					m_fCurrFreq = m_fBaseFreq;




  			if(fMod > 0.0f)
					fAmount = 1.0f * (1.0f + (fMod * 2.0f));
				else if(fMod < 0.0f)
					fAmount = 1.0f  / (1.0f - (fMod * 2.0f));
				else
					fAmount = 1.0f;



			*/