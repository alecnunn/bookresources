// GASineXOscillator Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Random modulated sine oscillator
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
#include "GASineXOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASineXOscillator() { return( new CGASineXOscillator); }
const CClass CGASineXOscillator::classCGASineXOscillator(CID_GASineXOscillator, CID_GAOscillator, "CGASineXOscillator", CreateCGASineXOscillator);
const IClass*	CGASineXOscillator::GetClass() { return((const IClass*)&classCGASineXOscillator); }


// CTOR
CGASineXOscillator::CGASineXOscillator()
{
	SetName("SineX Oscillator");

}

// DTOR
CGASineXOscillator::~CGASineXOscillator()
{
	Terminate();

}

// explicit terminate baseclass
void CGASineXOscillator::Terminate()
{

	CGAOscillator::Terminate();
}


UINT32 CGASineXOscillator::Process(IObject* pob)
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
				fPhaseIncr = fFreq / fSR;

			}

/*
			
		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;


  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.8f;
		FLOAT32 fRand = m_Random.ExponentialU();
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
		FLOAT32 fPM = 0.07f * fPhaseIncr;
		FLOAT32 fPhaseMod = sinf(m_fPhase*ATOM_2PI) * fPM;
		fPhaseIncr += fPhaseMod;

		pfOut[i] = fSample; 
		m_fSample = pfOut[i] ;

		
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;
			*/


		m_fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		pfOut[i] = m_fSample; 
	
		FLOAT32 fd = 0.0f;
  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.01f + (fPhaseIncr * fr * 4);
		if(fDamp < 0.0f)
			fDamp = 0.0f;
		else if(fDamp > 1.0f)
			fDamp = 1.0f;

		FLOAT32 fRand = m_Random.ExponentialB();
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
		fd = fPhaseIncr * fRand * 8;
		m_fPhase +=( fPhaseIncr + fd);
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;
	
	}


	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------

/*

  
		static FLOAT32 fr = 0.0f;
		FLOAT32 fRand = m_Random.UniformU() * 0.1f + fr *0.9f;
		fr = fRand;
		FLOAT32 fp = sinf(fRand*ATOM_2PI) * 0.001f;
		fPhaseIncr += fp;

		FLOAT32 fa = 0.0f; // (FLOAT32)sin((double)m_fPhase * ATOM_2PI) * fAmplitude;
		FLOAT32 fb = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;	

	//	m_fSample = (fa * fRand) + (fb * (1.0f - fRand));
		m_fSample = (fa+fb); //  *0.5f;

		pfOut[i] = m_fSample;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */

  /*

  		static FLOAT32 fr = 0.0f;
		FLOAT32 fRand = m_Random.UniformU() * 0.001f + fr *0.999f;
		fr = fRand;
		FLOAT32 fp = sinf(fRand*ATOM_2PI) * 0.0005f;
		fPhaseIncr += fp;

		FLOAT32 fa = 0.0f; // (FLOAT32)sin((double)m_fPhase * ATOM_2PI) * fAmplitude;
		FLOAT32 fb = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;	

	//	m_fSample = (fa * fRand) + (fb * (1.0f - fRand));
		m_fSample = (fa+fb); //  *0.5f;

		pfOut[i] = m_fSample;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */


  /*

  		static FLOAT32 fr = 0.0f;
		FLOAT32 fRand = m_Random.UniformU() * 0.001f + fr *0.999f;
		fr = fRand;
		FLOAT32 fp = sinf(fRand*ATOM_2PI) * (0.0003f / fPhaseIncr);
		fPhaseIncr += fp;

		FLOAT32 fa = 0.0f; // (FLOAT32)sin((double)m_fPhase * ATOM_2PI) * fAmplitude;
		FLOAT32 fb = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;	

	//	m_fSample = (fa * fRand) + (fb * (1.0f - fRand));
		FLOAT32 fSample = (fa+fb); //  *0.5f;

		pfOut[i] = (fSample * 0.01f) + (m_fSample * 0.99f);
		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */


  /*

  		static UINT32 nPeriod = 441;

		static FLOAT32 fr = 0.0f;
		FLOAT32 fRand = m_Random.UniformU() * 0.0005f + fr *0.9995f;
		fr = fRand;
		
		
		if(nPeriod % 1 == 0)
			m_fSample *= sinf( m_fPhase * ATOM_PI);



		FLOAT32 fp = sinf(fRand*ATOM_2PI) * (0.0012f / fPhaseIncr);
		fPhaseIncr += fp;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		pfOut[i] = (fSample * 0.01f) + (m_fSample * 0.99f);
		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		nPeriod++;

  */

/*

  
		static UINT32 nPeriod = 441;

		static FLOAT32 fr = 0.0f;
		FLOAT32 fRand = m_Random.UniformU() * 0.0003f + fr *0.9997f;
		fr = fRand;
		

		FLOAT32 fp = sinf(fRand*ATOM_2PI) * (0.0003f / fPhaseIncr);
		fPhaseIncr += fp;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		if(nPeriod % 1 == 0)
			pfOut[i] = (fSample * 0.01f) + (m_fSample * 0.99f);
		else
			pfOut[i]  = fSample;

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		nPeriod++;

  */

/*

  		static UINT32 nPeriod = 441;

		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.99f;
		FLOAT32 fRand = m_Random.ExponentialU(0.5f) * (1.0f - fDamp) + (fr *fDamp);
		fr = fRand;
		

		FLOAT32 fp = sinf(fRand*ATOM_2PI) * (0.0003f / fPhaseIncr);
		fPhaseIncr += fp;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		if(nPeriod % 1 == 0)
			pfOut[i] = (fSample * 0.01f) + (m_fSample * 0.99f);
		else
			pfOut[i]  = fSample;

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		nPeriod++;

  */


/*

  
		static UINT32 nPeriod = 441;

		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.99f;
		FLOAT32 fRand = m_Random.ExponentialU(0.5f) * (1.0f - fDamp) + (fr *fDamp);
		fr = fRand;
		

		FLOAT32 fp = sinf(fRand*ATOM_2PI) * (0.0003f / fPhaseIncr);
		fPhaseIncr += fp;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		int np = nPeriod * m_fPhase;
		if(np % 1 == 0)
			pfOut[i]  = fSample;
		else
			pfOut[i] = (fSample * 0.01f) + (m_fSample * 0.99f);

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		nPeriod++;

  */

/*

  		static UINT32 nPeriod = 441;

		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.99f;
		FLOAT32 fRand = m_Random.ExponentialU(0.5f) * (1.0f - fDamp) + (fr *fDamp);
		fr = fRand * m_fPhase;
		

		FLOAT32 fp = sinf(fRand*ATOM_2PI) * (0.0003f / fPhaseIncr);
		fPhaseIncr += fp;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		int np = nPeriod * m_fPhase;
		if(np % 1 == 0)
			pfOut[i]  = fSample;
		else
			pfOut[i] = (fSample * 0.01f) + (m_fSample * 0.99f);

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

		nPeriod++;

  */

/*

		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.5f;
		FLOAT32 fRand = m_Random.ExponentialU(fPhaseIncr) * (1.0f - fDamp) + (fr *fDamp);
		fr = fRand * m_fPhase;
		

		FLOAT32 fPhaseMod = sinf(fRand*ATOM_2PI) * m_fPhase * 0.003f;
		fPhaseIncr += fPhaseMod;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		pfOut[i] = (fSample * 0.1f) + (m_fSample * 0.9f);

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */

/*
sub

  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.5f;
		FLOAT32 fRand = m_Random.ExponentialU(fPhaseIncr) * (1.0f - fDamp) + (fr *fDamp);
		fr = fRand ;
		

		FLOAT32 fPhaseMod = sinf(fRand*ATOM_2PI) * m_fPhase * 0.003f;
		fPhaseIncr += fPhaseMod;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		pfOut[i] = (fSample * 0.1f) + (m_fSample * 0.9f);

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */


/*
boiler

  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.99f;
		FLOAT32 fRand = (m_Random.ExponentialU(fPhaseIncr) * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
		

		FLOAT32 fPhaseMod = sinf(fRand*ATOM_2PI) * m_fPhase * 0.003f;
		fPhaseIncr += fPhaseMod;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		pfOut[i] = (fSample * 0.33f) + (m_fSample * 0.66);

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */








/*

  
  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.8f + fabs(0.2f * m_fSample);
		FLOAT32 fRand = m_Random.ExponentialU(fPhaseIncr);
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		
		FLOAT32 fPM = 0.001f;
		FLOAT32 fPhaseMod = sinf(fRand*ATOM_2PI) * fPM;
		fPhaseIncr += fPhaseMod;

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;

		pfOut[i] = fSample; // (fSample * 0.003f) + (m_fSample * 0.997);

		m_fSample = pfOut[i] ;

		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;
	
		fr = fRand ;

  */

/*

  engine roar

		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;


  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.8f;
		FLOAT32 fRand = m_Random.ExponentialU(fPhaseIncr);
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
		FLOAT32 fPM = 0.07f * fPhaseIncr;
		FLOAT32 fPhaseMod = sinf(m_fPhase*ATOM_2PI) * fPM;
		fPhaseIncr += fPhaseMod;

		pfOut[i] = fSample; 
		m_fSample = pfOut[i] ;

		
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */

/*
white mod
  
		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;


  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.8f;
		FLOAT32 fRand = m_Random.ExponentialU(fPhaseIncr);
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
	//	FLOAT32 fPM = ;
		FLOAT32 fPhaseMod = 1.0f / (fRand + 0.01f);
		fPhaseIncr += (fPhaseMod * m_fPhase);
		while( fPhaseIncr >= 1.0f )
			fPhaseIncr -= 1.0f;

		pfOut[i] = fSample; 
		m_fSample = pfOut[i] ;

		
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;

  */


/*
white mod 2 

  
		FLOAT32 fSample = (FLOAT32)sinf(m_fPhase * ATOM_2PI) * fAmplitude;


  		static FLOAT32 fr = 0.0f;
		FLOAT32 fDamp = 0.01f;
		FLOAT32 fRand = m_Random.ExponentialU(fPhaseIncr);
		fRand = (fRand * fDamp) + (fr * (1.0f- fDamp));
		fr = fRand ;
	
	//	FLOAT32 fPM = ;
		FLOAT32 fPhaseMod = 1.0f / (fRand + 0.1f);
		fPhaseIncr += (fPhaseMod * m_fPhase);
		while( fPhaseIncr >= 1.0f )
			fPhaseIncr -= 1.0f;

		pfOut[i] = fSample; 
		m_fSample = pfOut[i] ;

		
		m_fPhase +=fPhaseIncr ;
		while( m_fPhase >= 1.0f )
			m_fPhase -= 1.0f;


			*/

