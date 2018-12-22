// GAExpCurve Implementation
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
#include "GAExpCurve.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAExpCurve() { return( new CGAExpCurve); }
const CClass CGAExpCurve::classCGAExpCurve(CID_GAExpCurve, CID_GAProcessor, "CGAExpCurve", CreateCGAExpCurve);
const IClass*	CGAExpCurve::GetClass() { return((const IClass*)&classCGAExpCurve); }


// CTOR
CGAExpCurve::CGAExpCurve()
{
	SetName("ExpCurve");
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	m_nDuration = (UINT32)fSR; // default 1 second duration
	m_nPos =0;

	m_fStartValue = 0.0f; // default to ascending line
	m_fEndValue = 1.0f; 

	   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut",  pSampleOut);

	Trigger();
}

// DTOR
CGAExpCurve::~CGAExpCurve()
{
	Terminate();
}

// explicit terminate baseclass
void CGAExpCurve::Terminate()
{

	CGAProcessor::Terminate();
}

void CGAExpCurve::SetStartValue(FLOAT32 fStartValue)
{
	m_fStartValue = fStartValue;
	Update();
}

void CGAExpCurve::SetEndValue(FLOAT32 fEndValue)
{
	m_fEndValue = fEndValue;
	Update();
}


// recalc, when a parameter has changed
void CGAExpCurve::Update()
{
	FLOAT32 fRange = m_fEndValue - m_fStartValue;
	if(m_nDuration > 0 )
		m_fIncr = fRange / (FLOAT) m_nDuration;
}


void CGAExpCurve::Trigger()
{
	m_nPos = 0;	// reset to restart line 
	m_fLevel = m_fStartValue;
	Update();
}


UINT32 CGAExpCurve::Process(IObject* pob)
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
