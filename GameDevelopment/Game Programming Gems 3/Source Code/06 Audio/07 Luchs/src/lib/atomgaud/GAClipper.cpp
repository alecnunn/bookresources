// GAClipper Implementation
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
#include "GAClipper.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAClipper() { return( new CGAClipper); }
const CClass CGAClipper::classCGAClipper(CID_GAClipper, CID_GAProcessor, "CGAClipper", CreateCGAClipper);
const IClass*	CGAClipper::GetClass() { return((const IClass*)&classCGAClipper); }


// CTOR
CGAClipper::CGAClipper()
{
	SetName("Clipper");

	AppendPin(SampleOut); // inplace
	AppendPin(SampleIn);
	AppendPin(Clipping);		// clip level
	m_fCMAttenuation= 1.0f;	// amount of clip modulation, full mod

	m_fClipLevel = 0.5f;
}

// DTOR
CGAClipper::~CGAClipper()
{
	Terminate();
}

// explicit terminate baseclass
void CGAClipper::Terminate()
{

	CGAProcessor::Terminate();
}

void CGAClipper::SetClipLevel(FLOAT32 fLevel)
{
	m_fClipLevel = fLevel;
}

FLOAT32 CGAClipper::GetClipLevel()
{
	return(	m_fClipLevel );
}

// input 0 to 1
void CGAClipper::SetCMAttenuation(FLOAT32 fAttenuation)
{
	m_fCMAttenuation = fAttenuation; 
}


BOOL CGAClipper::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(pszValue != NULL) {
		if(strcmp(pszName, "Clipping") == 0 ) {
			FLOAT32 fValue = atof(pszValue);
			SetClipLevel(fValue);
			b = TRUE;
		}
		else if(strcmp(pszName, "CMAttenuation") == 0 ) {
			FLOAT32 fValue = atof(pszValue);
			SetCMAttenuation(fValue);
			b = TRUE;
		}
	}

	return(b);
}

UINT32 CGAClipper::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfClipping = GetSignalData(Clipping);

	FLOAT32 fSample = 0.0f;
	FLOAT32 fClipping = m_fClipLevel * 8 * m_fCMAttenuation;
	
	if(pfIn != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{
			// bipolar
			if(pfClipping != NULL) {
				FLOAT32 fMod = pfClipping[i];
				fMod += 1.0f;
				fMod *= 0.5f;
				fClipping = fMod * 8 * m_fCMAttenuation; 
			}

			fSample = pfIn[i];


			m_fSample = fSample * fClipping;
			

			if(m_fSample > 1.0f)
				m_fSample = 1.0f;
			else if(m_fSample < -1.0f)
				m_fSample = -1.0f;

			pfOut[i] = m_fSample;
		}
	}

	return(0);

}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
