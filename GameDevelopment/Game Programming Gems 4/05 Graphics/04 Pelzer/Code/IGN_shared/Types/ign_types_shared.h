
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			General shared
//*				File:				ign_types_shared.h
//*				Description:		Shared file with type declarations/definitions
//*
//********************************************************************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************************************************************
//*
//*				History:
//*
//*				11/11/02| 0.0.1		## Initial release															(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_TYPES_SHARED_H_INCLUDED
#define			IGN_TYPES_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_types_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#include		<windows.h>




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------

#define			IGN_DLLEXPORT										"C" __declspec( dllexport )
#define			IGN_DLLCALL											__cdecl
#define			IGN_OBJCALL											__stdcall
#define			IGN_OBJ_METHOD( a, b )								virtual a b = 0
#define			IGN_OBJ_INTERFACE_DECL_BaseObject( a )				struct a : public IGN_OBJIF_Base
#define			IGN_OBJ_INTERFACE_DECL_ExecutableObject( a )		struct a : public IGN_OBJIF_Executable

#ifndef MAKEFOURCC
  #define MAKEFOURCC(ch0, ch1, ch2, ch3)							((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
																	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif // MAKEFOURCC

#define			IGN_OK												1
#define			IGN_FAILED											0
#define			IGN_NOT_IMPLEMENTED									-1




typedef			int							IGNRESULT;
typedef			bool						IGNBOOL;
typedef			char						IGNCHAR;
typedef			unsigned char				IGNUCHAR;
typedef			BYTE						IGNU8;
typedef			unsigned long				IGNU32;
typedef			_LARGE_INTEGER				IGNU64;
typedef			int							IGNINT;
typedef			unsigned int				IGNUINT;
typedef			WORD						IGNWORD;
typedef			DWORD						IGNDWORD;
typedef			float						IGNFLOAT;
typedef			double						IGNDOUBLE;
typedef			void*						IGNHANDLE;




//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Base methods																				
//
typedef			IGNU32		IGN_OBJCALL	IGN_METHOD_Base_AddRef								(			void														);
typedef			IGNU32		IGN_OBJCALL	IGN_METHOD_Base_Release								(			void														);	

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Executable methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_Executable_Execute						(			void														);




//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fast math methods																				
//
__forceinline void FloatToInt( IGNINT* int_pointer, IGNFLOAT f ) 
{
	__asm  fld  f
	__asm  mov  edx,int_pointer
	__asm  FRNDINT
	__asm  fistp dword ptr [edx];
}




//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Base interface	
//
struct	IGN_OBJIF_Base
{
	//
	// Version 1.0.0
	//

	//
	// Increases the interface's reference count by 1
	//
	IGN_OBJ_METHOD(	IGN_METHOD_Base_AddRef,									AddRef							);

	//
	// Decreases the reference count of the interface by 1
	// The object that exports the interface deallocates itself when its total reference count reaches 0
	//
	IGN_OBJ_METHOD(	IGN_METHOD_Base_Release,								Release							);
};	

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Executable interface	
//
struct	IGN_OBJIF_Executable : public IGN_OBJIF_Base
{
	//
	// Version 1.0.0
	//

	//
	// Executes the object with saved stati
	//
	IGN_OBJ_METHOD(	IGN_METHOD_Executable_Execute,							Execute							);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Stuff added by Marco Spoerl
//

const IGNFLOAT	IGN_PI			=	3.14159265358979323846f;
const IGNFLOAT	IGN_2PI			=	6.28318530717958623200f;
const IGNFLOAT	IGN_PIDiv2		=	1.57079632679489655800f;
const IGNFLOAT	IGN_PIDiv4		=	0.78539816339744827900f;
const IGNFLOAT	IGN_InvPI		=	0.31830988618379069122f;
const IGNFLOAT	IGN_DegToRad	=	0.01745329251994329547f;
const IGNFLOAT	IGN_RadToDeg	=  57.29577951308232286465f;
const IGNFLOAT	IGN_Epsilon		=	1.0e-5f;

template< class T >
T Floor ( const T& x )
{
	return static_cast<T>((int)(x) - (((x) < 0) && ((x) != (int)(x))));
}

template< class T , class ARG >
T Lerp ( const ARG& t, const T& x0, const T& x1 )
{	
	T r = x1;

	r = r - x0;
	r = r * t;
	r += x0;

	return r;		//	return ( x0 + ( x1 - x0 ) * t  ); 
}




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_TYPES_SHARED_H_INCLUDED


/*****************************************************************************
File:  fastmath.cpp

This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/
/*
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wtypes.h>


#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000


// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}

/////////////////////////////////////////////////
// The following comes from Vincent Van Eeckhout
// Thanks for sending us the code!
// It's the same thing in assembly but without this C-needed line:
//    r = *(float *)&_i;

float   two = 2.0f;

#define FP_INV2(r,p)                     \
{                                        \
    __asm { mov     eax,0x7F000000    }; \
    __asm { sub     eax,dword ptr [p] }; \
    __asm { mov     dword ptr [r],eax }; \
    __asm { fld     dword ptr [p]     }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fsubr   [two]             }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fstp    dword ptr [r]     }; \
}

/////////////////////////////////////////////////


#define FP_EXP(e,p)                                                          \
{                                                                            \
    int _i;                                                                  \
    e = -1.44269504f * (float)0x00800000 * (p);                              \
    _i = (int)e + 0x3F800000;                                                \
    e = *(float *)&_i;                                                       \
}

#define FP_NORM_TO_BYTE(i,p)                                                 \
{                                                                            \
    float _n = (p) + 1.0f;                                                   \
    i = *(int *)&_n;                                                         \
    if (i >= 0x40000000)     i = 0xFF;                                       \
    else if (i <=0x3F800000) i = 0;                                          \
    else i = ((i) >> 15) & 0xFF;                                             \
}



inline unsigned long FP_NORM_TO_BYTE2(float p)                                                 
{                                                                            
  float fpTmp = p + 1.0f;                                                      
  return ((*(unsigned *)&fpTmp) >> 15) & 0xFF;  
}


inline unsigned long FP_NORM_TO_BYTE3(float p)     
{
  float ftmp = p + 12582912.0f;                                                      
  return ((*(unsigned long *)&ftmp) & 0xFF);
}



static unsigned int fast_sqrt_table[0x10000];  // declare table of square roots 

typedef union FastSqrtUnion
{
  float f;
  unsigned int i;
} FastSqrtUnion;

void  build_sqrt_table()
{
  unsigned int i;
  FastSqrtUnion s;
  
  for (i = 0; i <= 0x7FFF; i++)
  {
    
    // Build a float with the bit pattern i as mantissa
    //  and an exponent of 0, stored as 127
    
    s.i = (i << 8) | (0x7F << 23);
    s.f = (float)sqrt(s.f);
    
    // Take the square root then strip the first 7 bits of
    //  the mantissa into the table
    
    fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);
    
    // Repeat the process, this time with an exponent of 1, 
    //  stored as 128
    
    s.i = (i << 8) | (0x80 << 23);
    s.f = (float)sqrt(s.f);
    
    fast_sqrt_table[i] = (s.i & 0x7FFFFF);
  }
}


inline float fastsqrt(float n)
{
  
  if (FP_BITS(n) == 0)
    return 0.0;                 // check for square root of 0
  
  FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
  
  return n;
}


// At the assembly level the recommended workaround for the second FIST bug is the same for the first; 
// inserting the FRNDINT instruction immediately preceding the FIST instruction. 

__forceinline void FloatToInt(int *int_pointer, float f) 
{
	__asm  fld  f
  __asm  mov  edx,int_pointer
  __asm  FRNDINT
  __asm  fistp dword ptr [edx];

}



int main(int argc, char* argv[])
{

  float t, it, test_sqrt;
  int i = 0;

  build_sqrt_table();

  t = 1234.121234f;
  
  test_sqrt = fastsqrt(t);
  printf("sqrt expected %20.10f  approx %20.10f\n", sqrt(t), test_sqrt);

  FP_INV(it,t);
  printf("inv  expected %20.10f  approx %20.10f\n", 1/t, it);


  i = 0xdeafbabe;

  FloatToInt(&i, t);
  printf("ftol expected %d  actual %d %08X\n", (int)t, i, i);

  return 0;
}


/////////////////////////////////////////////////////////////////////
//  3D and geometry ops /////////////////////////////////////////////



//-----------------------------------------------------------------------------
// Name: CylTest_CapsFirst
// Orig: Greg James - gjames@NVIDIA.com
// Lisc: Free code - no warranty & no money back.  Use it all you want
// Desc: 
//    This function tests if the 3D point 'testpt' lies within an arbitrarily
// oriented cylinder.  The cylinder is defined by an axis from 'pt1' to 'pt2',
// the axis having a length squared of 'lengthsq' (pre-compute for each cylinder
// to avoid repeated work!), and radius squared of 'radius_sq'.
//    The function tests against the end caps first, which is cheap -> only 
// a single dot product to test against the parallel cylinder caps.  If the
// point is within these, more work is done to find the distance of the point
// from the cylinder axis.
//    Fancy Math (TM) makes the whole test possible with only two dot-products
// a subtract, and two multiplies.  For clarity, the 2nd mult is kept as a
// divide.  It might be faster to change this to a mult by also passing in
// 1/lengthsq and using that instead.
//    Elminiate the first 3 subtracts by specifying the cylinder as a base
// point on one end cap and a vector to the other end cap (pass in {dx,dy,dz}
// instead of 'pt2' ).
//
//    The dot product is constant along a plane perpendicular to a vector.
//    The magnitude of the cross product divided by one vector length is
// constant along a cylinder surface defined by the other vector as axis.
//
// Return:  -1.0 if point is outside the cylinder
// Return:  distance squared from cylinder axis if point is inside.
//
//-----------------------------------------------------------------------------

struct Vec3
{
	float x;
	float y;
	float z;
};

float CylTest_CapsFirst( const Vec3 & pt1, const Vec3 & pt2, float lengthsq, float radius_sq, const Vec3 & testpt )
{
	float dx, dy, dz;	// vector d  from line segment point 1 to point 2
	float pdx, pdy, pdz;	// vector pd from point 1 to test point
	float dot, dsq;

	dx = pt2.x - pt1.x;	// translate so pt1 is origin.  Make vector from
	dy = pt2.y - pt1.y;     // pt1 to pt2.  Need for this is easily eliminated
	dz = pt2.z - pt1.z;

	pdx = testpt.x - pt1.x;		// vector from pt1 to test point.
	pdy = testpt.y - pt1.y;
	pdz = testpt.z - pt1.z;

	// Dot the d and pd vectors to see if point lies behind the 
	// cylinder cap at pt1.x, pt1.y, pt1.z

	dot = pdx * dx + pdy * dy + pdz * dz;

	// If dot is less than zero the point is behind the pt1 cap.
	// If greater than the cylinder axis line segment length squared
	// then the point is outside the other end cap at pt2.

	if( dot < 0.0f || dot > lengthsq )
	{
		return( -1.0f );
	}
	else 
	{
		// Point lies within the parallel caps, so find
		// distance squared from point to line, using the fact that sin^2 + cos^2 = 1
		// the dot = cos() * |d||pd|, and cross*cross = sin^2 * |d|^2 * |pd|^2
		// Carefull: '*' means mult for scalars and dotproduct for vectors
		// In short, where dist is pt distance to cyl axis: 
		// dist = sin( pd to d ) * |pd|
		// distsq = dsq = (1 - cos^2( pd to d)) * |pd|^2
		// dsq = ( 1 - (pd * d)^2 / (|pd|^2 * |d|^2) ) * |pd|^2
		// dsq = pd * pd - dot * dot / lengthsq
		//  where lengthsq is d*d or |d|^2 that is passed into this function 

		// distance squared to the cylinder axis:

		dsq = (pdx*pdx + pdy*pdy + pdz*pdz) - dot*dot/lengthsq;

		if( dsq > radius_sq )
		{
			return( -1.0f );
		}
		else
		{
			return( dsq );		// return distance squared to axis
		}
	}
}
*/
