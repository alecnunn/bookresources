// GADelay Implementation
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
#include "GADelay.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGADelay() { return( new CGADelay); }
const CClass CGADelay::classCGADelay(CID_GADelay, CID_GAProcessor, "CGADelay", CreateCGADelay);
const IClass*	CGADelay::GetClass() { return((const IClass*)&classCGADelay); }


// CTOR
CGADelay::CGADelay()
{
	SetName("Delay");
	FLOAT32 fSR = g_GASystem.GetSampleRate();

	m_nWritePos=0;
	m_fDelay = 0.250f; // 250 ms
	m_fDMAttenuation = 1.0f;
	// create a buffer for SR * 2 samples, eg 88200 samples
	m_nDelayBufferSize = (UINT32)fSR * 2;
	m_pfDelayBuffer = (FLOAT32*)calloc(m_nDelayBufferSize, sizeof(FLOAT32));

	   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn);
	AppendPin(Delay);

	SetDelay((UINT32)fSR); // 1 second
}

// DTOR
CGADelay::~CGADelay()
{
	ATOM_FREE(m_pfDelayBuffer);
	Terminate();
}


// call the baseclass,  terminate explicitly 
void CGADelay::Terminate()
{
	// cleanup this
	ATOM_FREE(m_pfDelayBuffer);

	// cleanup base
	CGAProcessor::Terminate();
}

// set the delay in samples
bool CGADelay::SetDelay(UINT32 nSamples)
{
	bool bSuccess = false;

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	m_fDelay = nSamples / fSR;

//	if( nSamples  < m_nDelayBufferSize) {

//		m_nDelay = nSamples;
//	}


	return(bSuccess);
}

UINT32 CGADelay::GetDelay()
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	return(	m_fDelay * fSR );
}

bool CGADelay::SetDelayTime(FLOAT32 fDelayTime)
{
	m_fDelay = fDelayTime;

//	FLOAT32 fSR = g_GASystem.GetSampleRate();
//	UINT32 nDelay = fSR * fDelayTime;

//	bool bSuccess = SetDelay(nDelay);
	return(true);
}


FLOAT32 CGADelay::GetDelayTime()
{
//	FLOAT32 fSR = g_GASystem.GetSampleRate();
//	FLOAT32 fDelayTime = fSR / m_nDelay;
	return(	m_fDelay );
}

BOOL CGADelay::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Delay") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetDelayTime(fValue);
		b = TRUE;
	}

	return(b);
}

//#define IP_LINEAR(a,b,f)  a*(1.0f-f) + b*f ) // interpolate linear
// Linear interpolation between A and B:
#define LIP( T, A, B )   ( (A) + (T) * ( (B) - (A) ) )

UINT32 CGADelay::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfDM = GetSignalData(Delay); // delay modulation
	
	FLOAT32 fDelay = m_fDelay;
	FLOAT32 fDM =0.0f;
	INT32 nReadPosA = 0;
	INT32 nReadPosB = 0;
	INT32 nDelay = 0;
	static UINT32 nCount =0;

	if(pfIn != NULL &&  pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
			
			if(pfDM != NULL) {

				fDM = pfDM[i]; // -1 to +1
				fDM += 1.0f; // 0 to 2
				fDM *= 0.5f; // 0 to 1
			}
			
		
			fDelay = m_fDelay * fDM * fSR; // modulated delay
			nDelay = (UINT32)fDelay;
			FLOAT32 fFract = fDelay - nDelay;

			m_pfDelayBuffer[m_nWritePos] = pfIn[i];

			nReadPosA = m_nWritePos - nDelay;

			while(nReadPosA < 0)
				nReadPosA += m_nDelayBufferSize;

			nReadPosB = nReadPosA-1;

			while(nReadPosB < 0)
				nReadPosB += m_nDelayBufferSize;

	
			FLOAT32 fa = m_pfDelayBuffer[nReadPosA];
			fa *= (1.0f - fFract); 
			FLOAT32 fb = m_pfDelayBuffer[nReadPosB];
			fb *= fFract;

			pfOut[i] = (fa + fb); // * fAmplitude;

			m_nWritePos++;
			if(m_nWritePos >= m_nDelayBufferSize) 
				m_nWritePos = 0;	


		}
	}

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
