/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cHost.h: interface for the cHost class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHOST_H__4A115C53_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_CHOST_H__4A115C53_D1D4_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include "cQueueIn.h"
#include "cQueueOut.h"
#include "cUnreliableQueueIn.h"
#include "cUnreliableQueueOut.h"
#include "cMonitor.h"


#define ACK_MAXPERMSG           256
#define ACK_BUFFERLENGTH        48

#define PING_RECORDLENGTH       64
#define PING_DEFAULTVALLINK     150
#define PING_DEFAULTVALTRANS    200


class cHost : public cMonitor
{
  DWORD               d_address;
  unsigned short      d_port;
  cQueueIn            d_inQueue;
  cQueueOut           d_outQueue;
  cUnreliableQueueIn  d_unreliableInQueue;
  cUnreliableQueueOut d_unreliableOutQueue;

  // Ping records
  DWORD d_pingLink[ PING_RECORDLENGTH ],
        d_pingTrans[ PING_RECORDLENGTH ];

  // For clock synchronization
  DWORD d_lastClockTime;
  bool  d_bClockTimeSet;

  // A buffer of the latest ACK message for this host
  char            d_ackBuffer[ ACK_BUFFERLENGTH ];
  unsigned short  d_ackLength;  // ammount of the buffer actually used.


  void ACKPacket( DWORD packetID, DWORD receiveTime );

public:
	cHost();
	virtual ~cHost();

  float           GetAverageLinkPing( float percent );
  float           GetAverageTransPing( float percent );
  unsigned short  ProcessIncomingACKs( char *pBuffer, unsigned short len, DWORD receiveTime );
  unsigned short  ProcessIncomingReliable( char *pBuffer, unsigned short len, DWORD receiveTime );
  unsigned short  ProcessIncomingUnreliable( char *pBuffer, unsigned short len, DWORD receiveTime );
  unsigned short  AddACKMessage( char *pBuffer, unsigned short maxLen );

  void  SetPort( unsigned short port );
  bool  SetAddress( char *pAddress );
  bool  SetAddress( DWORD address );

  DWORD GetLastClockTime();
  void  SetLastClockTime( DWORD time );

  
  bool  WasClockTimeSet()
  {
    return d_bClockTimeSet;
  }


  DWORD GetAddress()
  {
    return d_address;
  }


  unsigned short GetPort()
  {
    return d_port;
  }


  cQueueIn &GetInQueue()
  {
    return d_inQueue;
  }


  cQueueOut &GetOutQueue()
  {
    return d_outQueue;
  }


  cUnreliableQueueIn &GetUnreliableInQueue()
  {
    return d_unreliableInQueue;
  }


  cUnreliableQueueOut &GetUnreliableOutQueue()
  {
    return d_unreliableOutQueue;
  }
};

#endif // !defined(AFX_CHOST_H__4A115C53_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
