// GAImpulseGenerator Implementation
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
#include "GAImpulseGenerator.h"



#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAImpulseGenerator() { return( new CGAImpulseGenerator); }
const CClass CGAImpulseGenerator::classCGAImpulseGenerator(CID_GAImpulseGenerator, CID_GAOscillator, "CGAImpulseGenerator", CreateCGAImpulseGenerator);
const IClass*	CGAImpulseGenerator::GetClass() { return((const IClass*)&classCGAImpulseGenerator); }


// CTOR
CGAImpulseGenerator::CGAImpulseGenerator()
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	m_fBaseFreq = 1.0f;

}

// DTOR
CGAImpulseGenerator::~CGAImpulseGenerator()
{

}

// explicit terminate baseclass
void CGAImpulseGenerator::Terminate()
{

	CGAGenerator::Terminate();
}

// generate a single impulse every nth sample
UINT32 CGAImpulseGenerator::Process(IObject* pob)
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);

	FLOAT32 fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fFreq = m_fBaseFreq;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		// bipolar frequency modulation
		// 0.1 == 1 octave
			if(pfFM  != NULL) {
				
				FLOAT fMod = pfFM[i]; 
				FLOAT32 fAmount = 0.0f;

				if(fMod > 0.0f)
					fAmount = 1.0f * (1.0f + (fMod *  (m_fFMAttenuation * 10.0f)));
				else if(fMod < 0.0f)
					fAmount = 1.0f  / (1.0f - (fMod * (m_fFMAttenuation * 10.0f)));
				else
					fAmount = 1.0f;

				fFreq = m_fBaseFreq * fAmount;

			}

				fPhaseIncr = fFreq / fSR;


		pfOut[i] = 0.0f;	
		FLOAT32 fPos = m_fPhase * fSR; // (FLOAT32)nSamplesPerCycle; 
		UINT32 nSamplePos = (UINT32)fPos;// truncating

		if(m_fPhase < fPhaseIncr ) 
			pfOut[i] = 0.99f;
		

	
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;


	}


	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
 