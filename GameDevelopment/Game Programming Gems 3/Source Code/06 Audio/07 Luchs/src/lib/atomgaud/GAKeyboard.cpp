// GAKeyboard Implementation
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
#include "GAKeyboard.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAKeyboard() { return( new CGAKeyboard); }
const CClass CGAKeyboard::classCGAKeyboard(CID_GAKeyboard, CID_GAObject, "CGAKeyboard", CreateCGAKeyboard);
const IClass*	CGAKeyboard::GetClass() { return((const IClass*)&classCGAKeyboard); }


// CTOR
CGAKeyboard::CGAKeyboard()
{

}

// DTOR
CGAKeyboard::~CGAKeyboard()
{
	Terminate();
}

// explicit terminate baseclass
void CGAKeyboard::Terminate()
{

	CGAObject::Terminate();
}

void CGAKeyboard::SetKey(UINT32 nKey, UINT32 nValue)
{
	if(nKey < 256) {
		m_nKey[nKey] = nValue;
		//GADUMP(nKey);

	}

}

	
UINT32 CGAKeyboard::GetKey(UINT32 nKey)
{
	UINT32 nValue=0;
	if(nKey < 256)
		nValue = m_nKey[nKey];

	return(nValue);
}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
