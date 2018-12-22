// GAPatches Implementation
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
#include "GAPatches.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAPatches() { return( new CGAPatches); }
const CClass CGAPatches::classCGAPatches(CID_GAPatches, CID_GAObject, "CGAPatches", CreateCGAPatches);
const IClass*	CGAPatches::GetClass() { return((const IClass*)&classCGAPatches); }


// CTOR
CGAPatches::CGAPatches()
{
	m_nCurrPatch = (UINT32)-1;

}

// DTOR
CGAPatches::~CGAPatches()
{
	Terminate();
}

void CGAPatches::Terminate()
{
	RemoveAll();
	CGAObject::Terminate();
}


UINT32 CGAPatches::RemoveAll()
{
	UINT32 nCount = GetCount();
	for(UINT32 i=0; i < nCount; i++)  {
		IGAPatch* pPatch = m_PatchArray.at(i);
		pPatch->Terminate();
		delete pPatch;
		pPatch = NULL;
	}

	m_PatchArray.clear();
	return(nCount);
}


UINT32 CGAPatches::GetCount()
{
	return(m_PatchArray.size());
}
	
// return currrent patch = appended patch no
UINT32 CGAPatches::Append(IGAPatch* pPatch)
{
	m_PatchArray.push_back(pPatch);
	UINT32 nCount = GetCount();
	m_nCurrPatch = nCount-1;
	return(m_nCurrPatch);
}


IGAPatch*	CGAPatches::GetAt(UINT32 nIndex)
{
	IGAPatch* pPatch = NULL;
	UINT32 nCount = GetCount();

	if(nIndex < nCount)
		pPatch = m_PatchArray.at(nIndex);

	return(pPatch);
}


// cycles thru patches
IGAPatch*	CGAPatches::GetNext()
{
	IGAPatch* pPatch = GetAt(m_nCurrPatch+1);

	if(pPatch == NULL) { // was last, try the first patch
		pPatch = GetAt(0);

		if(pPatch == NULL)
			m_nCurrPatch = (UINT32)-1; // no patch available
	}
	else {

		m_nCurrPatch++; 
	}

	return(pPatch);
}

// cycles thru patches
IGAPatch*	CGAPatches::GetPrevious()
{
	UINT32 nCount = GetCount();
	if(nCount < 1)
		return(NULL);

	IGAPatch* pPatch = GetAt(m_nCurrPatch-1);

	if(pPatch == NULL) { // was first, try the last patch
		pPatch = GetAt(nCount-1);

		if(pPatch == NULL)
			m_nCurrPatch = (UINT32)-1; // no patch available
	}
	else {

		m_nCurrPatch--; 
	}

	return(pPatch);
}

IGAPatch*	CGAPatches::GetCurrent()
{
	IGAPatch* pPatch = GetAt(m_nCurrPatch);
	return(pPatch);
}


// This will find the patch index from the specified
// object pointer and set the variable m_nCurrPatch.
UINT32 CGAPatches::SetCurrent(IGAPatch* pPatch)
{

	UINT32 nCount = GetCount();
	
	for(UINT32 i=0;i < nCount;i++) {

		if(GetAt(i) == pPatch) {
			m_nCurrPatch = i;
			pPatch->Process(NULL);
			break;
		}
	}
	return(m_nCurrPatch);
}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
