/*===========================================================================
	Title: PAR.h
	Module: GPM/PAR
	Author: Ignacio Castaño
	Description: Main header file.
	Changes:
		11/01/2003 - Ignacio Castaño
			File added.

===========================================================================*/

#ifndef _GPM_PAR_H_
#define _GPM_PAR_H_


/*---------------------------------------------------------------------------
	Doc:
---------------------------------------------------------------------------*/

/** @file PAR.h
 * Main header file.
**/


/*---------------------------------------------------------------------------
	Headers:
---------------------------------------------------------------------------*/

//#include "Core.h"
//#include "Debug.h"
//#include "MathLib.h"


// From Core
#include <windows.h>
#include <stdio.h>

#define CORE_API

#define PI_DISABLE_COPY(C)								\
	private:											\
    C( const C & );										\
    C &operator=( const C & );

typedef signed char			sint8;
typedef signed short		sint16;
typedef signed int			sint32;
typedef signed __int64		sint64;

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned __int64	uint64;

typedef unsigned int		uint;

#define _CDECL	__cdecl

#pragma warning(disable : 4244)		// conversion to float, possible loss of data



// From LinkList
#define LL_Reset(list,next,prev)	(list)->next = (list)->prev = (list)

#define LL_AddLast( rootnode, newnode, next, prev )		 		\
	{                                              				\
		(newnode)->next = (rootnode);                  			\
		(newnode)->prev = (rootnode)->prev;                		\
		(rootnode)->prev->next = (newnode);                		\
		(rootnode)->prev = (newnode);                      		\
	}



// From Debug
#include <assert.h>

#define piAssert(exp)		assert(exp)
#define piCheck(exp)		assert(exp)
#define piLightCheck(exp)	assert(exp)


inline void piDebug( const char *msg, ... ) {
	va_list arg;
	va_start(arg,msg);
	vprintf( msg, arg );
	va_end(arg);	
}


// From MathLib
#include <math.h>
#include <limits.h>
#include <float.h>

#define MATHLIB_API

#define PI_MAX_FLOAT	FLT_MAX
#define PI_MIN_FLOAT	-FLT_MAX

#define REAL				float
#define InverseSqrt(a)		(1.0f/sqrtf(a))
#define SQ(a)				((a)*(a))
#define PI_EPSILON			0.00001

#define	SIGN_BITMASK			0x80000000
#define IR(x)					((uint32&)(x))

inline uint NextPowerOfTwo( uint n ) {
    uint p = 1;
    while( n > p )
        p += p;
    return p;
}

/** Float to byte. */
inline uint8 PackFloatInByte( float in ) {
	return (uint8) ((in+1.0f) / 2.0f * 255.0f);
}

/** Byte to float. */
inline float UnPackByteInFloat( uint8 in ) {
	return ((float)in) / 127.5f - 1.0f;
}

/** Float to word. */
inline uint16 PackFloatInWord( float in ) {
	return (uint16) ((in+1.0f) / 2.0f * 65535.0f);
}

/** Word to float. */
inline float UnPackWordInFloat( uint16 in ) {
	return ((float)in) / 32767.5f - 1.0f;
}


// From Templates

template <class T> inline T piMax(const T a, const T b){
	return (a>=b)?a:b;
}


template <class T> inline T piMin(const T a, const T b){
	return (a<=b)?a:b;
}

template <class T> inline void piSwap(T &a, T &b){
	T temp = a; 
	a = b; 
	b = temp;
}




#endif // _GPM_PAR_H_