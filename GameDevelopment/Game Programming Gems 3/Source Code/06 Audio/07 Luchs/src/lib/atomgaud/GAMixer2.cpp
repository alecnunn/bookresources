// GAMixer2 Implementation
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
#include "GAMixer2.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAMixer2() { return( new CGAMixer2); }
const CClass CGAMixer2::classCGAMixer2(CID_GAMixer2, CID_GAMixer, "CGAMixer2", CreateCGAMixer2);
const IClass*	CGAMixer2::GetClass() { return((const IClass*)&classCGAMixer2); }


// CTOR
CGAMixer2::CGAMixer2()
{
	SetName("Mixer2");

 	m_nChannels = 2;
	FLOAT32 fGain = 1.0f / (FLOAT32)m_nChannels; 

	m_fAtt[0] = fGain; 
	m_fAtt[1] = fGain; 

   // create the pins
	AppendPin(SampleIn1);
	AppendPin(SampleIn2);

}

// DTOR
CGAMixer2::~CGAMixer2()
{

}

void CGAMixer2::Terminate()
{
	

	CGAMixer::Terminate();
}


UINT32 CGAMixer2::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn1 = GetSignalData(SampleIn1);
	FLOAT32* pfIn2 = GetSignalData(SampleIn2);

	for( UINT32 i=0; i< nFrames; i++ )
	{
		FLOAT32 f1 = pfIn1 ? pfIn1[i] * m_fAtt[0] : 0.0f;
		FLOAT32 f2 = pfIn2 ? pfIn2[i] * m_fAtt[1] : 0.0f;

		pfOut[i] = f1+f2;
	}

	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
