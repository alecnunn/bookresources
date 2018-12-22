// GATriangleEnvelope Implementation
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
#include "GATriangleEnvelope.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGATriangleEnvelope() { return( new CGATriangleEnvelope); }
const CClass CGATriangleEnvelope::classCGATriangleEnvelope(CID_GATriangleEnvelope, CID_GAEnvelope, "CGATriangleEnvelope", CreateCGATriangleEnvelope);
const IClass*	CGATriangleEnvelope::GetClass() { return((const IClass*)&classCGATriangleEnvelope); }


// CTOR
CGATriangleEnvelope::CGATriangleEnvelope()
{
	SetName("TriangleEnvelope");
//	m_fSample = 0.0f;
	m_fCenter = 0.5f;
//	m_fLastImpulse = 0.0f;

	AppendPin(Center);


}

// DTOR
CGATriangleEnvelope::~CGATriangleEnvelope()
{

}

// explicit terminate baseclass
void CGATriangleEnvelope::Terminate()
{

	CGAEnvelope::Terminate();
}



void CGATriangleEnvelope::SetCenter(FLOAT32 fCenter)
{
	m_fCenter = fCenter;
}

BOOL CGATriangleEnvelope::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAEnvelope::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Center") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetCenter(fValue);
		b = TRUE;
	}

	return(b);
}


// m_fCenter is in range 0 to 1 
// center 0.5 forms a triangle, 0 a descending sawtooth
// 1 forms an ascending sawtooth
UINT32 CGATriangleEnvelope::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

//	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfTrigger = GetSignalData(TRIG);
	FLOAT32* pfCenter = GetSignalData(Center);
	FLOAT32 fCenter = m_fCenter;

	UINT32 nDuration =	m_fDuration * fSR;			// calc duration in samples
	UINT32 nCenter = nDuration *  fCenter;	// 
	FLOAT32 fIncr1 = nCenter != 0 ? 1.0f / nCenter : 0.0f;
	FLOAT32 fIncr2 = nCenter != nDuration ? 1.0f / (nDuration - nCenter) : 0.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{

			if(pfTrigger != NULL) {

				FLOAT32 fTrigger = pfTrigger[i] - m_fLastTrigger;
				if(fTrigger  > 0.5f ) {

					Trigger();

				}

				m_fLastTrigger = pfTrigger[i];

			}

			
		if(pfCenter != NULL) {

				FLOAT32 fCenter = pfCenter[i]; // -1 to +1
				
				fCenter += 1.0f;	// 0 to 2
				fCenter *= 0.5f;	// 0 to 1
				
				nCenter = nDuration *  fCenter;	// 
				fIncr1 = nCenter != 0 ? 1.0f / nCenter : 0.0f;
				fIncr2 = nCenter != nDuration ? 1.0f / (nDuration - nCenter) : 0.0f;

			}



		if(m_nSamplePos >= 0 && m_nSamplePos < nCenter)
			m_fSample += fIncr1;
		else if(m_nSamplePos >= nCenter && m_nSamplePos < nDuration)
			m_fSample -= fIncr2;
		else 
			m_fSample = 0.0f;


		pfOut[i] = m_fSample;
		
		if(m_nSamplePos < nDuration)
			m_nSamplePos++;
	}


	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
