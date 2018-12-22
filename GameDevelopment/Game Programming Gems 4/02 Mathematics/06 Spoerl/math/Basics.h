#ifndef			BASICS_H_INCLUDED
#define			BASICS_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		<cmath> 
#include		<cstdlib>

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	Values
//*****************************************************************************

#ifdef	NULL
#undef	NULL
#endif

#define NULL		0			//!< Definition of NULL keyword
#define FORCE_DWORD	0x7fffffff	//!< Force dword to 4 bytes in enum

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	Primitives
//*****************************************************************************

typedef unsigned char		U8	;	//!< Unsigned char (8 bit) definition
typedef signed   char		I8	;	//!< Signed char (8bit) definition
typedef unsigned short		U16	;	//!< Unsigned short (word, 16 bit) definition
typedef signed   short		I16	;	//!< Signed short (word, 16 bit) definition
typedef signed   long		I32	;	//!< Signed long (dword, 32 bit) definition
typedef unsigned long		U32	;	//!< Unsigned long (dword, 32 bit) definition
typedef signed __int64		I64	;	//!< Signed ultra long (qword, 64 bit) definition
typedef unsigned __int64	U64	;	//!< Unsigned ultra long (qword, 64 bit) definition

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	Global vars
//*****************************************************************************

extern const float	g_fPI			;	//!< PI 
extern const float	g_f2PI			;	//!< PI * 2 
extern const float	g_fPIDiv2		;	//!< PI / 2 
extern const float	g_fPIDiv4		;	//!< PI / 4 
extern const float	g_fInvPI		;	//!< 1 / PI 
extern const float	g_fDegToRad		;	//!< Degrees to Radians
extern const float	g_fRadToDeg		;	//!< Radians to Degrees
extern const float	g_fHuge			;	//!< Huge float number
extern const float	g_fEpsilon		;	//!< Tiny float (almost zero)
extern const float	g_fInvEpsilon	;	//!< Huge float (1/epsilon)
extern const float	g_fPosInfinity	;	//!< Positive infinity
extern const float	g_fNegInfinity	;	//!< Negative infinity
extern const float	g_fFOV			;	//!< Field of View
extern const float	g_fZNear		;	//!< Near clipping distance
extern const float	g_fZFar			;	//!< Far clipping distance

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	Global functions
//*****************************************************************************

//*****************************************************************************
//	Min
//*****************************************************************************
//
//!	\brief	Returns the smaller of the two given values
//!	\param	[in] First value to check
//!	\param	[in] Second value to check
//! \return	The smaller of the two values
//
//*****************************************************************************
template< class T >
T Min ( const T& a, const T& b )
{
	return a < b ? a : b;
}

//*****************************************************************************
//	Max
//*****************************************************************************
//
//!	\brief	Returns the bigger of the two given values
//!	\param	[in] First value to check
//!	\param	[in] Second value to check
//! \return	The bigger of the two values
//
//*****************************************************************************
template< class T >
T Max ( const T& a, const T& b )
{
	return a > b ? a : b;
}

//*****************************************************************************
//	Clamp
//*****************************************************************************
//
//!	\brief	Clamps the given value
//!	\param	[in] The value to clamp
//!	\param	[in] The lower constraint
//!	\param	[in] The upper constraint
//! \return	The clamped value
//
//*****************************************************************************
template< class T >
T Clamp	( const T& x, const T& min, const T& max )
{
	return Max( min, Min( max, x ) );
}

//*****************************************************************************
//	Saturate
//*****************************************************************************
//
//!	\brief	Saturates the given value
//!	\param	[in] The value to saturate
//!	\param	[in] The lower constraint
//!	\param	[in] The upper constraint
//! \return	The saturated value
//
//*****************************************************************************
template< class T >
void Saturate ( T& x, const T& min, const T& max )
{
	x = ( x > max ) ? (x % max) : ( x < min ) ? (-x % max) : x;
}

//*****************************************************************************
//	Step
//*****************************************************************************
//
//!	\brief	Determines a step
//!	\param	[in] Value to check to
//!	\param	[in] Value to check
//! \return	Returns either of the following values
//! \retval	true	- x is greater or euqals a
//! \retval	false	- x is smaller than a
//
//*****************************************************************************
template< class T >
bool Step ( const T& a, const T& x )
{
	return x >= a;
}

//*****************************************************************************
//	Mix
//*****************************************************************************
//
//!	\brief	Mixes the given values
//!	\param	[in] Value 1
//!	\param	[in] Value 2
//!	\param	[in] Value 3
//! \return	The mixed value
//
//*****************************************************************************
template< class T >
T Mix ( const T& a, const T& b, const T& f )
{
	return (1.0 - f ) * a + f * b;
}

//*****************************************************************************
//	Pulse
//*****************************************************************************
//
//!	\brief	?
//!	\param	[in] Value 1
//!	\param	[in] Value 2
//!	\param	[in] Value 3
//! \return	?
//
//*****************************************************************************
template< class T >
bool Pulse ( const T& a, const T& b, const T& x )
{
	return Step( a, x ) - Step( b, x );
}

//*****************************************************************************
//	Abs
//*****************************************************************************
//
//!	\brief	Returns the absolute value
//!	\param	[in] Value to use
//! \return	The absolute value
//
//*****************************************************************************
template< class T >
T Abs ( const T& x )
{
	return x < 0 ? -x : x;
}

//*****************************************************************************
//	Floor
//*****************************************************************************
//
//!	\brief	Returns the floor of the value
//!	\param	[in] Value to use
//! \return	The floor of the specified value
//
//*****************************************************************************
template< class T >
T Floor ( const T& x )
{
	return (int)(x) - (((x) < 0) && ((x) != (int)(x)));
}

//*****************************************************************************
//	Ceil
//*****************************************************************************
//
//!	\brief	Returns the ceiling of the value
//!	\param	[in] Value to use
//! \return	The ceiling of the specified value
//
//*****************************************************************************
template< class T >
T Ceil ( const T& x )
{
	return (int)(x) + (((x) > 0) && ((x) != (int)(x)));
}

//*****************************************************************************
//	Swap
//*****************************************************************************
//
//!	\brief	Swaps the values
//!	\param	[in/out] Value 1
//!	\param	[in/out] Value 2
//
//*****************************************************************************
template< class T >
void Swap ( T& a, T& b )
{
	T temp = a;
	a = b;
	b = temp;
}

//*****************************************************************************
//	Bias
//*****************************************************************************
//
//!	\brief	?
//!	\param	[in] Value 1
//!	\param	[in] Value 2
//! \return	?
//
//*****************************************************************************
template< class T >
T Bias( const T& b, const T& x )
{
	return pow( x, log( b ) / log( 0.5 ) );
}

//*****************************************************************************
//	Gain
//*****************************************************************************
//
//!	\brief	?
//!	\param	[in] Value 1
//!	\param	[in] Value 2
//! \return	?
//
//*****************************************************************************
template< class T >
T Gain ( const T& g, const T& x )
{
	return ( x < 0.5 ) ? ( Bias( 1 - g, 2 * x ) / 2 ) : ( 1 - ( Bias( 1 - g, 2 * x ) / 2 ) ) ;
}

//*****************************************************************************
//	Square
//*****************************************************************************
//
//!	\brief	Squares the given value
//!	\param	[in] Value to square
//! \return	The squared value
//
//*****************************************************************************
template< class T >
T Square ( const T& x )
{
	return ( x * x );
}

//*****************************************************************************
//	Smoothstep
//*****************************************************************************
//
//!	\brief	?
//!	\param	[in] Value 1
//! \return	?
//
//*****************************************************************************
template< class T >
T Smoothstep ( const T& x )
{
	return ( x * x * ( 3 - ( x + x ) ) );
}

//*****************************************************************************
//	Smoothstep
//*****************************************************************************
//
//!	\brief	?
//!	\param	[in] Value 1
//!	\param	[in] Value 2
//!	\param	[in] Value 3
//! \return	?
//
//*****************************************************************************
template< class T >
T Smoothstep ( const T& a, const T& b, const T& x ) 
{
	if ( x <  a ) return 0;
	if ( x >= b ) return 1;

	x = (x-a)/(b-a);

	return ( x * x * ( 3 - ( x + x ) ) );
}

//*****************************************************************************
//	Mod
//*****************************************************************************
//
//!	\brief	Modulus "operator"
//!	\param	[in/out]	Value 1
//!	\param	[in]		Value 2
//! \return	The remainder when the first value is divided by the second
//
//*****************************************************************************
template< class T>
T Mod ( T& a, const T& b ) 
{
	int n = (int)( a / b );

	a -= (n * b);

	if ( a < 0 ) 
	{
		a += b;
	}

	return a;
}

//*****************************************************************************
//	Pythagoras
//*****************************************************************************
//
//!	\brief	Pythagoras "operator"
//!	\param	[in] Value 1
//!	\param	[in] Value 2
//!	\param	[in] Value 3
//! \return	The result ;)
//
//*****************************************************************************
template< class T >
T Pythagoras ( const T& x, const T& y, const T& z )
{	
	return ::sqrt( x * x + y * y + z * z );
}

//*****************************************************************************
//	Lerp
//*****************************************************************************
//
//!	\brief	Linear interpolation
//!	\param	[in] Value 1
//!	\param	[in] Value 2
//!	\param	[in] Value 3
//! \return	The interpolated value
//
//*****************************************************************************
template< class T , class ARG >
T Lerp ( const ARG& t, const T& x0, const T& x1 )	// Assuming t ist almost always a non-class
{	
	T r = x1;

	r -= x0;
	r *= t;
	r += x0;

	return r;		//	return ( x0 + ( x1 - x0 ) * t  ); 
}

//*****************************************************************************
//	g_fabsf
//*****************************************************************************
//
//! \brief	Returns the abolute of the specified value
//! \param	[in] The value whose absolute to retrieve
//! \return	The absolute value
//
//*****************************************************************************
inline
#ifndef __BCPLUSPLUS__
float g_fabsf( float a_fAngle )
{
	return ::fabsf( a_fAngle );
}
#else
float g_fabsf( float a_fAngle )
{
	return static_cast<float> (::fabs( static_cast<double> (a_fAngle) ));
}
#endif

//*****************************************************************************
//	g_log10f
//*****************************************************************************
//
//! \brief	Returns the base-10 logarithm of the specified value
//! \param	[in] The value to determine base-10 logarithm for
//! \return	The base-10 logarithm of the specified value
//
//*****************************************************************************
inline
float g_log10f( float a_fValue )
{
	return static_cast<float> (::log10( static_cast<double> (a_fValue) ));
}

//*****************************************************************************
//	g_logf
//*****************************************************************************
//
//! \brief	Returns the natural (base-e) logarithm of the specified value
//! \param	[in] The value to determine natural (base-e) logarithm for
//! \return	The natural (base-e) logarithm of the specified value
//
//*****************************************************************************
inline
float g_logf( float a_fValue )
{
	return static_cast<float> (::log( static_cast<double> (a_fValue) ));
}

//*****************************************************************************
//	g_logf
//*****************************************************************************
//
//! \brief	Returns the logarithm of the specified value for the specified base
//! \param	[in] The base to determine logarithm of
//! \param	[in] The value to determine logarithm for
//! \return	The specified base's logarithm of the specified value
//
//*****************************************************************************
inline
float g_logf( float a_fBase, float a_fValue )
{
	return static_cast< float > (	( ::log( static_cast< double > (a_fValue) ) ) 
								  / ( ::log( static_cast< double > (a_fBase ) ) ) );
}

//*****************************************************************************
//	g_sinf
//*****************************************************************************
//
//! \brief	Returns the sine of the specified angle
//! \param	[in] The angle whose sine to retrieve
//! \return	The angle's sine
//
//*****************************************************************************
inline
#ifndef __BCPLUSPLUS__
float g_sinf( float a_fAngle )
{
	return ::sinf( a_fAngle );
}
#else
float g_sinf( float a_fAngle )
{
	return static_cast<float> (::sin( static_cast<double> (a_fAngle) ));
}
#endif

//*****************************************************************************
//	g_cosf
//*****************************************************************************
//
//! \brief	Returns the cosine of the specified angle
//! \param	[in] The angle whose cosine to retrieve
//! \return	The angle's cosine
//
//*****************************************************************************
inline
#ifndef __BCPLUSPLUS__
float g_cosf( float a_fAngle )
{
	return ::cosf( a_fAngle );
}
#else
float g_cosf( float a_fAngle )
{
	return static_cast<float> (::cos( static_cast<double> (a_fAngle) ));
}
#endif

//*****************************************************************************
//	g_asinf
//*****************************************************************************
//
//! \brief	Returns the arcus sine of the specified angle
//! \param	[in] The angle whose arcus sine to retrieve
//! \return	The angle's arcus sine
//
//*****************************************************************************
inline
#ifndef __BCPLUSPLUS__
float g_asinf( float a_fAngle )
{
	return ::asinf( a_fAngle );
}
#else
float g_asinf( float a_fAngle )
{
	return static_cast<float> (::asin( static_cast<double> (a_fAngle) ));
}
#endif

//*****************************************************************************
//	g_acosf
//*****************************************************************************
//
//! \brief	Returns the arcus cosine of the specified angle
//! \param	[in] The angle whose arcus cosine to retrieve
//! \return	The angle's arcus cosine
//
//*****************************************************************************
inline
#ifndef __BCPLUSPLUS__
float g_acosf( float a_fAngle )
{
	return ::acosf( a_fAngle );
}
#else
float g_acosf( float a_fAngle )
{
	return static_cast<float> (::acos( static_cast<double> (a_fAngle) ));
}
#endif

//*****************************************************************************
//	g_atan2f
//*****************************************************************************
//
//! \brief	Returns the arcus tangens of the specified angle
//! \param	[in] The angle whose arcus tangens to retrieve
//! \return	The angle's arcus tangens
//
//*****************************************************************************
inline
#ifndef __BCPLUSPLUS__
float g_atan2f( float a_fAngle1, float a_fAngle2 )
{
	return ::atan2f( a_fAngle1, a_fAngle2 );
}
#else
float g_atan2f( float a_fAngle1, float a_fAngle2 )
{
	return static_cast<float> (::atan2( static_cast<double> (a_fAngle1), static_cast<double> (a_fAngle2) ));
}
#endif

//*****************************************************************************
//	g_sqrtf
//*****************************************************************************
//
//! \brief	Returns the square root of the specified angle
//! \param	[in] The angle whose square root to retrieve
//! \return	The angle's square root
//
//*****************************************************************************
inline
#ifndef __BCPLUSPLUS__
float g_sqrtf( float a_fAngle )
{
	return ::sqrtf( a_fAngle );
}
#else
float g_sqrtf( float a_fAngle )
{
	return static_cast<float> (::sqrt( static_cast<double> (a_fAngle) ));
}
#endif

//*****************************************************************************
//	g_fCurve
//*****************************************************************************
//
//! \brief	Determines a spherically interpolated value
//! \param	[in] The value to interpolate
//! \return	The spherically interpolated value
//
//*****************************************************************************
inline	
float g_fCurve( float fValue )														
{	
	return ( fValue * fValue * ( 3.0f - 2.0f * fValue ) ); 
}

//*****************************************************************************
//	g_fRandom
//*****************************************************************************
//
//! \brief	Generates a random value within the specified boundaries
//! \param	[in] Minimum value
//! \param	[in] Maximum value
//! \return	The random value
//
//*****************************************************************************
inline
float g_fRandom( float a_fMin, float a_fMax )
{
	return ( a_fMin + ( ( a_fMax - a_fMin ) * ( static_cast< float > ( ::rand() ) / RAND_MAX ) ) );
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif  // BASICS_H_INCLUDED
