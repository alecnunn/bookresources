// Macros.h - macros, defines, typedefs, etc.
//
//////////////////////////////////////////////////////////////////////

#if !defined( __MACROS_H )
#define __MACROS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined( _CONSOLE )

#define OUTPUTREPORT0( a )            { printf( a ); }
#define OUTPUTREPORT1( a, b )         { printf( a, b ); }
#define OUTPUTREPORT2( a, b, c )      { printf( a, b, c ); }
#define OUTPUTREPORT3( a, b, c, d )   { printf( a, b, c, d ); }

#else // !defined( _CONSOLE )

#define OUTPUTREPORT0( a ) \
{ \
  OutputDebugString( a ); \
}


#define OUTPUTREPORT1( a, b ) \
{ \
  char outputBuffer[ 1000 ]; \
  sprintf( outputBuffer, a, b ); \
  OutputDebugString( outputBuffer ); \
}


#define OUTPUTREPORT2( a, b, c ) \
{ \
  char outputBuffer[ 1000 ]; \
  sprintf( outputBuffer, a, b, c ); \
  OutputDebugString( outputBuffer ); \
}


#define OUTPUTREPORT3( a, b, c, d ) \
{ \
  char outputBuffer[ 1000 ]; \
  sprintf( outputBuffer, a, b, c, d ); \
  OutputDebugString( outputBuffer ); \
}

#endif  // defined( _CONSOLE )

#if defined( _DEBUG )

// printf() a lot of information
#define _DEBUG_VERBOSE

// Drop 1 in _DEBUG_DROPTEST random packets to see how the system handles it.
// Undefine _DEBUG_DROPTEST to turn it off.  _DEBUG_DROPTEST > 1.
//#define _DEBUG_DROPTEST   4

// Queue an outgoing message for at least _DEBUG_PINGTEST ms
// Undefine _DEBUG_PINGTEST to turn it off.  _DEBUG_PINGTEST > 0.
//#define _DEBUG_PINGTEST   100

// printf() some ping information
//#define _DEBUG_TIMING

// Clock synch testing information
//#define _DEBUG_CLOCK

#endif  // !defined( _DEBUG )


// Convert bits to bytes.
#define BYTESREQUIRED( x )  ( ( x >> 3 ) + ( x & 0x7 != 0 ) )


#endif