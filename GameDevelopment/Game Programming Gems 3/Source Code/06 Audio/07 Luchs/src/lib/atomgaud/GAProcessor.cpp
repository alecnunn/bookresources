// GAProcessor Implementation
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
#include "GAProcessor.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAProcessor() { return( new CGAProcessor); }
const CClass CGAProcessor::classCGAProcessor(CID_GAProcessor, CID_GAObject, "CGAProcessor", CreateCGAProcessor);
const IClass*	CGAProcessor::GetClass() { return((const IClass*)&classCGAProcessor); }


// CTOR
CGAProcessor::CGAProcessor()
{
	SetName("Processor");
	m_bBypass = false;
	AppendPin(Amplitude);	// unipolar level pin

	m_nSampleCounter = 0;
}

// DTOR
CGAProcessor::~CGAProcessor()
{
	Terminate();
}

void CGAProcessor::Terminate()
{
	CGAObject::Terminate();
	m_Pins.Terminate();

}

// return 0 on success
UINT32 CGAProcessor::Process(IObject* pContext)
{
	// implement in derived class
	// this baseclass has no pins
	return(0);
}


// recalc internal state, when a parameter has changed
void CGAProcessor::Update()
{
	// implement in derived class
}

void CGAProcessor::Trigger()
{
	// implement in derived class
}

void CGAProcessor::Start()
{
	// implement in derived class
}

void CGAProcessor::Stop()
{
	// implement in derived class
}

// leave signal unchanged
void CGAProcessor::SetBypass(bool bBypass)
{
	m_bBypass = bBypass;
}


IGAPins* CGAProcessor::GetPins()
{
	return((IGAPins*)&m_Pins);
}


// CReate and append a new pin
IGAPin*	CGAProcessor::AppendPin(UINT32 nPinID, ATOM_CHAR* pszName, IGASignal* pSignal)
{
	CGAPin* pPin = new CGAPin((IGAProcessor*)this, nPinID, pszName, pSignal);
	m_Pins.Append((IGAPin*)pPin);

	return((IGAPin*)pPin);
}

// this is somewhat complex, because we try different ways
// finding the signal.
// shortcut to get the sample data from specified pin
// supports currently only float data
// if this is an output pin and we have no signal buffer
// at this pin, we retrieve the signal from the sample input  pin
// also this can be an input pin and we may ask out upstream
// connected processor
FLOAT32* CGAProcessor::GetSignalData( UINT32 nPinID)
{
	FLOAT32* pfSignal = NULL;
	IGAPin* pPin = m_Pins.Find(nPinID); 
	
	if(pPin == NULL)
		return(NULL);

	IGASignal* pSignal = GetSignal( nPinID);

	if( pSignal == NULL) {

		if(nPinID == SampleOut) {

	    	// try to get the samples from the input
			IGASignal* pSignalIn = GetSignal( SampleIn);
	
			if(pSignalIn != NULL)
				pfSignal = pSignalIn->GetData();
		}
		else if(nPinID < SampleOut) { // this is an input

	    	// try to get the samples from connected processor
			// this pin is an output 
			IGAPin* pConnectedPin = pPin->GetConnectedPin();
			if(pConnectedPin != NULL) {

				pfSignal = pConnectedPin->GetSignalData();
				
				if(pfSignal == NULL) {

					// try to get the samples from the input of the connected processor
					IGAProcessor* pProcessor = pConnectedPin->GetProcessor();
					IGASignal* pSignalIn = pProcessor->GetSignal( SampleIn);
			
					if(pSignalIn != NULL)
						pfSignal = pSignalIn->GetData();


				}

			}
				
		}

	}
	else  {

		pfSignal = pSignal->GetData();
	}

	return(pfSignal);
}

/*
// get signal from pin with specified id
IGASignal* CGAProcessor::GetSignal( UINT32 nPinID)
{
	IGASignal* pSignal = m_Pins.GetSignal(nPinID);
	return(pSignal);
}

*/

IGASignal* CGAProcessor::GetSignal( UINT32 nPinID)
{
//	IGASignal* pSignal = NULL;
	IGAPin* pPin = m_Pins.Find(nPinID); 
	
	if(pPin == NULL)
		return(NULL);

	IGASignal* pSignal = m_Pins.GetSignal(nPinID);

	if( pSignal == NULL) {

		if(nPinID == SampleOut) {

	    	// try to get the samples from the input
			IGASignal* pSignalIn = GetSignal( SampleIn);
	
			if(pSignalIn != NULL)
				pSignal = pSignalIn;
		}
		else if(nPinID < SampleOut) { // this is an input

	    	// try to get the samples from connected processor
			// this pin is an output 
			IGAPin* pConnectedPin = pPin->GetConnectedPin();
			if(pConnectedPin != NULL) {

				pSignal = pConnectedPin->GetSignal();
				
				if(pSignal == NULL) {

					// try to get the samples from the input of the connected processor
					IGAProcessor* pProcessor = pConnectedPin->GetProcessor();
					IGASignal* pSignalIn = pProcessor->GetSignal( SampleIn);
			
					if(pSignalIn != NULL)
						pSignal = pSignalIn;


				}

			}
				
		}

	}
	

	return(pSignal);
}


/*
BOOL CGAProcessor::SetParameter(UINT32 idParam, FLOAT32 fValue)
{
	return(FALSE);
}
*/
BOOL CGAProcessor::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAObject::SetParameter(pszName, pszValue);


	return(b);
}

// get signal of pin with specified id
BOOL CGAProcessor::SetSignal(UINT32 nPinID, IGASignal* pSignal)
{

	BOOL b = m_Pins.SetSignal(nPinID, pSignal);
	return(b); 
}

// connect this processor to the next in chain
BOOL CGAProcessor::Connect( UINT32 npinOut, IGAProcessor* pNext,UINT32 nPinIn)
{
	IGAPin* ppinFrom = m_Pins.Find(npinOut); // out pin of this processor
	IGAPins* ppinsNext = pNext->GetPins();
	IGAPin* ppinTo = ppinsNext->Find(nPinIn); // out pin of this processor

	if(ppinFrom != NULL && ppinTo != NULL) {
		ppinFrom->SetConnectedPin(ppinTo);
		ppinTo->SetConnectedPin(ppinFrom);

		IGASignal* pSignal = GetSignal(npinOut);	// this out
		pNext->SetSignal(nPinIn, pSignal);		// next in
	}

	return(TRUE); 
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
