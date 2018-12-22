// GASignalRandomizer Implementation
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
#include "GASignalRandomizer.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGASignalRandomizer() { return( new CGASignalRandomizer); }
const CClass CGASignalRandomizer::classCGASignalRandomizer(CID_GASignalRandomizer, CID_GAObject, "CGASignalRandomizer", CreateCGASignalRandomizer);
const IClass*	CGASignalRandomizer::GetClass() { return((const IClass*)&classCGASignalRandomizer); }


// CTOR
CGASignalRandomizer::CGASignalRandomizer()
{

}

// DTOR
CGASignalRandomizer::~CGASignalRandomizer()
{
	Terminate();
}

// explicit terminate baseclass
void CGASignalRandomizer::Terminate()
{

	CGAObject::Terminate();
}

FLOAT32 CGASignalRandomizer::InterpolateLinear( FLOAT32 a, FLOAT32 b, FLOAT32 x)
{
	return ( a*(1.0f-x) + b*x );
}


FLOAT32 CGASignalRandomizer::InterpolateCosine( FLOAT32 a, FLOAT32 b, FLOAT32 x)
{
	FLOAT32 ft = x * 3.1415926535f;
	FLOAT32 f = (FLOAT32)(1.0f - cos(ft)) * 0.5f;

	return ( a*(1.0f-f) + b*f);
}

FLOAT32 CGASignalRandomizer::InterpolateCubic( FLOAT32 f0, FLOAT32 f1, FLOAT32 f2, FLOAT32 f3, FLOAT32 x)
{
	FLOAT32 fx2 = x*x;
	FLOAT32 fx3 = fx2*x;

	FLOAT32 a = (f3 - f2) - (f0 - f1);
	FLOAT32 b = (f0 - f1) - a;
	FLOAT32 c = f2 - f0;
	FLOAT32 d = f1;

	return (a * fx3 + b * fx2 + c*x + d);
}





FLOAT32 CGASignalRandomizer::BipolarRandom(UINT32 x)
{
	x = (x<<13) ^ x;
    FLOAT32 f = ( 1.0f - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);    

	return(f);
}


// fill the signal with random values from -1 to +1
void CGASignalRandomizer::BipolarRandom(FLOAT32* pSamples, UINT32 nCount)
{
	FLOAT32 f = 0.0f;

	for(UINT32 i=0;i < nCount; i++) {
		f = BipolarRandom(i);
		pSamples[i] = f;
	}

}

// fill the signal with alternating values of -1 or +1 and a gap from
// 0 to nMaxGap
void CGASignalRandomizer::BipolarPeaks(FLOAT32* pSamples, UINT32 nCount, UINT32 nMaxGap)
{
	UINT32 nGap = 0;

	for(UINT32 i=0;i < nCount;i++) {

		if(nGap==0) {
			nGap = rand() % nMaxGap;
			pSamples[i] = i & 1 ? 1.0f : - 1.0f;
		}
		else {
			pSamples[i] = 0.0f;
			nGap--;
		}
	}

}

// fill the signal with random values from -1 to +1 and a gap from
// 0 to nMaxGap
void CGASignalRandomizer::BipolarImpulses(FLOAT32* pSamples, UINT32 nCount, UINT32 nMaxGap)
{
	UINT32 nGap = 0;

	for(UINT32 i=0;i < nCount;i++) {

		if(nGap==0) {
			nGap = rand() % nMaxGap;
			pSamples[i] = BipolarRandom(i);
		}
		else {
			pSamples[i] = 0.0f;
			nGap--;
		}
	}

}





FLOAT32 CGASignalRandomizer:: SmoothedNoise(UINT32 nx)
{
	FLOAT32 fa = BipolarRandom(nx)*0.5f;
  	FLOAT32 fb = BipolarRandom(nx-1)* 0.25f;
  	FLOAT32 fc = BipolarRandom(nx+1)* 0.25f;

	return (fa + fb + fc);
}






FLOAT32 CGASignalRandomizer::InterpolatedNoise(FLOAT32 fx)
{
      int nx   = (int) fx;
      FLOAT32 fxFrac = fx - nx;

      FLOAT32 f1 = SmoothedNoise(nx);
      FLOAT32 f2 = SmoothedNoise(nx + 1);

	  FLOAT32 fInterpol = InterpolateLinear(f1 , f2 , fxFrac);
      return (fInterpol);
}



FLOAT32 CGASignalRandomizer::PerlinNoise1D(FLOAT32 x, UINT32 nOctaves)
{
    FLOAT32 fTotal = 0.0f;
	FLOAT32 fPersistence = 0.5f;
//	int nOctaves = 8;
    FLOAT32 fFrequency = 1.0f;
    FLOAT32 fAmplitude = 0.0f;

	for (int i=0; i <nOctaves; i++) {

     fFrequency = (FLOAT32)ATOM_POW2[i];
     fAmplitude = fPersistence * i;

      fTotal += InterpolatedNoise(x * fFrequency) * fAmplitude;
	}

      return fTotal;
}  



#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
