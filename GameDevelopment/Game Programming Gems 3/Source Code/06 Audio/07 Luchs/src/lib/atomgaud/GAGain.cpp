// GAGain Implementation
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
#include "GAGain.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAGain() { return( new CGAGain); }
const CClass CGAGain::classCGAGain(CID_GAGain, CID_GAProcessor, "CGAGain", CreateCGAGain);
const IClass*	CGAGain::GetClass() { return((const IClass*)&classCGAGain); }


// CTOR
CGAGain::CGAGain()
{
	SetName("Gain");

	AppendPin(SampleOut);
	AppendPin(SampleIn);
	AppendPin(AM);


}

// DTOR
CGAGain::~CGAGain()
{
	Terminate();
}

void CGAGain::Terminate()
{
	CGAProcessor::Terminate();
}

UINT32 CGAGain::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfAmplitude = GetSignalData(Amplitude);

	FLOAT32 fAmplitude = 1.0f;

	if(pfIn != NULL ) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
		
				// unipolar amplitude modulation
			if(pfAmplitude != NULL) {
				
				fAmplitude = pfAmplitude[i];
			}

			// bipolar amplitude modulation
			if(pfAM != NULL) {
				
				FLOAT32 fMod = pfAM[i];
				fMod += 1.0f;
				fMod *= 0.5f;

				fAmplitude *= fMod;
			}

			 pfOut[i] = pfIn[i] * fAmplitude;
		}

		
	}

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
