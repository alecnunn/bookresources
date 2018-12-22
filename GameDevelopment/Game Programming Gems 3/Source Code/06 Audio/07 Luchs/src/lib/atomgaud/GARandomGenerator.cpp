// GARandomGenerator Implementation
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
#include "GARandomGenerator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGARandomGenerator() { return( new CGARandomGenerator); }
const CClass CGARandomGenerator::classCGARandomGenerator(CID_GARandomGenerator, CID_GAOscillator, "CGARandomGenerator", CreateCGARandomGenerator);
const IClass*	CGARandomGenerator::GetClass() { return((const IClass*)&classCGARandomGenerator); }


// CTOR
CGARandomGenerator::CGARandomGenerator()
{
	SetName("Random Generator");
	m_fCurrSample = 0.0f;
	m_fPrevSample = 0.0f;

	AppendPin(SampleTime);

}

// DTOR
CGARandomGenerator::~CGARandomGenerator()
{
	Terminate();
}


void CGARandomGenerator::Terminate()
{
	CGAGenerator::Terminate();
}

BOOL CGARandomGenerator::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAGenerator::SetParameter(pszName, pszValue);

/*	if( pszValue != NULL) {
		if(strcmp(pszName, "SampleTime") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetSampleTime(fValue);
			b = TRUE;
	
	}
	*/
	return(b);
}


UINT32 CGARandomGenerator::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfSampleTime = GetSignalData(SampleTime);
	FLOAT32 fAmount = 4.0f;

	FLOAT32 fPhaseIncr = 1.0f / 100.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		if(pfSampleTime != NULL) {

			FLOAT32 fMod = pfSampleTime[i];	//-1 to +1
			fMod *= fAmount;				//-4 to +4
			fMod += fAmount;				//0 to 8

			fMod = pow(2,fMod);

			fPhaseIncr = 1.0f / fMod;
		}


		if(m_fPhase >= 1.0f) {

			m_fCurrSample = m_Random.UniformB();
		//	m_fCurrSample = m_Random.UniformB() * 0.3f + m_fCurrSample * 0.7f;
			m_fPhase = 0.0f;
		}

		pfOut[i] =  (m_fCurrSample * 0.5f);
		pfOut[i] +=  (m_fPrevSample * 0.5f); 
		pfOut[i] *= m_fAmplitude;  


		m_fPhase += fPhaseIncr;
		m_fPrevSample = pfOut[i]; //  * m_fPhaseIncr;
	}

	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
