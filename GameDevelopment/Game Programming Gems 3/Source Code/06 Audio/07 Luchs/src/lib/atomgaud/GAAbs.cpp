// GAAbs Implementation
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
#include "GAAbs.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAAbs() { return( new CGAAbs); }
const CClass CGAAbs::classCGAAbs(CID_GAAbs, CID_GAProcessor, "CGAAbs", CreateCGAAbs);
const IClass*	CGAAbs::GetClass() { return((const IClass*)&classCGAAbs); }


// CTOR
CGAAbs::CGAAbs()
{
	SetName("Abs");
	AppendPin(SampleOut);
	AppendPin(SampleIn);

}

// DTOR
CGAAbs::~CGAAbs()
{

}

void CGAAbs::Terminate()
{
	CGAProcessor::Terminate();
}

// This unit operates inplace, using the same buffer
// for sample input and output
UINT32 CGAAbs::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);

	if(pfIn != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
			if(pfIn[i]  < 0.0f)
				pfIn[i] = -pfIn[i];
		}
	}

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
