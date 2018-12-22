// GALine Implementation
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
#include "GALine.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGALine() { return( new CGALine); }
const CClass CGALine::classCGALine(CID_GALine, CID_GAProcessor, "CGALine", CreateCGALine);
const IClass*	CGALine::GetClass() { return((const IClass*)&classCGALine); }


// CTOR
CGALine::CGALine()
{
	SetName("Line");
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	m_nDuration = (UINT32)fSR; // default 1 second duration
	m_nPos =0;

	m_fStartValue = 0.0f; // default to ascending line
	m_fEndValue = 1.0f; 

	   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut",  pSampleOut);
//	AppendPin(Balance, "Balance", CGAPin::Input, NULL);

	Trigger();
}

// DTOR
CGALine::~CGALine()
{

}

// call the baseclass terminate explicitly 
void CGALine::Terminate()
{
	// cleanup this


	// cleanup base
	CGAProcessor::Terminate();
}


void CGALine::SetStartValue(FLOAT32 fStartValue)
{
	m_fStartValue = fStartValue;
	Update();
}

void CGALine::SetEndValue(FLOAT32 fEndValue)
{
	m_fEndValue = fEndValue;
	Update();
}

// recalc, when a parameter has changed
void CGALine::Update()
{
	FLOAT32 fRange = m_fEndValue - m_fStartValue;
	if(m_nDuration > 0 )
		m_fIncr = fRange / (FLOAT) m_nDuration;
}


void CGALine::Trigger()
{
	m_nPos = 0;	// reset to restart line 
	m_fLevel = m_fStartValue;
	Update();
}


UINT32 CGALine::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32* pfOut = GetSignalData(SampleOut);

	if( pfOut != NULL && m_nDuration > 0 ) {
		

		for( UINT32 i=0; i< nFrames; i++ )
		{

			pfOut[i] = m_fLevel;
			if(m_nPos < m_nDuration)
				m_fLevel += m_fIncr;
			// else hold last value
			m_nPos++;
		}
	}

	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
