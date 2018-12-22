// GAMixer4 Implementation
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
#include "GAMixer.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAMixer() { return( new CGAMixer); }
const CClass CGAMixer::classCGAMixer(CID_GAMixer, CID_GAProcessor, "CGAMixer", CreateCGAMixer);
const IClass*	CGAMixer::GetClass() { return((const IClass*)&classCGAMixer); }


// CTOR
CGAMixer::CGAMixer()
{
	SetName("Mixer");
	m_fAtt[0] = 0.0f; 
	m_fAtt[1] = 0.0f; 
	m_fAtt[2] = 0.0f;
	m_fAtt[3] = 0.0f;  
	m_fAtt[4] = 0.0f;  
	m_fAtt[5] = 0.0f;  
	m_fAtt[6] = 0.0f;  
	m_fAtt[7] = 0.0f;  

   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size
	AppendPin(SampleOut, "SampleOut", pSampleOut);
	m_nChannels = 0;
}

// DTOR
CGAMixer::~CGAMixer()
{
	Terminate();
}

// explicit terminate baseclass
void CGAMixer::Terminate()
{

	CGAProcessor::Terminate();
}

BOOL CGAMixer::SetInputGain(UINT32 nChannel, FLOAT32 fGain)
{
	BOOL b = FALSE;
	if(nChannel < m_nChannels) {
	
		m_fAtt[nChannel] = fGain;
		b = TRUE;
	}
	return(b);
}

BOOL CGAMixer::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strncmp(pszName, "InputGain", 9) == 0 && pszValue != NULL) { // InputGain1,InputGain2,InputGain3
		CHAR8* pszIndex = pszName+9;
		UINT32 nIndex = atoi(pszIndex);
		FLOAT32 fValue = atof(pszValue);
		SetInputGain(nIndex-1, fValue);
		b = TRUE;
	}

	return(b);
}


UINT32 CGAMixer::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	// implement in derived class

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
