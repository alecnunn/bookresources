// GAInterleaver Implementation
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
#include "GAInterleaver.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAInterleaver() { return( new CGAInterleaver); }
const CClass CGAInterleaver::classCGAInterleaver(CID_GAInterleaver, CID_GAProcessor, "CGAInterleaver", CreateCGAInterleaver);
const IClass*	CGAInterleaver::GetClass() { return((const IClass*)&classCGAInterleaver); }


// CTOR
CGAInterleaver::CGAInterleaver()
{
	SetName("Interleaver");
   // create the pins
	UINT32 nSize = g_GASystem.GetFrameCount();

	IGASignal* pSampleOut = g_GASystem.CreateSignal(nSize*2); // alloc in current frame size * 2
	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn1);
	AppendPin(SampleIn2);

}

// DTOR
CGAInterleaver::~CGAInterleaver()
{
	Terminate();
}

// explicit terminate baseclass
void CGAInterleaver::Terminate()
{

	CGAProcessor::Terminate();
}

// combine two mono signals in one interleaved stereo signal
UINT32 CGAInterleaver::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn1 = GetSignalData(SampleIn1);
	FLOAT32* pfIn2 = GetSignalData(SampleIn2);

		
	for( UINT32 i=0; i< nFrames; i++ )
	{
		FLOAT32 f1 = pfIn1[i];
		FLOAT32 f2 = pfIn2[i];

		*pfOut++ = f1;
		*pfOut++ = f2;

	}

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
