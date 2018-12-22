// GAPins Implementation
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
//				2001-11-03	changed indexed pins to id-mapped pins
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GAPins.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAPins() { return( new CGAPins); }
const CClass CGAPins::classCGAPins(CID_GAPins, CID_GAObject, "CGAPins", CreateCGAPins);
const IClass*	CGAPins::GetClass() { return((const IClass*)&classCGAPins); }


// common pin names
typedef struct tagIDName {

	UINT32 nID;
	CHAR8* pszName;

} IDNAME;

IDNAME	g_idname[] = {

	SampleIn, "SampleIn",
	SampleIn1, "SampleIn1",
	SampleIn2, "SampleIn2",
	SampleIn3, "SampleIn3",
	SampleIn4, "SampleIn4",
	SampleIn5, "SampleIn5",
	SampleIn6, "SampleIn6",
	SampleIn7, "SampleIn7",
	SampleIn8, "SampleIn8",

	ControlIn, "ControlIn",
	ControlIn1, "ControlIn1",
	ControlIn2, "ControlIn2",
	ControlIn3, "ControlIn3",

	FM, "FM",
	AM, "AM",
	Frequency, "Frequency",
	Amplitude, "Amplitude",
	Phase, "Phase",
	Balance, "Balance",
	BalanceU, "BalanceU",
	Delay, "Delay",
	
	SampleTime, "SampleTime",
	HoldTime, "HoldTime",
	TRIG, "TRIG",
	Cutoff, "Cutoff",
	Harmonics, "Harmonics",
	StartTime, "StartTime",
	Duration, "Duration",
	PulseWidth, "PulseWidth",
	Clipping, "Clipping",



	SampleThru, "SampleIn",
	SampleOut, "SampleOut",
	SampleOut1, "SampleOut1",
	SampleOut2, "SampleOut2",
	SampleOut3, "SampleOut3",
	SampleOut4, "SampleOut4",

	ControlOut, "ControlOut",
	DeltaOut,	"DeltaOut",
};




// CTOR
CGAPins::CGAPins()
{

}

// DTOR
CGAPins::~CGAPins()
{
	Terminate();
}

void CGAPins::Terminate()
{

	UINT32 nCount = GetCount();
	for(UINT32 i=0; i < nCount; i++)  {
		IGAPin* pPin = m_PinArray.at(i);
		pPin->Terminate();
		delete pPin;
		pPin = NULL;
	}

	m_PinArray.clear();
	CGAObject::Terminate();

}


UINT32 CGAPins::GetCount(UINT32 nWhich)
{
	UINT32 nCount = 0;
	UINT32 nInputs = 0;
	UINT32 nOutputs=0;

	if(nWhich == (IGAPin::Input || IGAPin::Output))
		nCount = m_PinArray.size();
	else {

		UINT32 nPins = m_PinArray.size();
		for(UINT32 i=0; i < nPins; i++)  {

			IGAPin* pPin = m_PinArray.at(i);
			if(pPin->IsInput())
				nInputs++;
			else
				nOutputs++;
		}

		if(nWhich == IGAPin::Input)
			nCount = nInputs;
		else if (nWhich == IGAPin::Output)
			nCount = nOutputs;

	}



	return(nCount);
}
	

UINT32 CGAPins::Append(IGAPin* pPin)
{
	m_PinArray.push_back(pPin);
	UINT32 nCount = GetCount();
	return(nCount);
}


IGAPin*	CGAPins::GetAt(UINT32 nIndex)
{
	IGAPin* pPin = NULL;
	UINT32 nCount = GetCount();

	if(nIndex < nCount)
		pPin = m_PinArray.at(nIndex);

	return(pPin);
}


IGAPin*	CGAPins::GetFirst()
{
	IGAPin* pPin = GetAt(0);
	return(pPin);
}

IGAPin*	CGAPins::GetLast()
{
	UINT32 nCount = GetCount();
	if(nCount < 1)
		return(NULL);

	IGAPin* pPin = GetAt(nCount-1);
	return(pPin);
}

// find from specified type
IGAPin*	CGAPins::Find(UINT32 nPinID)
{
	UINT32 nCount = GetCount();
	for(UINT32 i=0;i < nCount;i++) {
		
		IGAPin* pPin = m_PinArray.at(i);
		UINT32 id = pPin->GetID();
		if(id == nPinID)
			return(pPin); // found
	}

	return(NULL);
}




FLOAT32* CGAPins::GetSignalData(UINT32 nPinID)
{
	FLOAT32* pData = NULL;
	IGASignal* pSignal = GetSignal(nPinID);

	if(pSignal != NULL)
		pData = pSignal->GetData();

	return(pData);
}


IGASignal* CGAPins::GetSignal( UINT32 nPinID)
{
	IGASignal* pSignal = NULL;
	IGAPin* pPin = Find(nPinID); 

	if(pPin != NULL)
		pSignal = pPin->GetSignal();

	return(pSignal);
}

BOOL CGAPins::SetSignal(UINT32 nPinID, IGASignal* pSignal)
{
	BOOL b = FALSE;
	IGAPin* pPin = Find(nPinID); 

	if(pPin != NULL)
		b = pPin->SetSignal(pSignal);

	return(b); 
}

// find pinid having a name
UINT32 CGAPins::FindPinID(CHAR8* pszName)
{
	CGAObject temp;
	temp.SetName(pszName);

	UINT32 nCount = sizeof(g_idname) / sizeof(g_idname[0]);

	for(UINT32 i=0;i < nCount; i++) {

		IDNAME idn = g_idname[i];
		if(temp.EqualsName(idn.pszName)==0)
			return(idn.nID);
	}

	return(0); 
}

// find pinname having an id
CHAR8* CGAPins::FindPinName(UINT32 nID)
{

	UINT32 nCount = sizeof(g_idname) / sizeof(g_idname[0]);

	for(UINT32 i=0;i < nCount; i++) {

		IDNAME idn = g_idname[i];
		if(idn.nID==nID)
			return(idn.pszName);
	}

	return(NULL); 
}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
