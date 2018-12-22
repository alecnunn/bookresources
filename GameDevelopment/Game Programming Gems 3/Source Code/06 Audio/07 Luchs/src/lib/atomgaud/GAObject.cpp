// GAObject Implementation
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
#include "GAObject.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAObject() { return( new CGAObject); }
const CClass CGAObject::classCGAObject(CID_GAObject, CID_NamedObject, "CGAObject", CreateCGAObject);
const IClass*	CGAObject::GetClass() { return((const IClass*)&classCGAObject); }


// CTOR
CGAObject::CGAObject()
{

}

// DTOR
CGAObject::~CGAObject()
{
	Terminate();
}

void CGAObject::Terminate()
{
	CNamedObject::Terminate();
}

/*
// implement in derived class to set parameters by name
BOOL CGAObject::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = FALSE;

	if(strcmp(pszName, "Name") == 0) {
		SetName(pszValue);
		b = TRUE;
	}

	return(b);
}
*/



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
