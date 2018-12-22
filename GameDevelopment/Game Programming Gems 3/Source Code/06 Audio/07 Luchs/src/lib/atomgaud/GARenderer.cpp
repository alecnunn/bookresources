// GARenderer Implementation
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
#include "GARenderer.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGARenderer() { return( new CGARenderer); }
const CClass CGARenderer::classCGARenderer(CID_GARenderer, CID_GAProcessor, "CGARenderer", CreateCGARenderer);
const IClass*	CGARenderer::GetClass() { return((const IClass*)&classCGARenderer); }


// CTOR
CGARenderer::CGARenderer()
{

}

// DTOR
CGARenderer::~CGARenderer()
{

}

// explicit terminate baseclass
void CGARenderer::Terminate()
{

	CGAProcessor::Terminate();
}

// return 0 on success
UINT32 CGARenderer::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);


	for(UINT32 i=0;i < nFrames;i++) {

	


	}

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
