// GAShapeOscillator Implementation
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
#include "GAShapeOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAShapeOscillator() { return( new CGAShapeOscillator); }
const CClass CGAShapeOscillator::classCGAShapeOscillator(CID_GAShapeOscillator, CID_GAOscillator, "CGAShapeOscillator", CreateCGAShapeOscillator);
const IClass*	CGAShapeOscillator::GetClass() { return((const IClass*)&classCGAShapeOscillator); }


// CTOR
CGAShapeOscillator::CGAShapeOscillator()
{
	SetName("Shape Oscillator");
	AppendPin(ControlIn1);

}

// DTOR
CGAShapeOscillator::~CGAShapeOscillator()
{
	Terminate();

}

// explicit terminate baseclass
void CGAShapeOscillator::Terminate()
{

	CGAGenerator::Terminate();
}

// return 0 on success
// if an AM is connected, the internal amplitude is not used
UINT32 CGAShapeOscillator::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfPhase = GetSignalData(Phase);
	FLOAT32* pfControl1 = GetSignalData(ControlIn1);

	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT32 fControl1 = 64.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		if(pfAM != NULL) { // amplitude modulation
			
			// mod range ( -1 to +1) -> 0 to 1
			FLOAT32 fMod = pfAM[i];
			fMod += 1.0f;
			fMod *= 0.5f;

			fAmplitude = fMod;
		}


		if(pfPhase != NULL) {
			
			// mod range ( -1 +1)

			FLOAT32 fMod = pfPhase[i]; 
			m_fPhase +=fMod ;

			if( m_fPhase >= 1.0f )
				m_fPhase -= 1.0f;
			else if( m_fPhase <= 0.0f )
				m_fPhase += 1.0f;

		}

		if(pfFM  != NULL) {
			
			FLOAT32 fFMAtt = m_fFMAttenuation * ATT_MAX;
			FLOAT32 fMod = pfFM[i]; 
			fMod *= (fFMAtt / 2);
			fMod += (fFMAtt / 2);
			fMod = pow(2,fMod);
			fMod /= fFMAtt;


			FLOAT32 fFreq = m_fBaseFreq * fMod; 
			fPhaseIncr = fFreq / fSR;

		}

		if(pfControl1  != NULL) {
			
			// convert mod range ( -1 +1)---> amount ( 1/fAtt --> 1*fAtt )
			FLOAT32 fAtt = 4.0f;
			FLOAT32 fMod = pfControl1[i];
			FLOAT32 fUnip = ( fMod + 1) *0.5f;

			fMod *= (fAtt / 2);
			fMod += (fAtt / 2);
			fMod = pow(2,fMod);
			fMod /= fAtt;

			fControl1 =  16.0f * fMod;
		
		}


		double dbsin = sin((double)m_fPhase * ATOM_2PI);

		pfOut[i] = (FLOAT32)sin((dbsin*dbsin)*fControl1*ATOM_PI) * fAmplitude;	


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
