// GAPhaser Implementation
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
#include "GAPhaser.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAPhaser() { return( new CGAPhaser); }
const CClass CGAPhaser::classCGAPhaser(CID_GAPhaser, CID_GAProcessor, "CGAPhaser", CreateCGAPhaser);
const IClass*	CGAPhaser::GetClass() { return((const IClass*)&classCGAPhaser); }


// CTOR
CGAPhaser::CGAPhaser()
{
	SetName("Phaser");

	FLOAT32 fSR = g_GASystem.GetSampleRate();

	m_fb = 0.7f;
    m_lfoPhase = 0.0f;
    m_depth = 0.5f; // 1.0f;
    m_zm1 = 0.0f;

   //	Rate( .5f );
	FLOAT32 fRate = 0.02f;
	m_lfoInc = 2.0f * ATOM_PI * (fRate / fSR);

   	// Range( 440.f, 1600.f );
	FLOAT32 fMinRange = 440.0f;
	FLOAT32 fMaxRange = 1600.0f;
    m_dmin = fMinRange / (fSR/2.0f);
    m_dmax = fMaxRange / (fSR/2.0f);

	IGASignal* pSampleOut = g_GASystem.CreateSignal(); // alloc in current frame size

	AppendPin(SampleOut, "SampleOut", pSampleOut);
	AppendPin(SampleIn);
//	AppendPin(FM, "FM", CGAPin::Input, NULL);
//	AppendPin(AM, "AM", CGAPin::Input, NULL);

}

void CGAPhaser::Terminate()
{
	CGAProcessor::Terminate();
}


/*
void CVMPhaser::Range( float fMin, float fMax )
{ // Hz
    _dmin = fMin / (SR/2.f);
    _dmax = fMax / (SR/2.f);
}

void CVMPhaser::Rate( float rate )
{ // cps
    _lfoInc = 2.f * ATOM_PI * (rate / SR);
}

void CVMPhaser::Feedback( float fb )
{ // 0 -> <1.
    _fb = fb;
}

void CVMPhaser::Depth( float depth )
{  // 0 -> 1.
    _depth = depth;
}
*/

// DTOR
CGAPhaser::~CGAPhaser()
{

}

float CGAPhaser::ProcessSample( float inSamp )
{
    //calculate and update phaser sweep lfo...
    float d  = m_dmin + (m_dmax-m_dmin) * ((sin( m_lfoPhase ) + 1.f)/2.f);

    m_lfoPhase += m_lfoInc;
    if( m_lfoPhase >= ATOM_PI * 2.f )
        m_lfoPhase -= ATOM_PI * 2.f;

    //update filter coeffs
    for( int i=0; i<6; i++ )
        m_alps[i].Delay( d );

    //calculate output
    float y = 	m_alps[0].Process(
        		 m_alps[1].Process(
                  m_alps[2].Process(
                   m_alps[3].Process(
                    m_alps[4].Process(
                     m_alps[5].Process( inSamp + m_zm1 * m_fb ))))));
    m_zm1 = y;
	FLOAT32 fOut = inSamp + y * m_depth;
    return fOut;
}



UINT32 CGAPhaser::Process(IObject* pob)
{

	UINT32 nFrames = g_GASystem.GetFrameCount();

	FLOAT32* pfIn = GetSignalData(SampleIn);
	FLOAT32* pfOut = GetSignalData(SampleOut);

		// init random
	static UINT32 m_vssRandSeed = 0L;
	static UINT32 m_jflone = 0x3f800000;
	static UINT32 m_jflmask = 0x007fffff;
	static UINT32 m_itemp = 0L;


	for (UINT32 i=0; i < nFrames; i++)
	{

		// test
		m_vssRandSeed = 1664525L * m_vssRandSeed + 1013904223L;
		m_itemp = m_jflone | (m_jflmask & m_vssRandSeed);
		FLOAT32 fWhite = ((*(FLOAT32*)&m_itemp) - 1.0);

	//	FLOAT32 fWhite = ((FLOAT32)RAND_MAX) / (rand() + 1);
		fWhite *=2;
		fWhite -= 1.0f;

		pfOut[i] = ProcessSample(fWhite);


/*
		FLOAT32 fIn = fWhite; // pfIn[i];

		FLOAT32 d  = m_dmin + (m_dmax-m_dmin) * ((sin( m_lfoPhase ) + 1.0f)/2.0f);
		m_lfoPhase += m_lfoInc;


		if( m_lfoPhase >= ATOM_PI * 2.0f )
			m_lfoPhase -= ATOM_PI * 2.0f;
		
		//update filter coeffs
		for( int i=0; i<6; i++ )
			m_alps[i].Delay( d );
		
		//calculate output
		FLOAT32 y = 	m_alps[0].Process(
						m_alps[1].Process(
						m_alps[2].Process(
						m_alps[3].Process(
						m_alps[4].Process(
						m_alps[5].Process( fIn + m_zm1 * m_fb ))))));
		m_zm1 = y;
		pfOut[i] =  fIn + y * m_depth;

  */
	}

	return(0);
}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
