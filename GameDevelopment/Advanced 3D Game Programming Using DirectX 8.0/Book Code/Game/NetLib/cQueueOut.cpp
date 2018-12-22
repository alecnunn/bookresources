// cQueueOut.cpp: implementation of the cQueueOut class.
//
//////////////////////////////////////////////////////////////////////

#include "Macros.h"
#include "cQueueOut.h"
#include "cNetError.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cQueueOut::cQueueOut()
{
  d_currentPacketID = 0;
  d_count = 0;
}


cQueueOut::~cQueueOut()
{
  Clear();
}


void cQueueOut::Clear()
{
  cMonitor::MutexOn();

  d_packets.clear();

  cMonitor::MutexOff();
}


void cQueueOut::AddPacket( char *pData, unsigned short len )
{
  if( pData == NULL ||
      len == 0 )
    return;

  // Create the packet.
  cDataPacket *pPacket;

  pPacket = new cDataPacket;
  if( pPacket == NULL )
    throw cNetError( "cQueueOut::AddPacket() - insufficient memory." );

  pPacket->Init( GetTickCount(), d_currentPacketID, len, pData );
  d_currentPacketID++;
  d_count++;

  // Add the packet to the queues.
  cMonitor::MutexOn();

  d_packets.push_back( pPacket );

  cMonitor::MutexOff();
}


void cQueueOut::RemovePacket( DWORD packetID )
{
  list<cDataPacket *>::iterator iPacket;

  cMonitor::MutexOn();

  for( iPacket = d_packets.begin(); iPacket != d_packets.end(); ++iPacket )
  {
    if( (*iPacket)->d_id == packetID )
      break;
  }

  if( iPacket != d_packets.end() )
  {
    d_packets.erase( iPacket );
	// This line was causing crashes on some people's machines...
	//delete *iPacket;
  }

  cMonitor::MutexOff();
}


DWORD cQueueOut::GetLowestID()
{
  DWORD packetID;

  cMonitor::MutexOn();

  if( d_packets.empty() == true )
    packetID = d_currentPacketID;
  else
    packetID = ( *d_packets.begin() )->d_id;

  cMonitor::MutexOff();

  return packetID;
}


bool cQueueOut::IsEmpty()
{
  return d_packets.empty();
}

////////////////////////////////////////////////////////////////////////////////

bool cQueueOut::GetPacketForResend( DWORD waitTime, cDataPacket *pPacket )
{
  if( pPacket == NULL ||
      d_packets.empty() == true )
    return false;

  cDataPacket *pFirstPacket;

  cMonitor::MutexOn();

  pFirstPacket = *d_packets.begin();

  if( GetTickCount() > pFirstPacket->d_lastTime + waitTime )
  {
    // Packet found.
    *pPacket = *pFirstPacket;
    pFirstPacket->d_lastTime = GetTickCount();
    pFirstPacket->d_timesSent++;

    cMonitor::MutexOff();
    return true;
  }

  cMonitor::MutexOff();
  return false;
}


bool cQueueOut::GetPreviousPacket( DWORD packetID, cDataPacket *pPacket )
{
  if( pPacket == NULL ||
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

// Do not remove a packet from the queue, but get the packet.
// ReturnPacket() MUST be called or the mutex will not be released (BAD!)
// Also: do NOT delete the packet that you borrow (BAD!)
cDataPacket *cQueueOut::BorrowPacket( DWORD packetID )
{
  if( d_packets.empty() )
    return NULL;

  list<cDataPacket *>::iterator iPacket;

  cMonitor::MutexOn();

  for( iPacket = d_packets.begin(); iPacket != d_packets.end(); ++iPacket )
  {
    if( (*iPacket)->d_id == packetID )
      break;
  }

  if( iPacket != d_packets.end() )
    return *iPacket;

  // Packet not found in list.
  cMonitor::MutexOff();

  return NULL;
}


void cQueueOut::ReturnPacket()
{
  cMonitor::MutexOff();
}

