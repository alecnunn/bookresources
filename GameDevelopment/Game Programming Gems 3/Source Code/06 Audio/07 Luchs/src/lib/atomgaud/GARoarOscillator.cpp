// GARoarOscillator Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/atomos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of the Atomos Application Base
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GARoarOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGARoarOscillator() { return( new CGARoarOscillator); }
const CClass CGARoarOscillator::classCGARoarOscillator(CID_GARoarOscillator, CID_GAOscillator, "CGARoarOscillator", CreateCGARoarOscillator);
const IClass*	CGARoarOscillator::GetClass() { return((const IClass*)&classCGARoarOscillator); }


// CTOR
CGARoarOscillator::CGARoarOscillator()
{
	SetName("Roar Oscillator");

}

// DTOR
CGARoarOscillator::~CGARoarOscillator()
{
	Terminate();

}

// explicit terminate baseclass
void CGARoarOscillator::Terminate()
{

	CGAOscillator::Terminate();
}

UINT32 CGARoarOscillator::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfAmplitude = GetSignalData(Amplitude);

	FLOAT32* pfPhase = GetSignalData(Phase);
	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32 fAmount = 1.0f;


	for( UINT32 i=0; i< nFrames; i++ )
	{
		// unipolar amplitude modulation
		if(pfAmplitude != NULL) {
			
			fAmplitude = pfAmplitude[i];

		}

		// bipolar amplitude modulation
		if(pfAM != NULL) {
			
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

			// bipolar frequency modulation
		// 0.1 == 1 octave
			if(pfFM  != NULL) {
				
				FLOAT fMod = pfFM[i]; 

				if(fMod > 0.0f)
					fAmount = 1.0f * (1.0f + (fMod *  (m_fFMAttenuation * 10.0f)));
				else if(fMod < 0.0f)
					fAmount = 1.0f  / (1.0f - (fMod * (m_fFMAttenuation * 10.0f)));
				else
					fAmount = 1.0f;

				fFreq = m_fBaseFreq * fAmount;

			}

		static FLOAT32 fPhase2 = m_fPhase;

 		fPhaseIncr = fFreq / fSR;
 		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.17f;
		FLOAT32 fRand = m_Random.ExponentialU();
	
		FLOAT32 fPhaseMod = fr * fPhaseIncr * 0.02f;
		fr = (fRand * fDamp) + (fr * (1.0f- fDamp));

		FLOAT32 f1 = (FLOAT32)sinf(m_fPhase * ATOM_2PI);
		FLOAT32 f2 = (FLOAT32)cosf(fPhase2 * ATOM_2PI*2 );
		m_fSample = (f1 * 0.66f) + (f2 *0.33f);
		m_fSample *= fAmplitude;
		pfOut[i] = m_fSample; 


		m_fPhase += fPhaseIncr ;

		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		fPhase2 = m_fPhase + 0.25f;

		if(m_fPhase < 0.25f)
			fPhase2 *= fPhaseMod;

		while( fPhase2 >= 1.0f ) 
			fPhase2 -= 1.0f;
		
	



	}


	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------


/*
		m_fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;
		m_fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;
		pfOut[i] = m_fSample; 

 		fPhaseIncr = fFreq / fSR;
 		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.337f;
		FLOAT32 fRand = m_Random.ExponentialU();
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
		FLOAT32 fPhaseMod = sinf(fr*ATOM_2PI);
		fPhaseIncr += (fPhaseIncr * fPhaseMod);
		
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

			*/


			/*
chaotic gargle

  		m_fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;
		pfOut[i] = m_fSample; 

 		fPhaseIncr = fFreq / fSR;
 		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.07f * m_fPhase;
		FLOAT32 fRand = m_Random.ExponentialU();
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
		FLOAT32 fPhaseMod = sinf(fr*ATOM_2PI);

		fPhaseIncr += (fPhaseIncr * fPhaseMod*4);
		
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */

  /*

  NLBubble
  		m_fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;
		pfOut[i] = m_fSample; 

 		fPhaseIncr = fFreq / fSR;
 		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.66f * m_fPhase;
		FLOAT32 fRand = m_Random.ExponentialU();
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
		FLOAT32 fPhaseMod = sinf(fr*ATOM_2PI);

		fPhaseIncr += (fPhaseIncr * fPhaseMod*4);
		
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

*/

/*

  
 		fPhaseIncr = fFreq / fSR;
 		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.66f + fPhaseIncr * 0.33f;
		FLOAT32 fRand = m_Random.ExponentialU();
		fr = (fRand * fDamp) + (fr * (1.0f- fDamp));
	
		FLOAT32 fPhaseMod = fPhaseIncr * fr;

		m_fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;
		pfOut[i] = m_fSample; 

		m_fPhase +=fPhaseIncr ;

		if(m_fPhase < 0.5f)
			m_fPhase += (m_fPhase * fPhaseMod * 4);

		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */


  /*

  
		m_fPhase +=fPhaseIncr ;

		if(fPhase2 > 0.0f && fPhase2 < 0.5f) {
			m_fPhase *= (m_fPhase + (fPhase2*fPhaseMod) )* 0.5f;
		}
		else
			m_fPhase += ( fPhase2 * fPhaseMod);


		while( m_fPhase < 0.0f ) 
			m_fPhase += 1.0f;

		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		if(m_fPhase < 0.5f)
			fPhase2 += pow(2.0f,fPhaseIncr ); // (m_fPhase * fPhaseMod );
		else if(m_fPhase < 1.0f)
			fPhase2 += pow(2.0f,-fPhaseIncr );
		else
			fPhase2 = 0.0f; // m_fPhase;

		while( fPhase2 < 0.0f ) 
			fPhase2 += 1.0f;

		while( fPhase2 >= 1.0f ) 
			fPhase2 -= 1.0f;

  */

/*

  
		m_fPhase +=fPhaseIncr ;

		m_fPhase += ( fPhase2 * fPhaseMod);


		while( m_fPhase < 0.0f ) 
			m_fPhase += 1.0f;

		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		if(m_fPhase < 0.5f)
			fPhase2 += pow(2.0f,fPhaseIncr );
		else if(m_fPhase < 1.0f)
			fPhase2 *= exp(fPhaseIncr );
		else
			fPhase2 = 0.0f; // m_fPhase;

		while( fPhase2 < 0.0f ) 
			fPhase2 += 1.0f;

		while( fPhase2 >= 1.0f ) 
			fPhase2 -= 1.0f;

  */

/*

  ringing oscilator

		m_fPhase +=fPhaseIncr ;

		m_fPhase += ( fPhase2 * fPhaseMod);


		while( m_fPhase < 0.0f ) 
			m_fPhase += 1.0f;

		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		if(m_fPhase < 0.1f)
	//		fPhase2 *= pow(2.0f,fPhaseIncr );
			fPhase2 *= pow(fr,fPhaseIncr );
		else if(m_fPhase < 1.0f)
			fPhase2 += sinh( fPhaseIncr  * ATOM_2PI);

		while( fPhase2 < 0.0f ) 
			fPhase2 += 1.0f;

		while( fPhase2 >= 1.0f ) 
			fPhase2 -= 1.0f;

  */