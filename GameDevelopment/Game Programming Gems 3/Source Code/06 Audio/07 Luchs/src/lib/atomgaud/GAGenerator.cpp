// GAGenerator Implementation
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
#include "GAGenerator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAGenerator() { return( new CGAGenerator); }
const CClass CGAGenerator::classCGAGenerator(CID_GAGenerator, CID_GAProcessor, "CGAGenerator", CreateCGAGenerator);
const IClass*	CGAGenerator::GetClass() { return((const IClass*)&classCGAGenerator); }


// CTOR
CGAGenerator::CGAGenerator()
{
	SetName("Generator");

	m_fAmplitude = 1.0f;
	m_fAMAttenuation = 1.0f;

	IGASignal* pSampleOut = g_GASystem.CreateSignal();

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(AM);
	

}

// DTOR
CGAGenerator::~CGAGenerator()
{
	Terminate();

}

void CGAGenerator::Terminate()
{
	// @removed: now allocated and released by GASystem
	// ATOM_FREE(m_pfOutput);

	
	CGAProcessor::Terminate();

}


void CGAGenerator::SetAmplitude(FLOAT32 fAmplitude)
{
	m_fAmplitude = fAmplitude; 
	Update();
}

void CGAGenerator::SetAMAttenuation(FLOAT32 fAttenuation)
{
	m_fAMAttenuation = fAttenuation; 
	Update();
}




BOOL CGAGenerator::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Amplitude") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetAmplitude(fValue);
		b = TRUE;
	}

	return(b);
}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
