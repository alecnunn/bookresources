// GASelector2 Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	This unit flips between two signals on each trigger
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
#include "GASelector2.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASelector2() { return( new CGASelector2); }
const CClass CGASelector2::classCGASelector2(CID_GASelector2, CID_GAProcessor, "CGASelector2", CreateCGASelector2);
const IClass*	CGASelector2::GetClass() { return((const IClass*)&classCGASelector2); }


// CTOR
CGASelector2::CGASelector2()
{
	SetName("Selector2");
	m_nChannels= 2;
	m_nSelection=0;		// current channel to output
	m_fLastImpulse = 0.0f;

   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn1);
	AppendPin(SampleIn2);
	AppendPin(TRIG);
}

// DTOR
CGASelector2::~CGASelector2()
{
	Terminate();
}

// explicit terminate baseclass
void CGASelector2::Terminate()
{

	CGAProcessor::Terminate();
}


// @ currently using a new out buffer
UINT32 CGASelector2::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn1 = GetSignalData(SampleIn1);
	FLOAT32* pfIn2 = GetSignalData(SampleIn2);
	FLOAT32* pfTrigger = GetSignalData(TRIG);

	if(pfIn1 != NULL && pfIn2 != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{

			if(pfTrigger != NULL) {

				FLOAT32 fTrigger = pfTrigger[i] - m_fLastImpulse;
				if(fTrigger  > 0.5f ) {

					m_nSelection++;
					if(m_nSelection >= m_nChannels)
						m_nSelection = 0;

				}

				m_fLastImpulse = pfTrigger[i];

			}

			if(m_nSelection == 0)
				pfOut[i] = pfIn1[i];
			else
				pfOut[i] = pfIn2[i];
		}
	}

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
