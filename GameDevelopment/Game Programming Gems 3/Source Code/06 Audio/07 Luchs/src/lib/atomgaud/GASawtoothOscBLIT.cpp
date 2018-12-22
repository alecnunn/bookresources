// GASawtoothOscBLIT Implementation
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
#include "GASawtoothOscBLIT.h"

#define HALF_WRAP ((1 << 30)-1)
#define NEAR_IMPULSE(n, x) ((((x & HALF_WRAP) + n) & HALF_WRAP) < (2*n+1))


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASawtoothOscBLIT() { return( new CGASawtoothOscBLIT); }
const CClass CGASawtoothOscBLIT::classCGASawtoothOscBLIT(CID_GASawtoothOscBLIT, CID_GAOscillator, "CGASawtoothOscBLIT", CreateCGASawtoothOscBLIT);
const IClass*	CGASawtoothOscBLIT::GetClass() { return((const IClass*)&classCGASawtoothOscBLIT); }


// CTOR
CGASawtoothOscBLIT::CGASawtoothOscBLIT()
{
	SetName("Sawtooth Oscillator Blit");
	m_fSample  = -1.0f; 

	m_pSinTable = g_GASystem.GetFunctionTable(IGAFunctionTable::Sin);
	m_pNDivSinTable = g_GASystem.GetFunctionTable(IGAFunctionTable::NDivSin);

}

// DTOR
CGASawtoothOscBLIT::~CGASawtoothOscBLIT()
{
	Terminate();
}

// explicit terminate baseclass
void CGASawtoothOscBLIT::Terminate()
{

	CGAOscillator::Terminate();
}

UINT32 CGASawtoothOscBLIT::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
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
			
			}


		fPhaseIncr = fFreq / fSR;
	
		
		FLOAT32   fHalfIncr  = 0.5f * fPhaseIncr;                
		FLOAT32   fHarm = (0.5f / fPhaseIncr) -1;   
		UINT32    nHarm = (UINT32)fHarm;                     
		FLOAT32   fFracHarm = fHarm - nHarm;           
		UINT32	nHarm1 = (nHarm << 1) + 1;                
		UINT32  nHarm2 = (nHarm << 1) - 1;           
		UINT32  nPhase1  = nHarm1 * m_nPhase;         
		UINT32  nPhase2  = nHarm2 * m_nPhase;

		FLOAT32 fsinPhase1 = m_pSinTable->GetValue(nPhase1);
		FLOAT32 fsinPhase2 = m_pSinTable->GetValue(nPhase2);

		FLOAT32   fHCount   = (1.0 - fFracHarm) * fsinPhase2 + fFracHarm * fsinPhase1;
		FLOAT32   fBlit = 0.0f;

		// 2 p 12 = 4096 = ATOM_POW2 tablesize
		UINT32 nInRange = ATOM_POW2[27];
		UINT32 nNear = 32; 
		UINT32 nHalfPhase = ATOM_POW2[30]-1; 
		UINT32 nBit = m_nPhase & nHalfPhase;

		BOOL bInRange = ((((nBit) + nInRange) & nHalfPhase) < (2*nInRange+1));
		BOOL bNearImpulse = ((((nBit) + nNear) & nHalfPhase) < (2*nNear+1));

		if(bInRange == TRUE)
		{
			if(bNearImpulse == TRUE)
				fBlit = 1.0f / fPhaseIncr; 
			else
				fBlit = fHCount / m_pSinTable->GetValue(m_nPhase); 
		}
		else
		{
			FLOAT32 fInv =  m_pNDivSinTable->GetValue(m_nPhase); 
			fBlit = fHCount * fInv;
		}


		fBlit = 2.0 * fPhaseIncr * (fBlit - 1.0);
		m_fSample = 0.9998*m_fSample + fBlit;
		pfOut[i] = m_fSample;	

		m_nPhase += (int)(4294967296.0*fHalfIncr);

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

