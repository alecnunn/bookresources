/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#if !defined( __DATACOMPRESS_H )
#define __DATACOMPRESS_H
////////////////////////////////////////////////////////////////////////////////

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <string.h>
#include "cDataPacket.h"


#define SIZEOFU32  ( sizeof( DWORD ) * 8 )


class cDataCompress
{
protected:
  char  d_dataBuffer[ MAX_UDPBUFFERSIZE ];
  int   d_currByte,
        d_currBit,
        d_numBits;

public:
  cDataCompress()
  {
    ClearBuffer();
  }


  virtual ~cDataCompress()
  {
  }


  bool SetBuffer( char *pBuffer, int numBits )
  {
    if( !pBuffer ) return true;

    memcpy( d_dataBuffer, pBuffer, ( numBits >> 3 ) + ( numBits & 0x7 != 0 ) );
    d_currByte = 0;
    d_currBit = 0;
    d_numBits = numBits;

    return false;
  }


  bool AddNBits( int numBits, unsigned long int data )
  {
    assert( numBits <= 32 );

    int   spaceInByte, toAlign;
    char  *pBuffer;

    data <<= SIZEOFU32 - numBits;

//    assert( data & ( ( 1 << numBits ) - 1 ) == 0 );

    spaceInByte = 8 - d_currBit;

    // advance the bit/byte count, we won't be touching it again until the end.
    pBuffer = d_dataBuffer + d_currByte;
    d_currBit += numBits;
    if( d_currBit >= 8 )
    {
      d_currByte += d_currBit / 8;
      d_currBit %= 8;
    }
    if( d_currByte > sizeof( d_dataBuffer ) )
      return true;

    // Try to align the bits to the byte boundary to speed up compression
    if( spaceInByte )
    {
      // Are there enough bits being written to fill the byte?
      if( numBits < spaceInByte ) toAlign = numBits;
      else toAlign = spaceInByte;

      *pBuffer++ |= (char)( data >> ( SIZEOFU32 - spaceInByte ) );
      data <<= toAlign;
    }

    // Bits are aligned to word, copy a word at a time.
    while( data )
    {
      *pBuffer++ = (char)( data >> ( SIZEOFU32 - 8 ) );
      data <<= 8;
    }
    d_numBits += numBits;

    return false;
  }


  bool Add1Byte( unsigned char data )
  {
    return AddNBits( 8, data );
  }


  bool Add2Bytes( unsigned short int data )
  {
    char c1, c2;

    c1 = (unsigned char)( data & 0xff );
    c2 = (unsigned char)( ( data >> 8 ) & 0xff );
    data = ( c1 << 8 ) + c2;

    return AddNBits( 16, data );
  }


  bool Add4Bytes( unsigned long int data )
  {
    char c1, c2, c3, c4;

    c1 = (char)( data & 0xff );
    c2 = (char)( ( data >> 8 ) & 0xff );
    c3 = (char)( ( data >> 16 ) & 0xff );
    c4 = (char)( ( data >> 24 ) & 0xff );
    data = ( c1 << 24 ) + ( c2 << 16 ) + ( c3 << 8 ) + c4;

    return AddNBits( 32, data );
  }

/*
  bool Add8Bytes( u64 data ) {
    char c1, c2, c3, c4, c5, c6, c7, c8;

    c1 = (char)( data & 0xff );
    c2 = (char)( ( data >>  8 ) & 0xff );
    c3 = (char)( ( data >> 16 ) & 0xff );
    c4 = (char)( ( data >> 24 ) & 0xff );
    c5 = (char)( ( data >> 32 ) & 0xff );
    c6 = (char)( ( data >> 40 ) & 0xff );
    c7 = (char)( ( data >> 48 ) & 0xff );
    c8 = (char)( ( data >> 56 ) & 0xff );
    data = ( c1 << 24 ) + ( c2 << 16 ) + ( c3 << 8 ) + c4;

    return AddNBits( 32, data );
  }
*/


  // Is this endian friendly?  I can't seem to find any info on how other chips
  // store floats...grr!
  bool AddFloat( float value )
  {
    unsigned long int data;

    // Woo!  Typecasting silliness!
    data = *( (unsigned long int *)( &value ) );

    return AddNBits( 32, data );
  }

/*
  // Is this endian friendly?  I can't seem to find any info on how other chips
  // store floats...grr!
  bool AddDouble( double value )
  {
    return true;
  }
*/

  bool AddString( const char *pString )
  {
    int i;

    if( !pString )
    {
      if( AddNBits( 8, 0 ) )
        return true;
    }
    else
    {
      i = -1;
      do
      {
        i++;
        if( AddNBits( 8, pString[ i ] ) )
          return true;
      }
      while( pString[ i ] );
    }

    return false;
  }


  char *GetBuffer()
  {
    return d_dataBuffer;
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


  cDataCompress &operator=( cDataCompress &data )
  {
    memcpy( d_dataBuffer, data.d_dataBuffer, data.GetBufferLengthInBytes() );
    d_currBit = 0;
    d_currByte = 0;
    d_numBits = data.d_numBits;

    return *this;
  }


  cDataCompress &operator+=( cDataCompress &data )
  {
    int total, i;

    total = data.GetBufferLengthInBytes();

    for( i = 0; i < total; i++ )
      Add1Byte( data.d_dataBuffer[ i ] );

    AddNBits( data.d_numBits - ( total << 3 ), data.d_dataBuffer[ total ] );

    return *this;
  }
};


#endif

