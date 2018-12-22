/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cQueueIn.cpp: implementation of the cQueueIn class.
//
//////////////////////////////////////////////////////////////////////

#include "cQueueIn.h"
#include "cNetError.h"
#include "Macros.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cQueueIn::cQueueIn() : cMonitor()
{
  d_currentPacketID = 0;
  d_count = 0;
}


cQueueIn::~cQueueIn()
{
  Clear();
}


void cQueueIn::Clear()
{
  cMonitor::MutexOn();

  d_packetsUnordered.clear();
  d_packetsOrdered.clear();

  cMonitor::MutexOff();
}


void cQueueIn::AddPacket( DWORD packetID, char *pData, unsigned short len, DWORD receiveTime )
{
  if( pData == NULL ||
      len == 0 )
    return;

  if( d_currentPacketID > packetID )
    return;

  // Create the packet.
  cDataPacket *pPacket;

  pPacket = new cDataPacket;
  if( pPacket == NULL )
    throw cNetError( "cQueueIn::AddPacket() - insufficient memory." );

  pPacket->Init( receiveTime, packetID, len, pData );


  // Add the packet to the queues.
  cMonitor::MutexOn();

  if( d_currentPacketID == pPacket->d_id )
  {
    // This packet is the next ordered packet.
    // Add it to the ordered list and then move all
    // unordered that can be moved to the ordered list.
    d_packetsOrdered.push_back( pPacket );
    d_currentPacketID++;
    d_count++;

    pPacket = *d_packetsUnordered.begin();

    while( d_packetsUnordered.empty() == false &&
           d_currentPacketID == pPacket->d_id )
    {
      d_packetsUnordered.pop_front();
      d_packetsOrdered.push_back( pPacket );
      d_currentPacketID++;
      pPacket = *d_packetsUnordered.begin();
    }
  }
  else  // d_currentPacketID > pPacket->d_id
  {
    // Out of order.  Sort into the list.
    list<cDataPacket *>::iterator iPacket;
    bool                          bExists;

    bExists = false;

    for( iPacket = d_packetsUnordered.begin(); iPacket != d_packetsUnordered.end(); ++iPacket )
    {
      // Already in list - get out now!
      if( (*iPacket)->d_id == pPacket->d_id )
      {
        bExists = true;
        break;
      }
      if( (*iPacket)->d_id > pPacket->d_id )
        break;
    }

    if( bExists == true )
      delete pPacket;
    else
    {
      // We've gone 1 past the spot where pPacket belongs.  Back up and insert.
      d_packetsUnordered.insert( iPacket, pPacket );
      d_count++;
    }
  }

  cMonitor::MutexOff();
}


cDataPacket *cQueueIn::GetPacket()
{
  if( d_packetsOrdered.empty() == true )
    return NULL;

  cDataPacket *pPacket;

  cMonitor::MutexOn();

  pPacket = *d_packetsOrdered.begin();
  d_packetsOrdered.pop_front();

  cMonitor::MutexOff();

  return pPacket;
}


bool cQueueIn::UnorderedPacketIsQueued( DWORD packetID )
{
  bool                          bIsQueued;
  list<cDataPacket *>::iterator iPacket;

  bIsQueued = false;

  cMonitor::MutexOn();

  for( iPacket = d_packetsUnordered.begin(); iPacket != d_packetsUnordered.end(); ++iPacket )
  {
    // Already in list - get out now!
    if( (*iPacket)->d_id == packetID )
    {
      bIsQueued = true;
      break;
    }
  }

  cMonitor::MutexOff();

  return bIsQueued;
}


DWORD cQueueIn::GetHighestID()
{
  list<cDataPacket *>::iterator iPacket;

  iPacket = d_packetsUnordered.end();
  if( iPacket == d_packetsUnordered.begin() )
    return d_currentPacketID;
  else
  {
    --iPacket;
    return (*iPacket)->d_id + 1;
  }
}

