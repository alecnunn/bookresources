// GADevice Implementation
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
#include "GADevice.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGADevice() { return( new CGADevice); }
const CClass CGADevice::classCGADevice(CID_GADevice, CID_GAObject, "CGADevice", CreateCGADevice);
const IClass*	CGADevice::GetClass() { return((const IClass*)&classCGADevice); }


// CTOR
CGADevice::CGADevice()
{

}

// DTOR
CGADevice::~CGADevice()
{

}

void CGADevice::Terminate()
{
	CGAObject::Terminate();
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
