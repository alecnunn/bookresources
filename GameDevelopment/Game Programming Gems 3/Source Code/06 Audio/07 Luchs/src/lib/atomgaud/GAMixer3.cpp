// GAMixer3 Implementation
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
#include "GAMixer3.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAMixer3() { return( new CGAMixer3); }
const CClass CGAMixer3::classCGAMixer3(CID_GAMixer3, CID_GAMixer, "CGAMixer3", CreateCGAMixer3);
const IClass*	CGAMixer3::GetClass() { return((const IClass*)&classCGAMixer3); }


// CTOR
CGAMixer3::CGAMixer3()
{
	SetName("Mixer3");
	m_nChannels = 3;
	FLOAT32 fGain = 1.0f / (FLOAT32)m_nChannels; 

	m_fAtt[0] = fGain; 
	m_fAtt[1] = fGain; 
	m_fAtt[2] = fGain;   

   // create the pins
	AppendPin(SampleIn1);
	AppendPin(SampleIn2);
	AppendPin(SampleIn3);

}

// DTOR
CGAMixer3::~CGAMixer3()
{
	Terminate();
}

// explicit terminate baseclass
void CGAMixer3::Terminate()
{

	CGAMixer::Terminate();
}


UINT32 CGAMixer3::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn1 = GetSignalData(SampleIn1);
	FLOAT32* pfIn2 = GetSignalData(SampleIn2);
	FLOAT32* pfIn3 = GetSignalData(SampleIn3);

	for( UINT32 i=0; i< nFrames; i++ )
	{
		FLOAT32 f1 = pfIn1 ? pfIn1[i] * m_fAtt[0] : 0.0f;
		FLOAT32 f2 = pfIn2 ? pfIn2[i] * m_fAtt[1] : 0.0f;
		FLOAT32 f3 = pfIn3 ? pfIn3[i] * m_fAtt[2] : 0.0f;

		pfOut[i] = f1+f2+f3;
	}
	

	return(0);
}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
 