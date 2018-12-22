// GAKeyboardController Implementation
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
#include "GAKeyboardController.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAKeyboardController() { return( new CGAKeyboardController); }
const CClass CGAKeyboardController::classCGAKeyboardController(CID_GAKeyboardController, CID_GAProcessor, "CGAKeyboardController", CreateCGAKeyboardController);
const IClass*	CGAKeyboardController::GetClass() { return((const IClass*)&classCGAKeyboardController); }


// CTOR
CGAKeyboardController::CGAKeyboardController()
{
	SetName("KeyboardController");

//	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size
//	AppendPin(SampleOut, "SampleOut", pSampleOut);
	m_fSample = 0.0f;
	m_fAmount = 0.0f;

	memset(m_OutValue,0, sizeof(m_OutValue));
}

// DTOR
CGAKeyboardController::~CGAKeyboardController()
{
	Terminate();
}

// explicit terminate baseclass
void CGAKeyboardController::Terminate()
{
	UINT32 nCount = m_Array.size();

	for(UINT32 i=0; i < nCount; i++)  {
		KeyControl* pControl = m_Array.at(i);
		delete pControl;
		pControl = NULL;
	}

	m_Array.clear();


	CGAProcessor::Terminate();
}


// map a key to an output pin
// set the value which should appear at the output, if the key is pressed
// with lag = 0 the value appears immediatly
// lag > 0 sets a delay in seconds
void CGAKeyboardController::AppendKey(UINT32 nOut, UINT32 nKey, FLOAT32 fValue, FLOAT32 fLag)
{
	UINT32 nOutPins = m_Pins.GetCount(IGAPin::Output);

	if( nOut >= nOutPins) {
		IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size
		AppendPin(SampleOut + nOut, "SampleOut", pSampleOut);
	}


	KeyControl* pKey = new KeyControl;
	pKey->nOut = nOut;
	pKey->nKey = nKey;
	pKey->fLag = fLag;
	pKey->fTarget = fValue;
	pKey->pValue = &m_OutValue[nOut];

	pKey->pValue->fCurrValue = 0.0f; // the current value
//	pKey->pValue->fValue = fValue; // the value to reach

	m_Array.push_back(pKey);
	
}




UINT32 CGAKeyboardController::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	IGAKeyboard* pKeyboard =  g_GASystem.GetKeyboard();
	
	FLOAT32* pfOut = NULL; // GetSignalData(SampleOut);
	KeyControl* pKey = NULL;


	UINT32 nCount = m_Array.size();
	OutValue* pOutValue = NULL;

	for(UINT32 k=0;k <nCount;k++) {

		pKey = m_Array.at(k);
		pOutValue = pKey->pValue;

		pfOut = GetSignalData(SampleOut + pKey->nOut);
		UINT32 nDown = pKeyboard->GetKey(pKey->nKey);

		if(nDown != 0) {

			FLOAT32 fCurr = pOutValue->fCurrValue;
			FLOAT32 fTarget = pKey->fTarget;
			FLOAT32 fDelta = fTarget - fCurr;
			FLOAT32 fIncr = 0.0005f;
			
			if(fDelta > 0.0f) {
				if(pOutValue->fCurrValue + fIncr < fTarget)
					pOutValue->fCurrValue += fIncr;
			}
			else if(fDelta < 0.0f) {
				if(pOutValue->fCurrValue - fIncr > fTarget)
					pOutValue->fCurrValue -= fIncr;
			}
			else
				pOutValue->fCurrValue = fTarget;



			for( UINT32 i=0; i< nFrames; i++ )
			{
				pfOut[i] = pOutValue->fCurrValue;
			} 

		}

	} // for key


	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------

/*
UINT32 CGAKeyboardController::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	IGAKeyboard* pKeyboard =  g_GASystem.GetKeyboard();
	
	FLOAT32* pfOut = NULL; // GetSignalData(SampleOut);
	KeyControl* pKey = NULL;


	UINT32 nCount = m_Array.size();
	OutValue* pValue = NULL;

	for(UINT32 k=0;k <nCount;k++) {

		pKey = m_Array.at(k);
		pValue = pKey->pValue;

		pfOut = GetSignalData(SampleOut + pKey->nOut);
		UINT32 nDown = pKeyboard->GetKey(pKey->nKey);

		if(nDown != 0) {

			pValue->fAmount += pKey->fStep;
			pValue->fValue += pValue->fAmount;
			pValue->fValue *= pKey->fDamping;

			if( pValue->fValue >= 1.0f )	
				pValue->fValue = 1.0f;
			
			if( pValue->fValue <= -1.0f )	
				pValue->fValue = -1.0f;

			for( UINT32 i=0; i< nFrames; i++ )
			{
				pfOut[i] = pValue->fValue;
			} 

		}

	} // for key


	return(0);

}*/