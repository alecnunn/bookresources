// GABalance Implementation
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
#include "GABalance.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGABalance() { return( new CGABalance); }
const CClass CGABalance::classCGABalance(CID_GABalance, CID_GAProcessor, "CGABalance", CreateCGABalance);
const IClass*	CGABalance::GetClass() { return((const IClass*)&classCGABalance); }


// CTOR
CGABalance::CGABalance()
{
	SetName("Balance");
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
CGABalance::~CGABalance()
{

}

void CGABalance::Terminate()
{
	

	CGAProcessor::Terminate();
}

// set level of input 1
void CGABalance::SetBalance(FLOAT32 fLevel1)
{
	m_fBalance = fLevel1;
}


BOOL CGABalance::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Balance") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetBalance(fValue);
		b = TRUE;
	}

	return(b);
}

UINT32 CGABalance::Process(IObject* pob)
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
