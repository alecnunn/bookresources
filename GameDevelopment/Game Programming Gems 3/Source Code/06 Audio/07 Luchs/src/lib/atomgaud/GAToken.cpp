// GAToken Implementation
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
#include "GAToken.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAToken() { return( new CGAToken); }
const CClass CGAToken::classCGAToken(CID_GAToken, CID_GAObject, "CGAToken", CreateCGAToken);
const IClass*	CGAToken::GetClass() { return((const IClass*)&classCGAToken); }


// CTOR
CGAToken::CGAToken()
{
	m_nPos=0;
	m_nLine=0;

}

CGAToken::CGAToken(CHAR8 ch, UINT32 nPos, UINT32 nLine)
{
	CHAR8 szBuff[2];
	szBuff[0] = ch;
	szBuff[1]=0;
	SetName(szBuff);

	m_nPos=nPos;
	m_nLine=nLine;

}

CGAToken::CGAToken(CHAR8* psz, UINT32 nPos, UINT32 nLine)
{
	SetName(psz);
	m_nPos=nPos;
	m_nLine=nLine;
}



// DTOR
CGAToken::~CGAToken()
{
	Terminate();
}

// explicit terminate baseclass
void CGAToken::Terminate()
{

	CGAObject::Terminate();
}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
