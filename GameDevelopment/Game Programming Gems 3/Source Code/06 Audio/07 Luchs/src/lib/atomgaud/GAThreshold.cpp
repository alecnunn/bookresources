// GAThreshold Implementation
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
#include "GAThreshold.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAThreshold() { return( new CGAThreshold); }
const CClass CGAThreshold::classCGAThreshold(CID_GAThreshold, CID_GAProcessor, "CGAThreshold", CreateCGAThreshold);
const IClass*	CGAThreshold::GetClass() { return((const IClass*)&classCGAThreshold); }


// CTOR
CGAThreshold::CGAThreshold()
{
	SetName("Threshold");
   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut); // NOT inplace
	AppendPin(SampleIn);

	m_fLowerLevel = 0.0f;
	m_fUpperLevel = 0.0f;

}

// DTOR
CGAThreshold::~CGAThreshold()
{
	Terminate();
}

// explicit terminate baseclass
void CGAThreshold::Terminate()
{

	CGAProcessor::Terminate();
}



void CGAThreshold::SetLowerLevel(FLOAT32 fLevel)
{
	m_fLowerLevel = fLevel;			
}


FLOAT32 CGAThreshold::GetLowerLevel()
{
	return(	m_fLowerLevel );
}

void CGAThreshold::SetUpperLevel(FLOAT32 fLevel)
{
	m_fUpperLevel = fLevel;			
}


FLOAT32 CGAThreshold::GetUpperLevel()
{
	return(	m_fUpperLevel );
}


BOOL CGAThreshold::SetParameter(CHAR8* pszName, CHAR8* pszValue)
{
	BOOL b = CGAProcessor::SetParameter(pszName, pszValue);

	if(pszValue != NULL) {
		if(strcmp(pszName, "LowerLevel") == 0 ) {
			FLOAT32 fValue = atof(pszValue);
			SetLowerLevel(fValue);
			b = TRUE;
		}
	else if(strcmp(pszName, "UpperLevel") == 0 ) {
			FLOAT32 fValue = atof(pszValue);
			SetUpperLevel(fValue);
			b = TRUE;
		}
	}
	return(b);
}

// set all values outside the range defined by upper/lower to zero
//
UINT32 CGAThreshold::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn = GetSignalData(SampleIn);

	FLOAT32 fSample = m_fSample;

	if(pfIn != NULL && pfOut != NULL) {
		
		for( UINT32 i=0; i< nFrames; i++ )
		{

			fSample = pfIn[i];
			FLOAT32 fa = fabs(fSample) ;
			if( fa >= m_fLowerLevel && fa <= m_fUpperLevel)
				m_fSample = fSample; 
			else
				m_fSample = 0.0f;

			pfOut[i] = m_fSample;
		}
	}

	return(0);

}





#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
