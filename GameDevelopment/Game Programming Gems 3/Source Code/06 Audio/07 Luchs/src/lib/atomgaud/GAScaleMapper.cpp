// GAScaleMapper Implementation
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
#include "GAScaleMapper.h"

const FLOAT32 g_fChromaticFreqRatio = 1.05946309435930f; // the ratio of the frequencies of two adjacent notes  

#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAScaleMapper() { return( new CGAScaleMapper); }
const CClass CGAScaleMapper::classCGAScaleMapper(CID_GAScaleMapper, CID_GAProcessor, "CGAScaleMapper", CreateCGAScaleMapper);
const IClass*	CGAScaleMapper::GetClass() { return((const IClass*)&classCGAScaleMapper); }


// CTOR
CGAScaleMapper::CGAScaleMapper()
{
	InitScale();


	SetName("ScaleMapper");

	AppendPin(SampleOut);
	AppendPin(SampleIn);
	AppendPin(ControlIn, "Scale");


}

// DTOR
CGAScaleMapper::~CGAScaleMapper()
{
	Terminate();
}

void CGAScaleMapper::Terminate()
{
	CGAProcessor::Terminate();
}


void CGAScaleMapper::InitScale()
{
	FLOAT32 fValue= 1.0f;
	for(INT32 i=127;i >= 0;i--) {	

		m_fChromaticScale[i] = fValue;
		fValue /= g_fChromaticFreqRatio;
	}


	// build pentatonic scale
	UINT32 nPentaScale[] = {2,2,3,2,3 }; // c,d,e,g,a
	UINT32 nIndex=4;
	UINT32 iDest=0;
	for(i=0;i < 128;) {
		
		fValue = m_fChromaticScale[i];
		m_fPentatonicScale[iDest++] = fValue;
		if(++nIndex >= 5)
			nIndex=0;
		i+= nPentaScale[nIndex];
	};


}

BOOL CGAScaleMapper::SelectScale(UINT32 nScale, UINT32 nBase)
{
	return(TRUE);
}

FLOAT32 CGAScaleMapper::FindNote(FLOAT32 fNote)
{
//	FLOAT32 fValue= FindChromaticNote(fNote);
	FLOAT32 fValue= FindPentatonicNote(fNote);
	return(fValue); 
}



// map an input note between 0 and 1 to a quantized
// value of the chromatic scale
FLOAT32 CGAScaleMapper::FindChromaticNote(FLOAT32 fNote)
{
	FLOAT32 fValue= 1.0f;
	for(INT32 i=0;i < 128;i++) {	

		fValue = m_fChromaticScale[i];
		if(fValue  >= fNote)
			return(fValue);

	}

	return(0.0f); // should not happen if input is between 0 and 1
}


// map an input note between 0 and 1 to a quantized
// value of the pentatonic scale
FLOAT32 CGAScaleMapper::FindPentatonicNote(FLOAT32 fNote)
{
	FLOAT32 fValue= 1.0f;
	for(INT32 i=0;i < 128;i++) {	

		fValue = m_fPentatonicScale[i];
		if(fValue  >= fNote)
			return(fValue);

	}

	return(0.0f); // should not happen if input is between 0 and 1
}


// convert input to quantized scale (inplace)
UINT32 CGAScaleMapper::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
//	FLOAT32** pSignalIn = GetSignal(SampleIn);
//	SetSignal(SampleOut, pSignalIn); // inplace processing

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);

	for (UINT32 i=0; i < nFrames; i++)
	{
		pfOut[i] = FindNote(pfIn[i]); // write out

	}

	return(0);
}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
