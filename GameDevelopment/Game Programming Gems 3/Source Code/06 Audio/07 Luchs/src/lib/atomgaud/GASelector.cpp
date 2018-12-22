// GASelector Implementation
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
#include "GASelector.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASelector() { return( new CGASelector); }
const CClass CGASelector::classCGASelector(CID_GASelector, CID_GAProcessor, "CGASelector", CreateCGASelector);
const IClass*	CGASelector::GetClass() { return((const IClass*)&classCGASelector); }


// CTOR
CGASelector::CGASelector()
{
	SetName("Selector");
	m_nMask= 3;		// first two channels active
	m_nCurrInput=0;		// current channel to output
	m_fLastImpulse = 0.0f;

   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);

	for(UINT32 in=0; in < 8; in++)	// max 8 channels
		AppendPin(SampleIn+in);

	AppendPin(TRIG);
}

// DTOR
CGASelector::~CGASelector()
{
	Terminate();
}

// explicit terminate baseclass
void CGASelector::Terminate()
{

	CGAProcessor::Terminate();
}

// bitmask for active channels
void CGASelector::ActivateInputs(UINT32 nMask)
{
	m_nMask = nMask;
}

// bitmask for active channels
void CGASelector::SelectNext()
{
	for(int i=0;i <8;i++) {

		m_nCurrInput++;
		if(m_nCurrInput >= 8)
			m_nCurrInput = 0;

		UINT32 nBit = 1 << m_nCurrInput;
		bool bActive = m_nMask & nBit;

		if(bActive)
			break;

	}

}

BOOL CGASelector::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(pszValue != NULL) {
		if(strcmp(pszName, "Selection") == 0 ) {

			UINT32 nValue = atoi(pszValue);
				
			ActivateInputs(nValue);
			b = TRUE;
			
		}
	}
	return(b);
}


// @ currently using a new out buffer
UINT32 CGASelector::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfTrigger = GetSignalData(TRIG);
	FLOAT32* pfIn[8];
	
	for(UINT32 in=0; in < 8; in++)	// max 8 channels
		pfIn[in] = GetSignalData(SampleIn+in);

	
	for( UINT32 i=0; i< nFrames; i++ )
	{

		if(pfTrigger != NULL) {

			FLOAT32 fTrigger = pfTrigger[i] - m_fLastImpulse;
			if(fTrigger  > 0.5f )
				SelectNext();

			m_fLastImpulse = pfTrigger[i];

		}

		FLOAT32* pfCurrInput =  pfIn[m_nCurrInput];
		if(pfCurrInput != NULL)
			pfOut[i] = pfCurrInput[i];
		else
			pfOut[i] = 0.0f;

		
	} // for frames


	return(0);
}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
