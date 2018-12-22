// GACubic Implementation
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
#include "GACubic.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGACubic() { return( new CGACubic); }
const CClass CGACubic::classCGACubic(CID_GACubic, CID_GAProcessor, "CGACubic", CreateCGACubic);
const IClass*	CGACubic::GetClass() { return((const IClass*)&classCGACubic); }


// CTOR
CGACubic::CGACubic()
{
   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn);

}

// DTOR
CGACubic::~CGACubic()
{

}

// explicit terminate baseclass
void CGACubic::Terminate()
{

	CGAProcessor::Terminate();
}

UINT32 CGACubic::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);

	for( UINT32 i=0; i< nFrames; i++ )
	{

		pfOut[i] = pfIn[i] * pfIn[i] * pfIn[i];
		
	}


	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
