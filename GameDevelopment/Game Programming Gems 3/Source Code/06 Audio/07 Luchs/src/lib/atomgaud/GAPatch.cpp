// GAPatch Implementation
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
#include "GAPatch.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAPatch() { return( new CGAPatch); }
const CClass CGAPatch::classCGAPatch(CID_GAPatch, CID_GAProcessor, "CGAPatch", CreateCGAPatch);
const IClass*	CGAPatch::GetClass() { return((const IClass*)&classCGAPatch); }


// CTOR
CGAPatch::CGAPatch()
{
	SetName("Patch");

}

// DTOR
CGAPatch::~CGAPatch()
{
	Terminate();	
}

void CGAPatch::Terminate()
{
	m_Processors.Terminate();
	CGAObject::Terminate();

}


// send trigger to each processor in patch
void CGAPatch::Trigger()
{
	UINT32 nProcs = m_Processors.GetCount();
	for(UINT32 i=0;i <nProcs;i++) {
		IGAProcessor* pProcessor = m_Processors.GetAt(i); 
		pProcessor->Trigger();
	}

}

// send start to each processor in patch
void CGAPatch::Start()
{
	UINT32 nProcs = m_Processors.GetCount();
	for(UINT32 i=0;i <nProcs;i++) {
		IGAProcessor* pProcessor = m_Processors.GetAt(i); 
		pProcessor->Start();
	}

}


// send stop to each processor in patch
void CGAPatch::Stop()
{
	UINT32 nProcs = m_Processors.GetCount();
	for(UINT32 i=0;i <nProcs;i++) {
		IGAProcessor* pProcessor = m_Processors.GetAt(i); 
		pProcessor->Stop();
	}

}


IGAProcessors* CGAPatch::GetProcessors()
{
	return((IGAProcessors*)&m_Processors);
}


// connect by index specified  processors in this patch
//  default pins are samplein->sampleout
BOOL CGAPatch::Connect(UINT32 nFrom, UINT32 nTo, UINT32 nPinFrom, UINT32 nPinTo)
{
	BOOL bSuccess = FALSE;
	UINT32 nProcs = m_Processors.GetCount();

	if(nFrom < nProcs && nTo < nProcs) {

		IGAProcessor *pFrom = m_Processors.GetAt(nFrom); 
		IGAProcessor *pTo = m_Processors.GetAt(nTo); 

		if(pFrom != NULL)
			bSuccess = pFrom->Connect(nPinFrom,pTo, nPinTo);
	}

	return(bSuccess);
}


BOOL CGAPatch::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAObject::SetParameter(pszName, pszValue);

	if(pszValue != NULL) {
		if(strcmp(pszName, "Connection") == 0) {

			// parse pszValue which should consist of two integers
			// and two strings specifying the pin names
			UINT32 nPos[4]; // token start in value string
			CHAR8	ch= 0;
			UINT32 nLength = strlen(pszValue);
			UINT32 ip=0; // index in current buffer
			// parse the value string, assuming 4 token, no space and comma separators
			// 
			
			// first token always at 0:
			nPos[ip++] = 0;
			// replace ',' with 0 and store position
			for(int i=0;i < nLength;i++) {
				if(pszValue[i] == ',') {
					pszValue[i] = 0;
					nPos[ip++] = i+1;
				}
			}

			// the first values can be the index in the chain 
			// or the processor's name 

			UINT32 nFrom = atoi(pszValue+nPos[0]);
			UINT32 nTo = atoi(pszValue+nPos[1]);

			// if both are 0 the we assume to have a name
			if(nFrom == 0 && nTo == 0) {
				nFrom = m_Processors.GetIndex(pszValue+nPos[0]);
				nTo = m_Processors.GetIndex(pszValue+nPos[1]);
			}

			CHAR8* pszPinFrom = pszValue+nPos[2];
			CHAR8* pszPinTo = pszValue+nPos[3];
			UINT32 nPinFrom = CGAPins::FindPinID(pszPinFrom);
			UINT32 nPinTo = CGAPins::FindPinID(pszPinTo);

			Connect( nFrom,  nTo,  nPinFrom,  nPinTo);
			b = TRUE;
		}
	}
	else if(strcmp(pszName, "Description") == 0) {
	
		
	}

	return(b);
}

UINT32 CGAPatch::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	m_Processors.Process(pob);

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
