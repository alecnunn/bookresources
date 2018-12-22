// GASampleAndHold Implementation
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
#include "GASampleAndHold.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASampleAndHold() { return( new CGASampleAndHold); }
const CClass CGASampleAndHold::classCGASampleAndHold(CID_GASampleAndHold, CID_GAProcessor, "CGASampleAndHold", CreateCGASampleAndHold);
const IClass*	CGASampleAndHold::GetClass() { return((const IClass*)&classCGASampleAndHold); }


// CTOR
CGASampleAndHold::CGASampleAndHold()
{
	SetName("SampleAndHold");

	m_nHoldTime = 4000; // recalc each nth sample
	m_nCounter = 0; // current index
	m_fCurrSample = 0.0f;
	
	AppendPin(SampleOut);
	AppendPin(SampleIn);
	AppendPin(HoldTime);

//	FLOAT32** pSignalIn = GetSignal(SampleIn);
//	SetSignal(SampleOut, pSignalIn); // inplace processing

}

// DTOR
CGASampleAndHold::~CGASampleAndHold()
{
	Terminate();
}

void CGASampleAndHold::Terminate()
{
	CGAProcessor::Terminate();
}



UINT32 CGASampleAndHold::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
//	FLOAT32** pSignalIn = GetSignal(SampleIn);
//	SetSignal(SampleOut, pSignalIn); // inplace processing

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfHold = GetSignalData(HoldTime);

	for (UINT32 i=0; i < nFrames; i++)
	{

		if(m_nCounter == 0)
			m_fCurrSample = pfIn[i]; // sample

		pfIn[i] =  m_fCurrSample; // write out

		m_nCounter++;
		if(m_nCounter == m_nHoldTime)
			m_nCounter = 0;
	}

	return(0);
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
