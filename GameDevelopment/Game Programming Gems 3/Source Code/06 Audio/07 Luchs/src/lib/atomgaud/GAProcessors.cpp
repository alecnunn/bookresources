// GAProcessors Implementation
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
#include "GAProcessors.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAProcessors() { return( new CGAProcessors); }
const CClass CGAProcessors::classCGAProcessors(CID_GAProcessors, CID_GAObject, "CGAProcessors", CreateCGAProcessors);
const IClass*	CGAProcessors::GetClass() { return((const IClass*)&classCGAProcessors); }


// CTOR
CGAProcessors::CGAProcessors()
{

}

// DTOR
CGAProcessors::~CGAProcessors()
{
	Terminate();
}

void CGAProcessors::Terminate()
{
	UINT32 nCount = GetCount();
	for(UINT32 i=0; i < nCount; i++)  {
		IGAProcessor* pProcessor = m_ProcessorArray.at(i);
		pProcessor->Terminate();
		delete pProcessor;
		pProcessor = NULL;
	}

	m_ProcessorArray.clear();

	CGAObject::Terminate();

}



UINT32 CGAProcessors::GetCount()
{
	return(m_ProcessorArray.size());
}
	

UINT32 CGAProcessors::Append(IGAProcessor* pProcessor)
{
	m_ProcessorArray.push_back(pProcessor);
	UINT32 nCount = GetCount();
	return(nCount);
}


IGAProcessor*	CGAProcessors::GetAt(UINT32 nIndex)
{
	IGAProcessor* pProcessor = NULL;
	UINT32 nCount = GetCount();

	if(nIndex < nCount)
		pProcessor = m_ProcessorArray.at(nIndex);

	return(pProcessor);
}


IGAProcessor*	CGAProcessors::GetFirst()
{
	IGAProcessor* pProcessor = GetAt(0);
	return(pProcessor);
}

IGAProcessor*	CGAProcessors::GetLast()
{
	UINT32 nCount = GetCount();
	if(nCount < 1)
		return(NULL);

	IGAProcessor* pProcessor = GetAt(nCount-1);
	return(pProcessor);
}

// find by name
IGAProcessor* CGAProcessors::Find(ATOM_CHAR* pszName)
{

	IGAProcessor* pProcessor= NULL;
	GAProcessorArrayIter iter;
	for(iter = m_ProcessorArray.begin(); iter != m_ProcessorArray.end(); iter++) {
	
		pProcessor = *iter;
		if(pProcessor->EqualsName(pszName,0)==0)
				return(pProcessor);

	}

	return(NULL);
}

// get index in array searching by name by name
// return INT_MAX on error
UINT32 CGAProcessors::GetIndex(ATOM_CHAR* pszName)
{
	UINT32 nCount = GetCount();
	IGAProcessor* pProcessor= NULL;
	GAProcessorArrayIter iter;
	for(UINT32 i=0; i != nCount; i++) {
	
		pProcessor = m_ProcessorArray.at(i);
		if(pProcessor->EqualsName(pszName,0)==0)
				return(i);

	}

	return(INT_MAX);
}


void CGAProcessors::Trigger()
{
	UINT32 nCount = GetCount();
	for(UINT32 i=0;i <nCount;i++) {
		IGAProcessor* pProcessor = GetAt(i); 
		pProcessor->Trigger();
	}

}


void CGAProcessors::Update()
{
	UINT32 nCount = GetCount();
	for(UINT32 i=0;i <nCount;i++) {
		IGAProcessor* pProcessor = GetAt(i); 
		pProcessor->Update();
	}

}

UINT32 CGAProcessors::Process(IObject* pob)
{
	UINT32 nCount = GetCount();
	for(UINT32 i=0;i <nCount;i++) {
		IGAProcessor* pProcessor = GetAt(i); 
		pProcessor->Process(pob);
	}

	return(0);
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
