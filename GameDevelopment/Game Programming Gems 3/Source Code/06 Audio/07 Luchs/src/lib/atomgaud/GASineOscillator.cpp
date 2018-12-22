// GASineOscillator Implementation
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
#include "GASineOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASineOscillator() { return( new CGASineOscillator); }
const CClass CGASineOscillator::classCGASineOscillator(CID_GASineOscillator, CID_GAOscillator, "CGASineOscillator", CreateCGASineOscillator);
const IClass*	CGASineOscillator::GetClass() { return((const IClass*)&classCGASineOscillator); }


// CTOR
CGASineOscillator::CGASineOscillator()
{
	SetName("Sine Oscillator");

}

// DTOR
CGASineOscillator::~CGASineOscillator()
{
	Terminate();
}

void CGASineOscillator::Terminate()
{
	CGAOscillator::Terminate();
}

/* from music-dsp archive


  
  if(phase > 0.5)
    yp = 1.0 - phase;
  else
  {
    if(phase < -0.5)
       yp = -1.0 - phase;
    else
        yp = phase;
  }

  x = yp * PI;
  x2 = x*x;



  //Taylor expansion out to x**9/9! factored  into multiply-adds
  fastsin = x*(x2*(x2*(x2*(x2*(1.0/362880.0)
            - (1.0/5040.0))
            + (1.0/120.0))
            - (1.0/6.0))
            + 1.0);
*/



UINT32 CGASineOscillator::Process(IObject* pob)
{


	UINT32 nFrames =  g_GASystem.GetFrameCount();
	FLOAT32 fSR =  g_GASystem.GetSampleRate();
	FLOAT32 fNyquist = fSR * 0.5f;
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfFM = GetSignalData(FM);
	FLOAT32* pfFMU = GetSignalData(Frequency);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfAMU = GetSignalData(Amplitude);

	FLOAT32* pfPhase = GetSignalData(Phase);
	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fAmplitude = m_fAmplitude;
	FLOAT32 fFreq = m_fBaseFreq;
	FLOAT32 fAmount = 1.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{
		// unipolar amplitude modulation
		if(pfAMU != NULL) {
			
			fAmplitude = pfAMU[i];

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
		// unipolar normalized frequency modulation
		if(pfFMU != NULL) {
			
			FLOAT fMod = pfFMU[i]; 
			if(fMod > 0.0f) {
				fFreq = fMod * fNyquist;
			}
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
				fPhaseIncr = fFreq / fSR;

			}

	//	if(fFreq < 0.001f)
	//		fFreq = 0.001f;

		fPhaseIncr = fFreq / fSR;
	
		FLOAT32 yp = 0.0f;
		FLOAT32 x = 0.0f;
		FLOAT32 x2 = 0.0f;

		  if(m_fPhase > 0.5)
			yp = 1.0f - m_fPhase;
		  else
		  {
			if(m_fPhase < -0.5)
			   yp = -1.0f - m_fPhase;
			else
				yp = m_fPhase;
		  }

		  x = yp * ATOM_PI;
		  x2 = x*x;


		m_fSample = x*(x2*(x2*(x2*(x2*(1.0f/362880.0f)
            - (1.0/5040.0f))
            + (1.0/120.0f))
            - (1.0/6.0f))
            + 1.0);


	//	m_fSample = sinf(m_fPhase * ATOM_2PI) * fAmplitude;
		pfOut[i] = m_fSample * fAmplitude;;	


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
