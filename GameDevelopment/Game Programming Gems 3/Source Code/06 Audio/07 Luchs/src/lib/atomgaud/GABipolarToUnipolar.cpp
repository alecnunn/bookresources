// GABipolarToUnipolar Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Most oscillator signals are bipolar with a range of(-1.0 to +1.0).
//				Other functions, like line and envelope are unipolar (0.0 to +1.0).
//				This processor converts from bipolar to unipolar
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
#include "GABipolarToUnipolar.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGABipolarToUnipolar() { return( new CGABipolarToUnipolar); }
const CClass CGABipolarToUnipolar::classCGABipolarToUnipolar(CID_GABipolarToUnipolar, CID_GAProcessor, "CGABipolarToUnipolar", CreateCGABipolarToUnipolar);
const IClass*	CGABipolarToUnipolar::GetClass() { return((const IClass*)&classCGABipolarToUnipolar); }


// CTOR
CGABipolarToUnipolar::CGABipolarToUnipolar()
{
	AppendPin(SampleOut);
	AppendPin(SampleIn);

//	IGASignal* pSampleIn = GetSignal(SampleIn);
//	SetSignal(SampleOut, pSampleIn); // inplace

}

// DTOR
CGABipolarToUnipolar::~CGABipolarToUnipolar()
{

}

void CGABipolarToUnipolar::Terminate()
{
	CGAProcessor::Terminate();

}

// This unit operates inplace, using the same buffer
// for sample input and output
UINT32 CGABipolarToUnipolar::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut); // same like in

	if(pfIn != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
			 // convert range ( -1 +1)---> ( 0.0 --> 1.0 )
			 
			 FLOAT fSample = pfIn[i];
			 fSample += 1.0f;
			 fSample *= 0.5f;

			 pfIn[i] = fSample;
		}
	}

	return(0);
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
