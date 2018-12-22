// GAConnection Implementation
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
#include "GAConnection.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAConnection() { return( new CGAConnection); }
const CClass CGAConnection::classCGAConnection(CID_GAConnection, CID_GAObject, "CGAConnection", CreateCGAConnection);
const IClass*	CGAConnection::GetClass() { return((const IClass*)&classCGAConnection); }


// CTOR
CGAConnection::CGAConnection()
{

}

// DTOR
CGAConnection::~CGAConnection()
{

}

// explicit terminate baseclass
void CGAConnection::Terminate()
{

	CGAObject::Terminate();
}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
