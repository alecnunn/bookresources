/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cHost.cpp: implementation of the cHost class.
//
//////////////////////////////////////////////////////////////////////

#include "cHost.h"
#include "Macros.h"
#include <assert.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cHost::cHost() : cMonitor()
{
  unsigned int i;

  for( i = 0; i < PING_RECORDLENGTH; i++ )
  {
    d_pingLink[ i ] = PING_DEFAULTVALLINK;
    d_pingTrans[ i ] = PING_DEFAULTVALTRANS;
  }
  d_ackLength = 0;
  d_bClockTimeSet = false;
}


cHost::~cHost()
{
}


void cHost::SetPort( unsigned short port )
{
  d_port = port;
}


bool cHost::SetAddress( char *pAddress )
{
  if( pAddress == NULL )
    return true;

  IN_ADDR *pAddr;
  HOSTENT *pHe;

  pHe = gethostbyname( pAddress );
  if( pHe == NULL )
    return true;

  pAddr = (in_addr *)pHe->h_addr_list[ 0 ];
  d_address = ntohl( pAddr->s_addr );

  return false;
}


bool cHost::SetAddress( DWORD address )
{
  d_address = address;

  return false;
}


DWORD cHost::GetLastClockTime()
{
  return d_lastClockTime;
}


void cHost::SetLastClockTime( DWORD time )
{
  d_lastClockTime = time;
  d_bClockTimeSet = true;
}
////////////////////////////////////////////////////////////////////////////////

// For qsort
static int sSortPing( const void *arg1, const void *arg2 )
{
  if( *(DWORD *)arg1 < *(DWORD *)arg2 )
    return -1;
  if( *(DWORD *)arg1 > *(DWORD *)arg2 )
    return 1;
  return 0;
}


float cHost::GetAverageLinkPing( float bestPercentage )
{
  if( bestPercentage <= 0.0f ||
      bestPercentage > 100.0f )
    bestPercentage = 100.0f;

  DWORD pings[ PING_RECORDLENGTH ];
  float sum, worstFloat;
  int   worst, i;

  // Recalculate the ping list
  memcpy( pings, &d_pingLink, PING_RECORDLENGTH * sizeof( DWORD ) );
  qsort( pings, PING_RECORDLENGTH, sizeof( DWORD ), sSortPing );

  // Average the first bestPercentage / 100.
  worstFloat = (float)PING_RECORDLENGTH * bestPercentage / 100.0f;
  worst = (int)worstFloat + ( ( worstFloat - (int)worstFloat ) != 0 );
  sum = 0.0f;
  for( i = 0; i < worst; i++ )
    sum += pings[ i ];

  return sum / (float)worst;
}


float cHost::GetAverageTransPing( float bestPercentage )
{
  if( bestPercentage <= 0.0f ||
      bestPercentage > 100.0f )
    bestPercentage = 100.0f;

  DWORD pings[ PING_RECORDLENGTH ];
  float sum, worstFloat;
  int   worst, i;

  // Recalculate the ping list
  memcpy( pings, &d_pingTrans, PING_RECORDLENGTH * sizeof( DWORD ) );
  qsort( pings, PING_RECORDLENGTH, sizeof( DWORD ), sSortPing );

  // Average the first bestPercentage / 100.
  worstFloat = (float)PING_RECORDLENGTH * bestPercentage / 100.0f;
  worst = (int)worstFloat + ( ( worstFloat - (int)worstFloat ) != 0 );
  sum = 0.0f;
  for( i = 0; i < worst; i++ )
    sum += pings[ i ];

  return sum / (float)worst;
}


void cHost::ACKPacket( DWORD packetID, DWORD receiveTime )
{
  cDataPacket *pPacket;

  pPacket = d_outQueue.BorrowPacket( packetID );
  if( pPacket == NULL )
    return;

  DWORD         time;
  unsigned int  i;

  time = receiveTime - pPacket->d_firstTime;
  d_outQueue.ReturnPacket();

  if( pPacket->d_timesSent == 1 )
  {
    for( i = 0; i < PING_RECORDLENGTH - 1; i++ )
      d_pingLink[ i ] = d_pingLink[ i + 1 ];
    d_pingLink[ i ] = time;
  }
  for( i = 0; i < PING_RECORDLENGTH - 1; i++ )
    d_pingTrans[ i ] = d_pingTrans[ i + 1 ];
  d_pingTrans[ i ] = time;

  d_outQueue.RemovePacket( packetID );
}


unsigned short cHost::ProcessIncomingACKs( char *pBuffer, unsigned short len, DWORD receiveTime )
{
  // Get the number of ACKs in this message, not counting the base ACK.
  unsigned char numAcks, mask, *ptr;
  DWORD         basePacketID, ackID;

  ptr = (unsigned char *)pBuffer;

  // Get the base packet ID, which indicates all the ordered packets received so far.
  memcpy( &basePacketID, ptr, sizeof( DWORD ) );
  ptr += sizeof( DWORD );
  // Get the number of additional ACKs.
  numAcks = *ptr;
  ptr++;
  ackID = d_outQueue.GetLowestID();

#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT3( "<   Ack low=%04d base=%04d end=%04d\n", ackID, basePacketID, basePacketID + numAcks );
#endif

  while( ackID < basePacketID )
  {
    ACKPacket( ackID, receiveTime );
    ackID++;
  }

  mask = 0x80;

  while( ackID < basePacketID + numAcks )
  {
    if( mask == 0 )
    {
      mask = 0x80;
      ptr++;
    }

    if( ( *ptr & mask ) != 0 )
      ACKPacket( ackID, receiveTime );

    mask >>= 1;
    ackID++;
  }

  return ( ptr - (unsigned char *)pBuffer ) + ( mask != 0 );
}


unsigned short cHost::ProcessIncomingReliable( char *pBuffer, unsigned short maxLen, DWORD receiveTime )
{
  // Process any messages in the packet.
  DWORD           packetID;
  char            *readPtr;
  unsigned short  length;

  readPtr = pBuffer;
  memcpy( &packetID, readPtr, sizeof( DWORD ) );
  readPtr += sizeof( DWORD );
  memcpy( &length, readPtr, sizeof( unsigned short ) );
  readPtr += sizeof( unsigned short );
#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT2( "<   %04d (%d) R\n", packetID, length );
#endif
  // If this message is a packet, queue the data
  // to be dealt with by the application later.
  d_inQueue.AddPacket( packetID, (char *)readPtr, length, receiveTime );
  readPtr += length;


  // Should we build an ACK message?
  if( d_inQueue.GetCount() == 0 )
    return ( readPtr - pBuffer );


  // Build the new ACK message.
  DWORD         lowest, highest, ackID;
  unsigned char mask, *ptr;

  lowest = d_inQueue.GetCurrentID();
  highest = d_inQueue.GetHighestID();

  // Cap the highest so as not to overflow the ACK buffer
  // (or spend too much time building ACK messages).
  if( highest > lowest + ACK_MAXPERMSG )
    highest = lowest + ACK_MAXPERMSG;

#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT2( " >  %04d ack to %04d ", lowest, highest );
#endif

  ptr = (unsigned char *)d_ackBuffer;
  // Send the base packet ID, which is the ID of the last ordered packet received.
  memcpy( ptr, &lowest, sizeof( DWORD ) );
  ptr += sizeof( DWORD );
  // Add the number of additional ACKs.
  *ptr = highest - lowest;
  ptr++;

  ackID = lowest;
  mask = 0x80;

  while( ackID < highest )
  {
    if( mask == 0 )
    {
      mask = 0x80;
      ptr++;
    }

    // Is there a packet with id 'i' ?
    if( d_inQueue.UnorderedPacketIsQueued( ackID ) == true )
    {
#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT0( "1" );
#endif
      *ptr |= mask;  // There is
    }
    else
    {
#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT0( "0" );
#endif
      *ptr &= ~mask;  // There isn't
    }

    mask >>= 1;
    ackID++;
  }

#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT0( "\n" );
#endif

  // Record the ammount of the ackBuffer used.
  d_ackLength = ( ptr - (unsigned char *)d_ackBuffer ) + ( mask != 0 );

  // return the number of bytes read from 
  return ( readPtr - pBuffer );
}


unsigned short cHost::ProcessIncomingUnreliable( char *pBuffer, unsigned short maxLen, DWORD receiveTime )
{
  // Process any messages in the packet.
  DWORD           packetID;
  char            *readPtr;
  unsigned short  length;

  readPtr = pBuffer;
  memcpy( &packetID, readPtr, sizeof( DWORD ) );
  readPtr += sizeof( DWORD );
  memcpy( &length, readPtr, sizeof( unsigned short ) );
  readPtr += sizeof( unsigned short );
#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT2( "<   %04d (%d) U\n", packetID, length );
#endif
  // If this message is a packet, queue the data
  // to be dealt with by the application later.
  d_unreliableInQueue.AddPacket( packetID, (char *)readPtr, length, receiveTime );
  readPtr += length;

  // return the number of bytes read from 
  return ( readPtr - pBuffer );
}


unsigned short cHost::AddACKMessage( char *pBuffer, unsigned short maxLen )
{
  if( d_ackLength == 0 )
    return 0;

  unsigned short len;

  len = d_ackLength < maxLen ? d_ackLength : maxLen;

  memcpy( pBuffer, d_ackBuffer, len );

  return len;
}
