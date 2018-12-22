// GASawtoothOscDSF Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1998-08-02	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GASawtoothOscDSF.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASawtoothOscDSF() { return( new CGASawtoothOscDSF); }
const CClass CGASawtoothOscDSF::classCGASawtoothOscDSF(CID_GASawtoothOscDSF, CID_GAOscillator, "CGASawtoothOscDSF", CreateCGASawtoothOscDSF);
const IClass*	CGASawtoothOscDSF::GetClass() { return((const IClass*)&classCGASawtoothOscDSF); }

// CTOR
CGASawtoothOscDSF::CGASawtoothOscDSF()
{
	m_fBaseFreq = 50.0f;
	m_nHarmonics = 50;
	m_fCutoff = 1.0f;

	AppendPin(Cutoff); // cutoff modulation
	AppendPin(Harmonics); // count of harmonics

}

// DTOR
CGASawtoothOscDSF::~CGASawtoothOscDSF()
{

}

// explicit terminate baseclass
void CGASawtoothOscDSF::Terminate()
{

	CGAOscillator::Terminate();
}

// return 0 on success
// generate a pulse wave form by specified count of harmonics
UINT32 CGASawtoothOscDSF::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fNyquist = fSR * 0.5f;
	FLOAT fPhaseIncr = m_fBaseFreq / fSR;
	FLOAT32 fCutoffAdjust = 0.93f; // for stability

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfCM = GetSignalData(Cutoff); // cutoff modulation
	FLOAT32 fCutoff = m_fCutoff * fCutoffAdjust;

	FLOAT32* pfHarm = GetSignalData(Harmonics); // harmonic count modulation
	FLOAT32* pfFM = GetSignalData(FM); // frequency modulation
	FLOAT32* pfFMU = GetSignalData(Frequency);

	FLOAT32 fFreq = m_fBaseFreq;

	FLOAT32 fDamping = 1.0f - fCutoff;
	FLOAT32 fh1 = pow(fCutoff ,(m_nHarmonics+1));

	FLOAT32 fs1 = fDamping / (1.0f - fh1); 
	FLOAT32 fs2 = fDamping / (1.0f / fh1);

	UINT32 nHarmonics = m_nHarmonics;

	FLOAT32 fAmount = 1.0f;

	for( UINT32 i=0; i< nFrames; i++ )
	{

		// harmonic count modulation
		if(pfHarm != NULL) { 
			
			// mod range ( -1 to +1) -> 0 to 1
			FLOAT32 fMod = pfCM[i];
			fMod += 1.0f;
			fMod *= 0.5f;

			nHarmonics = fMod * 200;


			FLOAT32 fDamping = 1.0f - fCutoff;
			FLOAT32 fh1 = pow(fCutoff ,(nHarmonics+1));

			FLOAT32 fs1 = fDamping / (1.0f - fh1); 
			FLOAT32 fs2 = fDamping / (1.0f / fh1);

		}

		// internal cutoff modulation
		if(pfCM != NULL) { 
			
			// mod range ( -1 to +1) -> 0 to 1
			FLOAT32 fMod = pfCM[i];
			fMod += 1.0f;
			fMod *= 0.5f;

			fCutoff = fMod * fCutoffAdjust;
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

		if(fFreq < 0.001f)
			fFreq = 0.001f;

		fPhaseIncr = fFreq / fSR;


		FLOAT32 fPiPhase = (ATOM_2PI * m_fPhase);
		FLOAT32 ffcos = fCutoff * cos(fPiPhase);
		FLOAT32 ffsin = fCutoff * sin(fPiPhase);
		FLOAT32 fpiph2 = fPiPhase * (nHarmonics+2);

		m_fSample = (1.0f - ffcos) * (fs1 * cos(fPiPhase));
		m_fSample -= fs2 * cos(fpiph2);
		m_fSample -= ffsin * (fs2 * sin(fpiph2));
		m_fSample -= fs1 * sin(fPiPhase);
		m_fSample /= (1.0f- (2.0f*fCutoff*cos(fPiPhase)) + (fCutoff*fCutoff));

		pfOut[i] = m_fSample;

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

