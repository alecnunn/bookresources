// cUnreliableQueueOut.cpp: implementation of the cUnreliableQueueOut class.
//
//////////////////////////////////////////////////////////////////////

#include "cUnreliableQueueOut.h"
#include "cNetError.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cUnreliableQueueOut::cUnreliableQueueOut() : cMonitor()
{
  d_currentPacketID = 0;
  d_maxPackets = 2;
  d_numPackets = 0;
}


cUnreliableQueueOut::~cUnreliableQueueOut()
{
  Clear();
}


void cUnreliableQueueOut::Clear()
{
  cMonitor::MutexOn();

  d_packets.clear();

  cMonitor::MutexOff();
}


void cUnreliableQueueOut::AddPacket( char *pData, unsigned short len )
{
  if( pData == NULL ||
      len == 0 )
    return;

  // Create the packet.
  cDataPacket *pPacket;

  pPacket = new cDataPacket;
  if( pPacket == NULL )
    throw cNetError( "cUnreliableQueueOut::AddPacket() - insufficient memory." );

  pPacket->Init( GetTickCount(), d_currentPacketID, len, pData );
  d_currentPacketID++;

  // Add the packet to the queue.
  cMonitor::MutexOn();

  if( d_numPackets == d_maxPackets )
  {
    delete *d_packets.begin();
    d_packets.pop_front();
  }
  else
    d_numPackets++;

  d_packets.push_back( pPacket );

  cMonitor::MutexOff();
}


void cUnreliableQueueOut::SetMaxPackets( unsigned char maxPackets )
{
  if( maxPackets < d_maxPackets )
  {
    cMonitor::MutexOn();

    while( d_numPackets > maxPackets )
    {
      delete *d_packets.begin();
      d_packets.pop_front();
      d_numPackets--;
    }

    cMonitor::MutexOff();
  }

  d_maxPackets = maxPackets;
}

////////////////////////////////////////////////////////////////////////////////

bool cUnreliableQueueOut::GetPreviousPacket( DWORD packetID, cDataPacket *pPacket )
{
  if( pPacket == NULL ||
      d_numPackets == 0 ||
      d_packets.empty() == true )
    return false;

  list<cDataPacket *>::iterator iPacket;

  cMonitor::MutexOn();

  for( iPacket = d_packets.begin(); iPacket != d_packets.end(); ++iPacket )
  {
    if( (*iPacket)->d_id >= packetID )
      break;
  }

  if( iPacket != d_packets.begin() )
  {
    // Packet found.
    --iPacket;
    *pPacket = **iPacket;

    cMonitor::MutexOff();
    return true;
  }

  // Previous packet not found, either it was ACKed or it never existed.
  cMonitor::MutexOff();
  return false;
}

