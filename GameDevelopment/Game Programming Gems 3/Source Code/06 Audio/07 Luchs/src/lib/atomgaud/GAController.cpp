// GAController Implementation
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
#include "GAController.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAController() { return( new CGAController); }
const CClass CGAController::classCGAController(CID_GAController, CID_GAProcessor, "CGAController", CreateCGAController);
const IClass*	CGAController::GetClass() { return((const IClass*)&classCGAController); }


// CTOR
CGAController::CGAController()
{
	SetName("Controller");

	m_ppSignal = NULL;
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(ControlIn);

}

// DTOR
CGAController::~CGAController()
{

}

void CGAController::Terminate()
{
	CGAProcessor::Terminate();
}


BOOL CGAController::SetSignal(UINT32 nIndex, FLOAT32** ppSignal)
{
	m_ppSignal = ppSignal;
	return(TRUE);
}




// This unit fills the internal buffer with the value 
// currently found at the signal pointer
UINT32 CGAController::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = *m_ppSignal;
	FLOAT32* pfOut = GetSignalData(SampleOut);

	if(pfIn != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
			 pfOut[i] = *pfIn; // fill with current value at input
		}

		
	}

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
