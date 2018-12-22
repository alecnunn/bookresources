/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#if !defined( __DATAEXTRACT_H )
#define __DATAEXTRACT_H
////////////////////////////////////////////////////////////////////////////////

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <memory.h>
#include <assert.h>
#include "cDataPacket.h"
////////////////////////////////////////////////////////////////////////////////

class cDataExtract
{
protected:
  char  d_dataBuffer[ MAX_UDPBUFFERSIZE ];
  int   d_currByte,
        d_currBit,
        d_numBits;

public:
  cDataExtract()
  {
    ClearBuffer();
  }


  virtual ~cDataExtract()
  {
  }


  bool SetBuffer( char *pBuffer, int numBits )
  {
    if( !pBuffer )
      return true;

    memcpy( d_dataBuffer, pBuffer, ( numBits >> 3 ) + ( numBits & 0x7 != 0 ) );
    d_currByte = 0;
    d_currBit = 0;
    d_numBits = numBits;

    return false;
  }


  // This may not match the version in the book exactly.
  // (I optimized this quite a bit)
  unsigned long int GetNBits( int numBits, bool *pSuccessFlag )
  {
    assert( numBits <= 32 );

    DWORD         value, destMask;
    char          *pBuffer;
    unsigned char srcMask;
    int           i;

    value = 0;
    i = d_currBit;
    pBuffer = d_dataBuffer + d_currByte;

    // We don't need d_currByte, d_currBit again so increment
    // them and make sure that the data acutally exists in the buffer.

    d_currBit += numBits;
    if( d_currBit >= 8 )
    {
      d_currByte += d_currBit >> 3;
      d_currBit &= 0x7;
    }
    if( GetCurrentPos() > d_numBits )
    {
      // uh oh!
      if( pSuccessFlag )
        *pSuccessFlag = true;
      return 0;
    }
    else
    {
      // everything will be just fine...
      if( pSuccessFlag )
        *pSuccessFlag = false;
    }

    srcMask = 1 << ( 7 - i );
    destMask = 1 << ( numBits - 1 );

    // Get the value - easy way
    while( numBits )
    {
      if( *pBuffer & srcMask )
        value |= destMask;
      destMask >>= 1;
      numBits--;
      i++;
      if( i >= 8 )
      {
        i -= 8;
        pBuffer++;
        srcMask = 1 << 7;
      }
      else
        srcMask >>= 1;
    }

    return value;
  }


  unsigned char Get1Byte( bool *pSuccessFlag )
  {
    return (unsigned char)GetNBits( 8, pSuccessFlag );
  }


  unsigned short int Get2Bytes( bool *pSuccessFlag )
  {
    bool                success;
    unsigned char       c1, c2;
    unsigned short int  data;

    data = (unsigned short int)GetNBits( 16, &success );
    if( success )
    {
      if( pSuccessFlag )
        *pSuccessFlag = true;
      return 0;
    }
    c1 = (unsigned char)( data & 0xff );
    c2 = (unsigned char)( ( data >> 8 ) & 0xff );
    data = ( c1 << 8 ) + c2;

    return data;
  }


  unsigned long int Get4Bytes( bool *pSuccessFlag )
  {
    unsigned char      c1, c2, c3, c4;
    bool               success;
    unsigned long int  data;

    data = (unsigned long int)GetNBits( 32, &success );
    if( success )
    {
      if( pSuccessFlag )
        *pSuccessFlag = true;
      return 0;
    }
    c1 = (unsigned char)( data & 0xff );
    c2 = (unsigned char)( ( data >> 8 ) & 0xff );
    c3 = (unsigned char)( ( data >> 16 ) & 0xff );
    c4 = (unsigned char)( ( data >> 24 ) & 0xff );
    data = ( c1 << 24 ) + ( c2 << 16 ) + ( c3 << 8 ) + c4;

    return data;
  }

/*
  u64 Get8Bytes( bool *pSuccessFlag )
  {
    char c1, c2, c3, c4, c5, c6, c7, c8;
    bool success;
    u64  data;

    data = (u64)GetNBits( 32, &success );
    if( success )
    {
      if( pSuccessFlag )
        *pSuccessFlag = true;
      return 0;
    }

    c1 = (char)( data & 0xff );
    c2 = (char)( ( data >>  8 ) & 0xff );
    c3 = (char)( ( data >> 16 ) & 0xff );
    c4 = (char)( ( data >> 24 ) & 0xff );
    c5 = (char)( ( data >> 32 ) & 0xff );
    c6 = (char)( ( data >> 40 ) & 0xff );
    c7 = (char)( ( data >> 48 ) & 0xff );
    c8 = (char)( ( data >> 56 ) & 0xff );
    data = ( c1 << 56 ) + ( c2 << 48 ) + ( c3 << 40 ) + ( c4 << 32 ) +
           ( c5 << 24 ) + ( c6 << 16 ) + ( c7 << 8 ) + c8;

    return data;
  }
*/

  float GetFloat( bool *pSuccessFlag )
  {
    bool              success;
    unsigned long int data;

    data = GetNBits( 32, &success );
    if( success )
    {
      if( pSuccessFlag )
        *pSuccessFlag = true;
      return 0.0f;
    }

    // Woo!  Typecasting silliness!
    return *(float *)( &data );
  }


  bool GetString( char *pString, int maxLength )
  {
    bool           success;
    int            count;
    unsigned char  data;

    if( !pString )
      return true;

    count = 0;
    do
    {
      data = (unsigned char)GetNBits( 8, &success );
      if( success )
        return true;
      pString[ count++ ] = data;
    }
    while( count != maxLength && data );

    return false;
  }


  char *GetBuffer()
  {
    return d_dataBuffer;
  }


  // returns the length of the buffer, in bits.
  int GetCurrentPos()
  {
    return ( d_currByte << 3 ) + d_currBit;
  }


  int GetBufferLengthInBits()
  {
    return d_numBits;
  }


  int GetBufferLengthInBytes()
  {
    return ( d_numBits >> 3 ) + ( d_numBits & 0x7 != 0 );
  }


  void ClearBuffer()
  {
    memset( d_dataBuffer, 0, sizeof( d_dataBuffer ) );
    d_currBit = 0;
    d_currByte = 0;
    d_numBits = 0;
  }


  cDataExtract &operator=( cDataExtract &data )
  {
    memcpy( d_dataBuffer, data.d_dataBuffer, data.GetBufferLengthInBytes() );
    d_currBit = 0;
    d_currByte = 0;
    d_numBits = data.d_numBits;

    return *this;
  }


  cDataExtract &operator+=( cDataExtract &data )
  {
    memcpy( d_dataBuffer, data.d_dataBuffer, data.GetBufferLengthInBytes() );
    d_currBit = data.d_currBit;
    d_currByte = data.d_currByte;
    d_numBits = data.d_numBits;

    return *this;
  }
};


#endif

