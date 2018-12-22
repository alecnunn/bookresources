// GACrossFade2 Implementation
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
#include "GACrossFade2.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGACrossFade2() { return( new CGACrossFade2); }
const CClass CGACrossFade2::classCGACrossFade2(CID_GACrossFade2, CID_GAProcessor, "CGACrossFade2", CreateCGACrossFade2);
const IClass*	CGACrossFade2::GetClass() { return((const IClass*)&classCGACrossFade2); }


// CTOR
CGACrossFade2::CGACrossFade2()
{
	SetName("CrossFade2");
	m_fBalance = 0.5f; // center balance

   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn1);
	AppendPin(SampleIn2);
	AppendPin(Balance);
	AppendPin(BalanceU);			// unipolar balance modulation

}

// DTOR
CGACrossFade2::~CGACrossFade2()
{

}

void CGACrossFade2::Terminate()
{
	

	CGAProcessor::Terminate();
}

// set level of input 1
void CGACrossFade2::SetBalance(FLOAT32 fLevel1)
{
	m_fBalance = fLevel1;
}


BOOL CGACrossFade2::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Balance") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetBalance(fValue);
		b = TRUE;
	}

	return(b);
}

UINT32 CGACrossFade2::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn1 = GetSignalData(SampleIn1);
	FLOAT32* pfIn2 = GetSignalData(SampleIn2);
	FLOAT32* pfBBM = GetSignalData(Balance);
	FLOAT32* pfUBM = GetSignalData(BalanceU);
	FLOAT32 fBalance = m_fBalance; 

	if(pfIn1 != NULL && pfIn2 != NULL && pfOut != NULL) {
		
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



			FLOAT32 fLevel1 = fBalance ;
			FLOAT32 fLevel2 = 1.0f - fBalance;

			FLOAT32 fIn1 = pfIn1[i] * fLevel1;
			FLOAT32 fIn2 = pfIn2[i] * fLevel2;

			pfOut[i] = fIn1 + fIn2;
		}
	}

	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
