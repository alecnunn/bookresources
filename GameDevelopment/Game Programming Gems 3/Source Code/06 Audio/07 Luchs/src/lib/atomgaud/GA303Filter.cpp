// GA303Filter Implementation
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
#include "GA303Filter.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGA303Filter() { return( new CGA303Filter); }
const CClass CGA303Filter::classCGA303Filter(CID_GA303Filter, CID_GAProcessor, "CGA303Filter", CreateCGA303Filter);
const IClass*	CGA303Filter::GetClass() { return((const IClass*)&classCGA303Filter); }


// CTOR
CGA303Filter::CGA303Filter()
{
	SetName("303Filter");

	   // create the pins
	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn);
	AppendPin(FM);

	FLOAT32 fSR = g_GASystem.GetSampleRate();
	a = 0.0f;
	b = 0.0f;
	c = 0.0f;

	d1 = 0.0f;
	d2 = 0.0f;
	d3 = 0.0f;

    m_fCutoff = 0.05f;
    m_fResonance = 0.81f;

	m_fFM=0.0f;
	m_fFMAmount = 8.0f;
}

// DTOR
CGA303Filter::~CGA303Filter()
{

}

// call the baseclass terminate explicitly 
void CGA303Filter::Terminate()
{

	// cleanup base
	CGAProcessor::Terminate();
}

void CGA303Filter::Update()
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();

	// update cutoff
	FLOAT32 fCutoff = m_fCutoff * m_fFM; //  *ports[PORT_CUTOFF]
	FLOAT32 fResonance = m_fResonance * 100.0f; // *ports[PORT_RESONANCE])

 //   FLOAT32 whopping =  fCutoff; // e0 + c0;
    FLOAT32 k = exp (-fCutoff / fResonance);
  
    a = 2.0 * cos (fCutoff) * k ; // 
    b = -k * k;
    c = (1.0 - a - b) * 0.21;
  
}

/*

  void CGA303Filter::Update()
{

	FLOAT32 fSR = g_GASystem.GetSampleRate();

	// update cutoff
//	FLOAT32 fMod = 0.0f;	// *ports[PORT_ENV_MOD]
//	FLOAT32 fCutoff = 0.22f; //  *ports[PORT_CUTOFF]
	FLOAT32 fResonance = m_fResonance; // 0.5f; // *ports[PORT_RESONANCE])

    FLOAT32 e0 = exp (5.613 - 0.8 * m_fFM + 2.1553 * m_fCutoff - 0.7696 * (1.0 - fResonance));
    e0 *= ATOM_PI / fSR;

    FLOAT32 e1 = exp (6.109 + 1.5876 * m_fFM + 2.1553 * m_fCutoff  - 1.2 * (1.0 - fResonance));
    e1 *= ATOM_PI / fSR;

	FLOAT32 c0 = e1 - e0;
//	FLOAT32 c0 =  e0;

    // Update resonance. 
    fResonance = exp (4.66); // *ports[PORT_RESONANCE]);



    FLOAT32 whopping =  e0 + c0;
    FLOAT32 k = exp (-whopping / fResonance);
  
    a = 2.0 * cos (2.0 * whopping) * k;
    b = -k * k;
    c = (1.0 - a - b) * 0.2;
  
}
*/

UINT32 CGA303Filter::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfFM = GetSignalData(FM);

	if(pfIn != NULL &&  pfOut != NULL) {

		
  
		Update ();


		for( UINT32 i=0; i< nFrames; i++ )
		{


			 if(pfFM != NULL) {
				 
				 // convert mod range ( -1 +1)---> amount ( 0.25 --> 4.0 )
					 
				 FLOAT fMod = pfFM[i]; 
				 fMod *= (m_fFMAmount / 2);
				 fMod += (m_fFMAmount / 2);
				 fMod = pow(2,fMod);
				 fMod /= m_fFMAmount;
					 
				 m_fFM = fMod;

				 Update();
				 
			 }			



			FLOAT32 sample;
			
			sample = a * d1 + b * d2 + c * pfIn[i];
			pfOut[i] = sample;
			
			d2 = d1;
			d1 = sample;
			
		}
	}

	return(0);

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
