// GAMixer6 Implementation
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
#include "GAMixer6.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAMixer6() { return( new CGAMixer6); }
const CClass CGAMixer6::classCGAMixer6(CID_GAMixer6, CID_GAMixer, "CGAMixer6", CreateCGAMixer6);
const IClass*	CGAMixer6::GetClass() { return((const IClass*)&classCGAMixer6); }


// CTOR
CGAMixer6::CGAMixer6()
{
	SetName("Mixer6");

	m_nChannels = 6;
	FLOAT32 fGain = 1.0f / (FLOAT32)m_nChannels; 

	m_fAtt[0] = fGain; 
	m_fAtt[1] = fGain; 
	m_fAtt[2] = fGain;   
	m_fAtt[3] = fGain;  
	m_fAtt[4] = fGain;  
	m_fAtt[5] = fGain;  

   // create the pins
	AppendPin(SampleIn1);
	AppendPin(SampleIn2);
	AppendPin(SampleIn3);
	AppendPin(SampleIn4);
	AppendPin(SampleIn5);
	AppendPin(SampleIn6);

}

// DTOR
CGAMixer6::~CGAMixer6()
{
	Terminate();
}

// explicit terminate baseclass
void CGAMixer6::Terminate()
{

	CGAMixer::Terminate();
}


UINT32 CGAMixer6::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn1 = GetSignalData(SampleIn1);
	FLOAT32* pfIn2 = GetSignalData(SampleIn2);
	FLOAT32* pfIn3 = GetSignalData(SampleIn3);
	FLOAT32* pfIn4 = GetSignalData(SampleIn4);
	FLOAT32* pfIn5 = GetSignalData(SampleIn5);
	FLOAT32* pfIn6 = GetSignalData(SampleIn6);

	for( UINT32 i=0; i< nFrames; i++ )
	{
		FLOAT32 f1 = pfIn1 ? pfIn1[i] * m_fAtt[0] : 0.0f;
		FLOAT32 f2 = pfIn2 ? pfIn2[i] * m_fAtt[1] : 0.0f;
		FLOAT32 f3 = pfIn3 ? pfIn3[i] * m_fAtt[2] : 0.0f;
		FLOAT32 f4 = pfIn4 ? pfIn4[i] * m_fAtt[3] : 0.0f;
		FLOAT32 f5 = pfIn5 ? pfIn5[i] * m_fAtt[4] : 0.0f;
		FLOAT32 f6 = pfIn6 ? pfIn6[i] * m_fAtt[5] : 0.0f;

		pfOut[i] = f1+f2+f3+f4+f5+f6;
	}
	

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
