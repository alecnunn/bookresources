/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// MTUDP.h: interface for the MTUDP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MTUDP_H__B0419AC9_D198_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_MTUDP_H__B0419AC9_D198_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Shut off the "truncated to 255 characters" warning message.
#pragma warning( disable : 4786 )


#include <winsock2.h>
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include "cThread.h"
#include "cHost.h"
#include "cNetClock.h"
#include <list>
#include <map>
using namespace std;


// Host handles, used by non-comm class methods rather than return a pointer
typedef DWORD   HOSTHANDLE;


class MTUDP : public cThread
{
  typedef map<HOSTHANDLE, cHost *> HOSTMAP;
  typedef list<cHost *>            HOSTLIST;

protected:
  SOCKET          d_listenSocket,
                  d_sendSocket;
  bool            d_bRunning,
                  d_bListening,
                  d_bSending,
                  d_bIsServerOn,
                  d_bIsClientOn;
  unsigned short  d_localListenPort,
                  d_foreignListenPort;
  HOSTLIST        d_hosts;
  HOSTMAP         d_hostMap;
  cNetClock       d_clock;
  DWORD           d_bytesTransfered;

	void            ResendData( cDataPacket *pPacket, cHost *pHost );
  void            ProcessIncomingData( char *pData, unsigned short len, DWORD address, DWORD receiveTime );
  unsigned short  AddClockData( char *pData, unsigned short maxLen, cHost *pHost );
  unsigned short  ProcessIncomingClockData( char *pData, unsigned short len, cHost *pHost, DWORD receiveTime );

public:
  MTUDP();
	virtual ~MTUDP();

	void  Startup( unsigned short localListenPort, unsigned short ForeignListenPort );
	void  StartListening();
	void  StartSending();
	void  StopListening();
	void  StopSending();
	void  Cleanup();

  void  StartServer();
  void  StopServer();
  void  StartClient();
  void  StopClient();

	virtual DWORD ThreadProc();

	unsigned short  GetUnreliableData( char *pBuffer, unsigned short maxLen, HOSTHANDLE *pHostID );
	unsigned short  GetReliableData( char *pBuffer, unsigned short maxLen, HOSTHANDLE *pHostID );
  void            UnreliableSendTo( char *pStr, unsigned short len, HOSTHANDLE hostID );
  void            ReliableSendTo( char *pStr, unsigned short len, HOSTHANDLE hostID );
  void            ResendPackets();

  HOSTHANDLE      HostCreate( char *pAddress, unsigned short port );
  HOSTHANDLE      HostCreate( DWORD address, unsigned short port );
  void            HostDestroy( HOSTHANDLE hostID );
  float           HostGetAverageLinkPing( HOSTHANDLE hostID, float precent );
  float           HostGetAverageTransPing( HOSTHANDLE hostID, float precent );
  bool            HostGetIsOutEmpty( HOSTHANDLE hostID );
  unsigned short  HostGetPort( HOSTHANDLE hostID );
  DWORD           HostGetAddress( HOSTHANDLE hostID );

  unsigned short  CountBits( DWORD value );

  bool IsServerOn()
  {
    return d_bIsServerOn;
  }


  bool IsClientOn()
  {
    return d_bIsClientOn;
  }


  const cNetClock &GetClock()
  {
    return d_clock;
  }


  DWORD GetBytesTransfered()
  {
    return d_bytesTransfered;
  }


  void ResetBytesTransfered()
  {
    d_bytesTransfered = 0;
  }
};

#endif // !defined(AFX_MTUDP_H__B0419AC9_D198_11D3_AE4F_00E029031C67__INCLUDED_)
