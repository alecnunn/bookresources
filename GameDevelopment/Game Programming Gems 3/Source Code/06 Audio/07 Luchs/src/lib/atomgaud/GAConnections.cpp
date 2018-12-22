// GAConnection Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	array for objects of class GAConnection
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
#include "GAConnections.h"


namespace atomos
{
// metaclass implementation
void* __stdcall CreateCGAConnections() { return( new CGAConnections); }
const CClass CGAConnections::classCGAConnections(CID_GAConnections, CID_Object, "CGAConnections", CreateCGAConnections);
const IClass*	CGAConnections::GetClass() { return((const IClass*)&classCGAConnections); }


// CTOR
CGAConnections::CGAConnections()
{

}

// DTOR
CGAConnections::~CGAConnections()
{

}

	
// explicit terminate baseclass
void CGAConnections::Terminate()
{

	CObject::Terminate();
}

UINT32 CGAConnections::GetCount()
{
	return(m_GAConnectionArray.size());
}
	

UINT32 CGAConnections::Append(IGAConnection* pGAConnection)
{
	m_GAConnectionArray.push_back(pGAConnection);
	UINT32 nCount = GetCount();
	return(nCount);
}

// return count of removed objects
UINT32 CGAConnections::RemoveAll()
{
	GAConnectionArrayIter iter;
	IGAConnection* pGAConnection = NULL;
	UINT32 nRemoved=0;
	for(iter = m_GAConnectionArray.begin(); iter != m_GAConnectionArray.end(); iter++) {
			
		pGAConnection = *iter;
		ATOM_DELETE(pGAConnection);
		nRemoved++;
	}

	m_GAConnectionArray.clear();

	return(nRemoved);
}


// return true if we have found and removed this object
BOOL CGAConnections::Remove(IGAConnection* pGAConnection)
{
	GAConnectionArrayIter iter;
	for(iter = m_GAConnectionArray.begin(); iter != m_GAConnectionArray.end(); iter++) {
			
		if( *iter == pGAConnection) {
			m_GAConnectionArray.erase(iter);
			return(TRUE);
		}
	}
	return(FALSE);
}

// save, returns NULL if no GAConnection found at this index
IGAConnection* CGAConnections::GetAt(UINT32 nIndex)
{
	IGAConnection* pGAConnection = NULL;
	UINT32 nCount = GetCount();
	if(nIndex < nCount)
		pGAConnection = m_GAConnectionArray.at(nIndex);

	return(pGAConnection);
}

	
// find by name
IGAConnection* CGAConnections::Find(ATOM_CHAR* pszName)
{

	IGAConnection* pGAConnection = NULL;
	GAConnectionArrayIter iter;
	for(iter = m_GAConnectionArray.begin(); iter != m_GAConnectionArray.end(); iter++) {
	
		pGAConnection = *iter;
		if(pGAConnection->EqualsName(pszName,0))
				return(pGAConnection);

	}

	return(NULL);
}





} // namespace atomos
// --------------------------------------------------------------------------------------------------------
