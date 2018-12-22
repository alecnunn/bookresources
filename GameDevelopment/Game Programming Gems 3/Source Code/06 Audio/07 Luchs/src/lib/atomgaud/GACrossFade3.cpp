// GACrossFade3 Implementation
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
#include "GACrossFade3.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGACrossFade3() { return( new CGACrossFade3); }
const CClass CGACrossFade3::classCGACrossFade3(CID_GACrossFade3, CID_GAProcessor, "CGACrossFade3", CreateCGACrossFade3);
const IClass*	CGACrossFade3::GetClass() { return((const IClass*)&classCGACrossFade3); }


// CTOR
CGACrossFade3::CGACrossFade3()
{
	SetName("CrossFade3");
	m_fAtt[0] = 1.0f / 3.0f; 
	m_fAtt[1] = 1.0f / 3.0f; 
	m_fAtt[2] = 1.0f / 3.0f; 
	m_fBalance = 0.5f;

   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn1);
	AppendPin(SampleIn2);
	AppendPin(SampleIn3);
	AppendPin(Balance); // bipolar crossfade over the 3 inputs
	AppendPin(BalanceU); // unipolar crossfade over the 3 inputs

}

// DTOR
CGACrossFade3::~CGACrossFade3()
{
	Terminate();
}

// explicit terminate baseclass
void CGACrossFade3::Terminate()
{

	CGAProcessor::Terminate();
}

BOOL CGACrossFade3::SetInputGain(UINT32 nChannel, FLOAT32 fGain)
{
	BOOL b = FALSE;
	if(nChannel < 3) {
	
		m_fAtt[nChannel] = fGain;
		b = TRUE;
	}
	return(b);
}

// set balanced mix of inputs
void CGACrossFade3::SetBalance(FLOAT32 fLevel1)
{
	m_fBalance = fLevel1;
}


BOOL CGACrossFade3::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Balance") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetBalance(fValue);
		b = TRUE;
	}

	return(b);
}


UINT32 CGACrossFade3::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn1 = GetSignalData(SampleIn1);
	FLOAT32* pfIn2 = GetSignalData(SampleIn2);
	FLOAT32* pfIn3 = GetSignalData(SampleIn3);

	FLOAT32* pfBBM = GetSignalData(Balance);
	FLOAT32* pfUBM = GetSignalData(BalanceU);
	FLOAT32 fBalance = m_fBalance; 

	if(pfIn1 != NULL
		&& pfIn2 != NULL
		&& pfIn3 != NULL
		&& pfOut != NULL) {



		for( UINT32 i=0; i< nFrames; i++ )
		{

			// unipolar balance modulation
			if(pfUBM != NULL) {
				
				fBalance = pfUBM[i];

			}

			// bipolar balance modulation
			if(pfBBM != NULL) {
				
				fBalance = pfBBM[i];
				fBalance += 1.0f;
				fBalance *= 0.5f;

			}		

			// sinus crossfade
			FLOAT32 fd = 1.0f / 3.0f;
			m_fAtt[0] = sinf( (fBalance+0.5f) * ATOM_PI);
			m_fAtt[1] = sinf( fBalance * ATOM_PI);
			m_fAtt[2] = sinf( (fBalance-0.5f) * ATOM_PI);

			if(m_fAtt[0] < 0.0f)
				m_fAtt[0] = 0.0f;
			if(m_fAtt[1] < 0.0f)
				m_fAtt[1] = 0.0f;
			if(m_fAtt[2] < 0.0f)
				m_fAtt[2] = 0.0f;			
			
			
			
			FLOAT32 f1 = pfIn1[i] * m_fAtt[0] * fd;
			FLOAT32 f2 = pfIn2[i] * m_fAtt[1] * fd;
			FLOAT32 f3 = pfIn3[i] * m_fAtt[2] * fd;

			pfOut[i] = f1+f2+f3;
		}
	}

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
 