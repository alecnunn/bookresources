// GANumPadController Implementation
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
#include "GANumPadController.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGANumPadController() { return( new CGANumPadController); }
const CClass CGANumPadController::classCGANumPadController(CID_GANumPadController, CID_GAProcessor, "CGANumPadController", CreateCGANumPadController);
const IClass*	CGANumPadController::GetClass() { return((const IClass*)&classCGANumPadController); }


// CTOR
CGANumPadController::CGANumPadController()
{
	SetName("NumPadController");

	m_fSample = 0.0f;

	memset(m_KeyValue,0, sizeof(m_KeyValue));
	int nKeys = sizeof(m_KeyValue) /  sizeof(m_KeyValue[0]);
	FLOAT32 fValue = 0.5f;

	for(int i = nKeys-1; i > 0; i--) {
		m_KeyValue[i].nKey = 96+i; // 0
		m_KeyValue[i].fValue = fValue;
		fValue *= 0.5f;
	}

	m_KeyValue[10].nKey = 110; // ,
	m_KeyValue[10].fValue = 1.0f;

	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size
	AppendPin(SampleOut, "SampleOut", pSampleOut);

}

// DTOR
CGANumPadController::~CGANumPadController()
{
	Terminate();
}

// explicit terminate baseclass
void CGANumPadController::Terminate()
{

	CGAProcessor::Terminate();
}

  UINT32 CGANumPadController::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	IGAKeyboard* pKeyboard =  g_GASystem.GetKeyboard();
	
	FLOAT32* pfOut = GetSignalData(SampleOut);

	int nKeys = sizeof(m_KeyValue) /  sizeof(m_KeyValue[0]);


	for(UINT32 k=0;k <nKeys;k++) {

		UINT32 nKey = m_KeyValue[k].nKey;
		FLOAT32 fValue = m_KeyValue[k].fValue;

		UINT32 nDown = pKeyboard->GetKey(nKey);

		if(nDown != 0) 
			m_fSample = fValue;

		for( UINT32 i=0; i< nFrames; i++ )
		{
			pfOut[i] = m_fSample; // hold last value
		} 



	} // for key


	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------

/*

  UINT32 CGANumPadController::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	IGAKeyboard* pKeyboard =  g_GASystem.GetKeyboard();
	
	FLOAT32* pfOut = GetSignalData(SampleOut);

	int nKeys = sizeof(m_KeyValue) /  sizeof(m_KeyValue[0]);


	for(UINT32 k=0;k <nKeys;k++) {

		UINT32 nKey = m_KeyValue[k].nKey;
		FLOAT32 fValue = m_KeyValue[k].fValue;

		UINT32 nDown = pKeyboard->GetKey(nKey);

		if(nDown != 0) 
			m_fSample = fValue;

		for( UINT32 i=0; i< nFrames; i++ )
		{
			pfOut[i] = m_fSample; // hold last value
		} 



	} // for key


	return(0);

}

  */


/*

  
UINT32 CGANumPadController::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	IGAKeyboard* pKeyboard =  g_GASystem.GetKeyboard();
	
	FLOAT32* pfOut = GetSignalData(SampleOut);

	int nKeys = sizeof(m_KeyValue) /  sizeof(m_KeyValue[0]);
	
	
	for( UINT32 i=0; i< nFrames; i++ )
	{
		for(UINT32 k=0;k <nKeys;k++) {
			
			UINT32 nKey = m_KeyValue[k].nKey;
			FLOAT32 fValue = m_KeyValue[k].fValue;
			
			UINT32 nDown = pKeyboard->GetKey(nKey);
			
			if(nDown != 0) 
				m_fSample = fValue;
			
			pfOut[i] = m_fSample; // hold last value
			
			
			
		} // for key
	} 


	return(0);

}

  */