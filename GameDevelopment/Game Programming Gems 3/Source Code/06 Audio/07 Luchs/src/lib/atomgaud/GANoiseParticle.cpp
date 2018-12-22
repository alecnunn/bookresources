// GANoiseParticle Implementation
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
#include "GANoiseParticle.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGANoiseParticle() { return( new CGANoiseParticle); }
const CClass CGANoiseParticle::classCGANoiseParticle(CID_GANoiseParticle, CID_GANoiseGenerator, "CGANoiseParticle", CreateCGANoiseParticle);
const IClass*	CGANoiseParticle::GetClass() { return((const IClass*)&classCGANoiseParticle); }


// CTOR
CGANoiseParticle::CGANoiseParticle()
{
	SetName("Noise Particle");
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	m_fDuration = 1000.0f; // duration in millisec
	m_nEnvelope = 0; // type of envelope

}

// DTOR
CGANoiseParticle::~CGANoiseParticle()
{

}

// explicit terminate baseclass
void CGANoiseParticle::Terminate()
{

	CGANoiseGenerator::Terminate();
}

// return 0 on success
// generate a single impulse every nth sample
UINT32 CGANoiseParticle::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);

	for( UINT32 i=0; i< nFrames; i++ )
	{


		
	}


	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
