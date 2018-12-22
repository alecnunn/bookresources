// GAGlide Implementation
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
#include "GAGlide.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAGlide() { return( new CGAGlide); }
const CClass CGAGlide::classCGAGlide(CID_GAGlide, CID_GAProcessor, "CGAGlide", CreateCGAGlide);
const IClass*	CGAGlide::GetClass() { return((const IClass*)&classCGAGlide); }


// CTOR
CGAGlide::CGAGlide()
{
	SetName("Glide");
	m_nDuration = 0;
	AppendPin(SampleOut);
	AppendPin(SampleIn);
	IGASignal* pDeltaOut = g_GASystem.CreateSignal();

	AppendPin(DeltaOut, "DeltaOut", pDeltaOut);

	m_fSample = 0.0f;
	m_fGlide = 0.5f; // 0 to 1 
}

// DTOR
CGAGlide::~CGAGlide()
{
	Terminate();
}

// explicit terminate baseclass
void CGAGlide::Terminate()
{

	CGAProcessor::Terminate();
}

void CGAGlide::SetTime(FLOAT32 fTime)
{
	m_fGlide = fTime;
}

BOOL CGAGlide::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(strcmp(pszName, "Time") == 0 && pszValue != NULL) {
		FLOAT32 fValue = atof(pszValue);
		SetTime(fValue);
		b = TRUE;
	}

	return(b);
}

UINT32 CGAGlide::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfDelta = GetSignalData(DeltaOut);

	FLOAT32 fSample = 0.0f;
	FLOAT32 fGlideUp = m_fGlide;
	FLOAT32 fGlideDown = m_fGlide * m_fGlide; // descend faster

	if(pfIn != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{

			FLOAT32 fIn = pfIn[i];
			FLOAT32 fDelta = fIn - m_fSample;
			pfDelta[i] = fDelta;

			if(fDelta > 0.0f) {
				fSample = fIn * (1.0f - fGlideUp);
				fSample += m_fSample * fGlideUp;
			}
			else {
				fSample = fIn * (1.0f - fGlideDown);
				fSample += m_fSample * fGlideDown;


			}

			pfOut[i] = fSample;
			m_fSample = fSample;

		}
	}

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------

/*
		for( UINT32 i=0; i< nFrames; i++ )
		{

			fSample = pfIn[i] * (1.0f - m_fGlide);
			fSample += m_fSample * m_fGlide;

			pfOut[i] = fSample;
			m_fSample = fSample;


			m_nSampleCounter++;

		}
		*/