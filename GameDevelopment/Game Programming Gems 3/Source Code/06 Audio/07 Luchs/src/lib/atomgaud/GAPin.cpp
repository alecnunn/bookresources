// GAPin Implementation
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
#include "GAPin.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAPin() { return( new CGAPin); }
const CClass CGAPin::classCGAPin(CID_GAPin, CID_GAObject, "CGAPin", CreateCGAPin);
const IClass*	CGAPin::GetClass() { return((const IClass*)&classCGAPin); }


// CTOR
CGAPin::CGAPin(IGAProcessor* pProcessor, UINT32 nPinID, ATOM_CHAR* pszName, IGASignal* pSignal)
{
	m_nPinID = nPinID;
	m_pConnectedPin = NULL;

	if(pszName != NULL)
		SetName(pszName);
	else
		SetDefaultName();
	
	m_pProcessor = pProcessor;
	m_pSignal = pSignal;
}

// DTOR
CGAPin::~CGAPin()
{	
	Terminate();
}


void CGAPin::Terminate()
{
	CGAObject::Terminate();
}

UINT32 CGAPin::GetID()
{
	return(m_nPinID);
}

BOOL CGAPin::IsInput()
{
	BOOL b = m_nPinID < SampleOut;
	return(b);
}



FLOAT32* CGAPin::GetSignalData()
{
	FLOAT32* pData = NULL;
	
	if(m_pSignal != NULL)
		pData = m_pSignal->GetData();

	return(pData);
}

IGASignal* CGAPin::GetSignal()
{
	return(m_pSignal);
}


BOOL CGAPin::SetSignal(IGASignal* pSignal)
{
	m_pSignal = pSignal;
	return(TRUE);
}

		
void CGAPin::SetConnectedPin(IGAPin* pPin)
{
	m_pConnectedPin = pPin;
}

IGAPin* CGAPin::GetConnectedPin()
{
	return(m_pConnectedPin);
}

IGAProcessor* CGAPin::GetProcessor()
{
	return(m_pProcessor);
}


// set a default name for this pintype
void CGAPin::SetDefaultName()
{
	ATOM_CHAR* psz = CGAPins::FindPinName(m_nPinID);
	SetName(psz);
}






#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
