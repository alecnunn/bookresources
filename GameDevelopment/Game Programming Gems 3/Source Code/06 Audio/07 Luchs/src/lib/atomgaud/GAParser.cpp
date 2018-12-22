// GAParser Implementation
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
#include "GAParser.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAParser() { return( new CGAParser); }
const CClass CGAParser::classCGAParser(CID_GAParser, CID_GAObject, "CGAParser", CreateCGAParser);
const IClass*	CGAParser::GetClass() { return((const IClass*)&classCGAParser); }


// CTOR
CGAParser::CGAParser()
{

}

// DTOR
CGAParser::~CGAParser()
{

}

// explicit terminate baseclass
void CGAParser::Terminate()
{

	CGAObject::Terminate();
}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
