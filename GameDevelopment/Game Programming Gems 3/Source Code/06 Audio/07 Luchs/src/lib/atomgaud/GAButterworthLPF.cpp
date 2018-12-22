// GAButterworthLPF Implementation
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
#include "GAButterworthLPF.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAButterworthLPF() { return( new CGAButterworthLPF); }
const CClass CGAButterworthLPF::classCGAButterworthLPF(CID_GAButterworthLPF, CID_GAButterworthFilter, "CGAButterworthLPF", CreateCGAButterworthLPF);
const IClass*	CGAButterworthLPF::GetClass() { return((const IClass*)&classCGAButterworthLPF); }


// CTOR
CGAButterworthLPF::CGAButterworthLPF()
{
	SetName("Butterworth LPF");
	 Update();
}

// DTOR
CGAButterworthLPF::~CGAButterworthLPF()
{

}

void CGAButterworthLPF::Terminate()
{
	CGAButterworthFilter::Terminate();
}


// Call this after a parameter (cutoff frequency or bandwidth) has changed
void CGAButterworthLPF::Update()
{
	FLOAT32 fSR = g_GASystem.GetSampleRate();
	FLOAT32 fHalfNyquist = fSR * 0.25f;

	if(m_fCurrFreq > fHalfNyquist) // limit
		m_fCurrFreq = fHalfNyquist;


      m_C = 1.0 / tan(ATOM_PI * m_fCurrFreq / fSR );
 
      m_a[0] = 1.0 / (1.0 + sqrt(2.0) * m_C + m_C * m_C);
      m_a[1] = 2.0 * m_a[0];
      m_a[2] = m_a[0];
 
      m_b[0] = 2 * (1.0 - m_C * m_C) * m_a[0];
      m_b[1] = (1.0 - sqrt(2.0) * m_C + m_C * m_C) * m_a[0];

}



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
