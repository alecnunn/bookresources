// GAInverter Implementation
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
#include "GAInverter.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAInverter() { return( new CGAInverter); }
const CClass CGAInverter::classCGAInverter(CID_GAInverter, CID_GAProcessor, "CGAInverter", CreateCGAInverter);
const IClass*	CGAInverter::GetClass() { return((const IClass*)&classCGAInverter); }


// CTOR
CGAInverter::CGAInverter()
{
	SetName("Inverter");
	AppendPin(SampleIn);
	AppendPin(SampleOut);

}

// DTOR
CGAInverter::~CGAInverter()
{
	Terminate();
}

// explicit terminate baseclass
void CGAInverter::Terminate()
{

	CGAProcessor::Terminate();
}

UINT32 CGAInverter::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32* pfIn = GetSignalData(SampleOut);
	FLOAT32* pfOut = pfIn;

	if( pfIn != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
			if(m_bBypass == false) {

				m_fSample = -pfIn[i];
				pfOut[i] =m_fSample;
			}
		}
	}

	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
