// GANoiseGenerator Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Voss - McCartney noise generation, based on code by Phil Burk
//				adding multiple white noise sources at successively lower octaves
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs, 
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GANoiseGenerator.h"

#define PINK_MAX_RANDOM_ROWS   (30)
#define PINK_RANDOM_BITS       (24)
#define PINK_RANDOM_SHIFT      ((sizeof(long)*8)-PINK_RANDOM_BITS)

UINT32		m_nRowCount;
INT32      m_nRows[PINK_MAX_RANDOM_ROWS];



#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGANoiseGenerator() { return( new CGANoiseGenerator); }
const CClass CGANoiseGenerator::classCGANoiseGenerator(CID_GANoiseGenerator, CID_GAGenerator, "CGANoiseGenerator", CreateCGANoiseGenerator);
const IClass*	CGANoiseGenerator::GetClass() { return((const IClass*)&classCGANoiseGenerator); }


// CTOR
CGANoiseGenerator::CGANoiseGenerator()
{
	SetName("Noise Generator");

	m_nSeed = 22222;  // Change this for different random sequences. 
	m_nRowCount =  12;
	m_nRunningSum=0;   // Used to optimize summing of generators.
	m_nIndex = 0;        // Incremented each sample.
	m_nIndexMask = (1<<m_nRowCount) - 1;    // Index wrapped by ANDing with this mask.

	INT32 pmax = (m_nRowCount + 1) * (1<<(PINK_RANDOM_BITS-1))	;
	m_fScale = 1.0f / pmax;       // Used to scale within range of -1.0 to +1.0
	m_fSample = 0.0f;

	Initialize();
}

// DTOR
CGANoiseGenerator::~CGANoiseGenerator()
{
	Terminate();
}

void CGANoiseGenerator::Terminate()
{
	CGAGenerator::Terminate();
}



// Calculate pseudo-random 32 bit number based on linear congruential method.
UINT32 CGANoiseGenerator::GenerateRandomNumber( void )
{
	m_nSeed = (m_nSeed * 196314165) + 907633515;
	return m_nSeed;
}


BOOL CGANoiseGenerator::Initialize(IObject* pob)
{
	m_nRowCount = 12;
	m_nIndex = 0;
	m_nIndexMask = (1<<m_nRowCount) - 1;

// Calculate maximum possible signed random value. Extra 1 for white noise always added. 
	INT32 pmax = (m_nRowCount + 1) * (1<<(PINK_RANDOM_BITS-1));
	m_fScale = 1.0f / pmax;

// Initialize rows. 
	for(int i=0; i<m_nRowCount; i++ )
		m_nRows[i] = 0;

	m_nRunningSum = 0;

	return(TRUE);
}


// Generate Pink noise values between -1.0 and +1.0 
FLOAT32 CGANoiseGenerator::ProcessSample( )
{
	long newRandom;
	long sum;
	float fSample; // output

	UINT32 nShift =  PINK_RANDOM_SHIFT;
// Increment and mask index. 
	m_nIndex = (m_nIndex + 1) & m_nIndexMask;
	
	// If index is zero, don't update any random values. 
	if( m_nIndex != 0 )
	{
		// Determine how many trailing zeros in PinkIndex. 
		// This algorithm will hang if n==0 so test first. 
		int numZeros = 0;
		int n = m_nIndex;
		while( (n & 1) == 0 )
		{
			n = n >> 1;
			numZeros++;
		}
		// Replace the indexed ROWS random value.
		// Subtract and add back to RunningSum instead of adding all the random
		// values together. Only one changes each time.
		//
		m_nRunningSum -= m_nRows[numZeros];
		newRandom = ((long)GenerateRandomNumber()) >> nShift;
		m_nRunningSum += newRandom;
		m_nRows[numZeros] = newRandom;
	}
	
// Add extra white noise value. 
	newRandom = ((long)GenerateRandomNumber()) >> nShift;
	sum = m_nRunningSum + newRandom;

//	sum = m_nRunningSum + newRandom;

// Scale to range of -1.0 to 0.9999. 
	fSample = sum * m_fScale;


	return fSample;
}


UINT32 CGANoiseGenerator::Process(IObject* pob)
{
	UINT32 nFrames = g_GASystem.GetFrameCount();
	FLOAT32* pfOut = GetSignalData(SampleOut);
	FLOAT32* pfAM = GetSignalData(AM);
	FLOAT32* pfAmplitude = GetSignalData(Amplitude);
	FLOAT32 fAmplitude = m_fAmplitude;

	static UINT32 nCounter=0;
	for( UINT32 i=0; i< nFrames; i++ )
	{

		// unipolar amplitude modulation
		if(pfAmplitude != NULL) {
			
			fAmplitude = pfAmplitude[i];
		}

		// bipolar amplitude modulation
		if(pfAM != NULL) {
			
			// mod range ( -1 to +1) -> 0 to 1
			FLOAT32 fMod = pfAM[i];
			fMod += 1.0f;
			fMod *= 0.5f;

			fAmplitude = fMod;
		}

		m_fSample = ProcessSample() * fAmplitude;
		pfOut[i] = m_fSample;
		nCounter++;
	}

	return(0);

}

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
