// GAOscillator Implementation
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
#include "GAOscillator.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAOscillator() { return( new CGAOscillator); }
const CClass CGAOscillator::classCGAOscillator(CID_GAOscillator, CID_GAGenerator, "CGAOscillator", CreateCGAOscillator);
const IClass*	CGAOscillator::GetClass() { return((const IClass*)&classCGAOscillator); }


// CTOR
CGAOscillator::CGAOscillator()
{

	SetName("GAOscillator");
	m_fBaseFreq = 440.0f;// base frequency
	m_fFMAttenuation = 1.0f;
	
	//	m_fFreq  = m_fBaseFreq;		
	
	m_fPhase = 0.0f;			// phase from 0 to 1
	m_fPhaseOffset = 0.0f;		// the current phase offset
	m_fPMAttenuation = 1.0f;	// allow to shift the phase start
	m_nPhase = 0;

	AppendPin(Frequency);	// unipolar freq pin 0 to 1 -> 0 to nyquist
	AppendPin(FM);
	AppendPin(Phase);

}

	FLOAT32	m_fBaseFreq;		// base frequency 
	FLOAT32	m_fFMAttenuation;	// FM trimmer

//	FLOAT32	m_fFMMod;		// FM modulation input
//	FLOAT32	m_fFreq;		// current calculated frequency


	FLOAT32 m_fPhase;		// current phase from 0 to 1
//	FLOAT32 m_fPhaseIncr;	// calculated phase increment



// DTOR
CGAOscillator::~CGAOscillator()
{
	Terminate();
}

// explicit terminate baseclass
void CGAOscillator::Terminate()
{

	CGAGenerator::Terminate();
}

// set the base frequency
void CGAOscillator::SetFrequency(FLOAT32 fFreq)
{
	m_fBaseFreq = fFreq;
	Update();
}


// input 0 to 1
void CGAOscillator::SetFMAttenuation(FLOAT32 fAttenuation)
{
	m_fFMAttenuation = fAttenuation; 
	Update();
}

// set the phase, which runs from 0 to 1
void CGAOscillator::SetPhase(FLOAT32 fPhase)
{
	m_fPhaseOffset = fPhase;
	Update();
}


void CGAOscillator::Trigger()
{
	Reset(); // reset phase and sample position	
}

void CGAOscillator::Reset()
{
	m_fPhase = m_fPhaseOffset; // start point
}

// input 0 to 1
void CGAOscillator::SetPMAttenuation(FLOAT32 fAttenuation)
{
	m_fPMAttenuation = fAttenuation; 
	Update();
}

UINT32 CGAOscillator::Process(IObject* pob)
{

	// implement in derived class

	return(0);

}


BOOL CGAOscillator::SetParameter(UINT32 idParam, FLOAT32 fValue)
{
	BOOL b= FALSE;

	if(idParam == Frequency) {
		SetFrequency(fValue);
		b = TRUE;
	}

	return(b);
}

BOOL CGAOscillator::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAGenerator::SetParameter(pszName, pszValue);

	if( pszValue != NULL) {
		if(strcmp(pszName, "Frequency") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetFrequency(fValue);
			b = TRUE;
		}
		else if(strcmp(pszName, "FMAttenuation") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetFMAttenuation(fValue);
			b = TRUE;
		}
		else if(strcmp(pszName, "Phase") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetPhase(fValue);
			b = TRUE;
		}
		else if(strcmp(pszName, "PMAttenuation") == 0) {
			FLOAT32 fValue = atof(pszValue);
			SetPMAttenuation(fValue);
			b = TRUE;
		}
	}
	return(b);
}


/*
// calculate the current frequency
void CGAOscillator::UpdateFrequency(FLOAT32* pfFM)
{
	FLOAT32 fMaxMod = 8.0f;
	FLOAT fMod = pfFM[i]; // assume same frame count!
	fMod *= (fMaxMod / 2);
	fMod += (fMaxMod / 2);
	fMod = pow(2,fMod);
	fMod /= fMaxMod;


	m_fFreq = m_fBaseFreq * fMod;  // * m_fFMAtt
	m_fPhaseIncr = m_fFreq / fSR;

}
*/


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
