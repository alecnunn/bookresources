// GAMath Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Simple math for FLOAT32 scalar and vector
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
#include "GAMath.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAMath() { return( new CGAMath); }
const CClass CGAMath::classCGAMath(CID_GAMath, CID_GAObject, "CGAMath", CreateCGAMath);
const IClass*	CGAMath::GetClass() { return((const IClass*)&classCGAMath); }


// CTOR
CGAMath::CGAMath()
{

}

// DTOR
CGAMath::~CGAMath()
{

}

// explicit terminate baseclass
void CGAMath::Terminate()
{

	CGAObject::Terminate();
}


// round
INT32 CGAMath::Round (FLOAT32 fValue)
{
    INT32 nRounded=0;

 
    nRounded = (fValue >= 0.0f) ?
        ((INT32) (fValue + 0.5f)) : ((INT32) (fValue - 0.5f));
 
    return nRounded;
}

// zero
//---------------------------------------------------------------------------------------

void CGAMath::Zero (FLOAT32* pfDest, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		pfDest[i] = 0.0f;
	}
}



// set
//---------------------------------------------------------------------------------------
void CGAMath::Set (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] = fSource;
    }
}



// add
//---------------------------------------------------------------------------------------
void CGAMath::Add (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		pfDest[i] += fSource;
	}
}

void CGAMath::Add (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] += pfSource[i];
    }
}

void CGAMath::Add (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] = pfSource1[i] + pfSource2[i];
    }
}

// sub
//---------------------------------------------------------------------------------------
void CGAMath::Sub (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		pfDest[i] += fSource;
	}
}

void CGAMath::Sub (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] -= pfSource[i];
    }
}


void CGAMath::Sub (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] = pfSource1[i] - pfSource2[i];
    }
}

// mul
//---------------------------------------------------------------------------------------

void CGAMath::Mul (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		pfDest[i] *= fSource;
	}
}

void CGAMath::Mul (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] *= pfSource[i];
    }
}


void CGAMath::Mul (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] = pfSource1[i] * pfSource2[i];
    }
}

// div
//---------------------------------------------------------------------------------------

void CGAMath::Div (FLOAT32* pfDest, FLOAT32 fSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		pfDest[i] /= fSource;
	}
}

void CGAMath::Div (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] /= pfSource[i];
    }
}


void CGAMath::Div (FLOAT32* pfDest, const FLOAT32* pfSource1, const FLOAT32* pfSource2, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] = pfSource1[i] / pfSource2[i];
    }
}

// abs
//---------------------------------------------------------------------------------------

void CGAMath::Abs (FLOAT32* pfDest, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		pfDest[i] = fabsf(pfDest[i]);
	}
}

void CGAMath::Abs (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] = fabsf(pfSource[i]);
    }
}

// sqrt
//---------------------------------------------------------------------------------------

void CGAMath::Sqrt (FLOAT32* pfDest, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		pfDest[i] = sqrtf(pfDest[i]);
	}
}

void CGAMath::Sqrt (FLOAT32* pfDest, const FLOAT32* pfSource, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
        pfDest[i] = sqrtf(pfSource[i]);
    }
}


// max
//---------------------------------------------------------------------------------------

void CGAMath::Max (FLOAT32* pfDest, FLOAT32 fMax, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		if(pfDest[i] > fMax)
			pfDest[i] = fMax;
	}
}

void CGAMath::Max (FLOAT32* pfDest, const FLOAT32* pfSource, FLOAT32 fMax, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
		if(pfSource[i] > fMax)
			pfDest[i] = fMax;
		else
  			pfDest[i] = pfSource[i];
  }
}

// min
//---------------------------------------------------------------------------------------

void CGAMath::Min (FLOAT32* pfDest, FLOAT32 fMin, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		if(pfDest[i] < fMin)
			pfDest[i] = fMin;
	}
}

void CGAMath::Min (FLOAT32* pfDest, const FLOAT32* pfSource, FLOAT32 fMin, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
		if(pfSource[i] < fMin)
			pfDest[i] = fMin;
		else
  			pfDest[i] = pfSource[i];
  }
}

// clip
//---------------------------------------------------------------------------------------

void CGAMath::Clip (FLOAT32* pfDest, FLOAT32 fMin, FLOAT32 fMax, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
	{
		if(pfDest[i] < fMin)
			pfDest[i] = fMin;
		else if(pfDest[i] > fMax)
			pfDest[i] = fMax;
	}
}

void CGAMath::Clip (FLOAT32* pfDest, const FLOAT32* pfSource, FLOAT32 fMin, FLOAT32 fMax, UINT32 nCount)
{
	for (UINT32 i = 0; i < nCount; i++)
    {
		if(pfSource[i] < fMin)
			pfDest[i] = fMin;
		else if(pfSource[i] > fMax)
			pfDest[i] = fMax;
		else
  			pfDest[i] = pfSource[i];
  }
}

// convolve
//---------------------------------------------------------------------------------------
FLOAT32 CGAMath::Convolve (const FLOAT32* pfx, const FLOAT32* pfy,UINT32 nCount)
{
	FLOAT32 fConv = 0.0f;
	UINT32 nEnd = nCount - 1;

	for (UINT32 i = 0; i < nCount; i++)
	{
		fConv = fConv +(pfx[i] * pfy[nEnd-i]);
	}

	return(fConv);
}

void CGAMath::Convolve (FLOAT32* pfDest, const FLOAT32* pfx, const FLOAT32* pfy,UINT32 nCount)
{
	FLOAT32 fConv = 0.0f;
	UINT32 nEnd = nCount - 1;
	UINT32 ndx =0;

	for (UINT32 di = 0; di < nCount; di++)
	{
		for (UINT32 i = 0; i < nCount; i++)
		{
			ndx = i - di;
			
			if(ndx < 0)
                ndx += nEnd;

			fConv = fConv +(pfx[nEnd- i] * pfy[ndx]);
		}
		pfDest[di] = fConv;
		fConv = 0.0f;
	}
	
}


FLOAT32 CGAMath::Freq2Pitch(FLOAT32 fFreq)
{
	FLOAT32 f = 69.0f + 12.0f * (log(fFreq/440.0f)/log(2.0f));
	return (f);
}


#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
 