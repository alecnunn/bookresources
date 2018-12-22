// GASignal Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	array for objects of class GASignal
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
#include "GASignals.h"


namespace atomos
{
// metaclass implementation
void* __stdcall CreateCGASignals() { return( new CGASignals); }
const CClass CGASignals::classCGASignals(CID_GASignals, CID_Object, "CGASignals", CreateCGASignals);
const IClass*	CGASignals::GetClass() { return((const IClass*)&classCGASignals); }


// CTOR
CGASignals::CGASignals()
{

}

// DTOR
CGASignals::~CGASignals()
{
	Terminate();
}

	
// explicit terminate baseclass
void CGASignals::Terminate()
{
	RemoveAll();
	CObject::Terminate();
}

UINT32 CGASignals::GetCount()
{
	return(m_GASignalArray.size());
}
	

UINT32 CGASignals::Append(IGASignal* pGASignal)
{
	m_GASignalArray.push_back(pGASignal);
	UINT32 nCount = GetCount();
	return(nCount);
}

// return count of removed objects
UINT32 CGASignals::RemoveAll()
{
	GASignalArrayIter iter;
	IGASignal* pGASignal = NULL;
	UINT32 nRemoved=0;
	for(iter = m_GASignalArray.begin(); iter != m_GASignalArray.end(); iter++) {
			
		pGASignal = *iter;
		pGASignal->Terminate();
		ATOM_DELETE(pGASignal);
		nRemoved++;
	}

	m_GASignalArray.clear();

	return(nRemoved);
}


// return true if we have found and removed this object
BOOL CGASignals::Remove(IGASignal* pGASignal)
{
	GASignalArrayIter iter;
	for(iter = m_GASignalArray.begin(); iter != m_GASignalArray.end(); iter++) {
			
		if( *iter == pGASignal) {
			m_GASignalArray.erase(iter);
			return(TRUE);
		}
	}
	return(FALSE);
}

// save, returns NULL if no GASignal found at this index
IGASignal* CGASignals::GetAt(UINT32 nIndex)
{
	IGASignal* pGASignal = NULL;
	UINT32 nCount = GetCount();
	if(nIndex < nCount)
		pGASignal = m_GASignalArray.at(nIndex);

	return(pGASignal);
}

	
// find by name
IGASignal* CGASignals::Find(ATOM_CHAR* pszName)
{

	IGASignal* pGASignal = NULL;
	GASignalArrayIter iter;
	for(iter = m_GASignalArray.begin(); iter != m_GASignalArray.end(); iter++) {
	
		pGASignal = *iter;
		if(pGASignal->EqualsName(pszName,0)==0)
				return(pGASignal);

	}

	return(NULL);
}





} // namespace atomos
// --------------------------------------------------------------------------------------------------------
