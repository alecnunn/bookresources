// GADump Implementation
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
#include "GADump.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGADump() { return( new CGADump); }
const CClass CGADump::classCGADump(CID_GADump, CID_GAObject, "CGADump", CreateCGADump);
const IClass*	CGADump::GetClass() { return((const IClass*)&classCGADump); }


// CTOR
CGADump::CGADump()
{

}

// DTOR
CGADump::~CGADump()
{

}

// explicit terminate baseclass
void CGADump::Terminate()
{

	CGAObject::Terminate();
}

void CGADump::Write(ATOM_CHAR* psz)
{
#ifdef _DEBUG
	::OutputDebugString(psz);
	::OutputDebugString(_T("\r\n"));
#endif
}

void CGADump::Write(FLOAT32 fValue)
{

#ifdef _DEBUG
	_stprintf(m_szBuffer, _T("%.3f\r\n"), fValue);
	::OutputDebugString(m_szBuffer);
#endif
}

void CGADump::Write(UINT32 nValue)
{

#ifdef _DEBUG
	_stprintf(m_szBuffer, _T("%5d\r\n"), nValue);
	::OutputDebugString(m_szBuffer);
#endif
}

void CGADump::Write(INT32 nValue)
{

#ifdef _DEBUG
	_stprintf(m_szBuffer, _T("%5d\r\n"), nValue);
	::OutputDebugString(m_szBuffer);
#endif
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
