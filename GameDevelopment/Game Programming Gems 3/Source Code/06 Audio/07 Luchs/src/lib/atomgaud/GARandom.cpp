// GARandom Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1994-11-12	first draft
//				1995-03-06	replace of rand() 
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GARandom.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGARandom() { return( new CGARandom); }
const CClass CGARandom::classCGARandom(CID_GARandom, CID_GAObject, "CGARandom", CreateCGARandom);
const IClass*	CGARandom::GetClass() { return((const IClass*)&classCGARandom); }

// precision only 15 bit
// #define RandomF32Uni	(FLOAT32)((FLOAT32)rand()/(FLOAT32)RAND_MAX)

const FLOAT32 g_fSmall = 0.00000001f;

// CTOR
CGARandom::CGARandom()
{
	// init random
	m_vssRandSeed = 0L;
	m_nflone = 0x3f800000;
	m_nflmask = 0x007fffff;
	m_ntemp = 0L;
}

// DTOR
CGARandom::~CGARandom()
{

}

// explicit terminate baseclass
void CGARandom::Terminate()
{

	CGAObject::Terminate();
}

// explicit terminate baseclass
void CGARandom::Reset()
{
	// init random
	m_vssRandSeed = 0L;
	m_nflone = 0x3f800000;
	m_nflmask = 0x007fffff;
	m_ntemp = 0L;
}


// uniform unipolar random  0 to 1
// every value is equally common
// Originally from _Numerical Recipes in C_.
FLOAT32 CGARandom::UniformU()
{
	FLOAT32 f = 0.0f;
	m_vssRandSeed = 1664525L * m_vssRandSeed + 1013904223L;
	m_ntemp = m_nflone | (m_nflmask & m_vssRandSeed);
	f = ((*(FLOAT32*)&m_ntemp) - 1.0);

    return (f);
}

// uniform bipolar random  -1 to +1
FLOAT32 CGARandom::UniformB()
{	
	FLOAT32 f = 0.0f;
	m_vssRandSeed = 1664525L * m_vssRandSeed + 1013904223L;
	m_ntemp = m_nflone | (m_nflmask & m_vssRandSeed);
	f = ((*(FLOAT32*)&m_ntemp) - 1.0);
	f *=2;
	f -= 1.0f;

    return (f);
}

// linear distribution 0 to +1
// the distribution falls linearly from 0 to 1
// there are half as many values at 0.5 as around 0.0
FLOAT32 CGARandom::LinearU()
{
	FLOAT32 f1 = UniformU();
	FLOAT32 f2 = UniformU();

    if (f1 > f2)
		f1 = f2;

    return (f1);
}

// linear distribution -1 to +1
FLOAT32 CGARandom::LinearB()
{
	FLOAT32 f1 = UniformU();
	FLOAT32 f2 = UniformU();

    if (f1 > f2)
		f1 = f2;

 	f1 *= 2.0f;
	f1 -= 1.0f;

   return (f1);
}



// triangle distribution -1 to +1
FLOAT32 CGARandom::TriangleB()
{
	FLOAT32 f1 = UniformU();
	FLOAT32 f2 = UniformU();

	FLOAT32 f = f1+f2 -1.0f;

    return (f);
}



// 	 exponential distribution 0 to 1	
FLOAT32 CGARandom::ExponentialU()	
{
    FLOAT32 f =  UniformU() * (ATOM_E - 1.0f);
	f = (FLOAT32)log(1.0f + f ); 

    return (f);
}

// exponential distribution -1 to +1
FLOAT32 CGARandom::ExponentialB()	
{
    FLOAT32 f =  UniformU() * (ATOM_E - 1.0f);
	f = (FLOAT32)log(1.0f + f ); 
 	f *= 2.0f;
	f -= 1.0f;
	
	return (f);
}

/*
// bilateral distribution
FLOAT32 CGARandom::Bilateral(FLOAT32 fIn)	
{



    return (0.0f);
}
*/


// gaussian distribution -1 to +1
FLOAT32 CGARandom::GaussianB(FLOAT32 fIn)	
{
	fIn /= 3.83f;
    FLOAT32 f = 0.0f;
	
    for(UINT32 i=0;i <12; i++)
		f += UniformU();
 
	
	f = fIn * (f - 6.0f);

    return (f);
}



// cauchy distribution 0 to +1
FLOAT32 CGARandom::CauchyU(FLOAT32 fIn)	
{
 	fIn /= 318.3f;
    FLOAT32 f = 0.0f;

    do {
      do {
		f = UniformU();
      } while (f == 1.0f);

      f = fIn * (FLOAT32)tan( ATOM_PI * 0.5f * (double)f);
    } while ( f > 1.0f);


    return f;
}

// cauchy distribution -1 to +1
FLOAT32 CGARandom::CauchyB(FLOAT32 fIn)	
{
 	fIn /= 318.3f;
    FLOAT32 f = 0.0f;

    do {
      do {
		f = UniformU();
      } while (f == 0.5f);

      f = fIn * (FLOAT32)tan(ATOM_PI*(double)f);
    } while ( f > 1.0f || f < -1.0f); 

    return f;
}

// poisson distribution 0 to +1
FLOAT32 CGARandom::Poisson(FLOAT32 fIn)	
{

    if (fIn < 0.0f )
		return (0.0f);

    FLOAT32 f1 = UniformU();
    FLOAT32 f2 = (FLOAT32)exp(-fIn);
    FLOAT32 f3 = 0.0f;

    while (f1 >= f2) {
		f3++;
	f1 *= UniformU();
    }

    return (f3);
}


/*
// beta distribution 0 to +1
FLOAT32 CGARandom::Beta(FLOAT32 fIn)	
FLOAT32 betarand(FLOAT32 range, FLOAT32 a, FLOAT32 b)
{
    FLOAT32 r1, r2;

    if (a < FL(0.0) || b < FL(0.0) ) return (FL(0.0)); 

    do {
	do {
	    r1 = unirand();
	} while (r1 == FL(0.0));
	
	do {
	    r2 = unirand();
	} while (r2 == FL(0.0));
	
	r1 = (FLOAT32)pow(r1, 1.0 / (double)a);
	r2 = (FLOAT32)pow(r2, 1.0 / (double)b);
    } while ((r1 + r2) > FL(1.0));

    return ((r1 / (r1 +r2)) * range);
}

// weibull distribution 0 to +1
// can generate exp, gaussian and poisson like distribution
FLOAT32 CGARandom::Weibull(FLOAT32 fIn)	
FLOAT32 weibrand(FLOAT32 s, FLOAT32 t) 
{
    FLOAT32 r1, r2;

    if (t < FL(0.0) ) return (FL(0.0));

    do {
	r1 = unirand();
    } while (r1 == FL(0.0) || r1 == FL(1.0));

    r2 = FL(1.0) /  (FL(1.0) - r1);

    return (s * (FLOAT32)pow (log((double)r2),  (1.0 /(double)t)));
}
*/











#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
