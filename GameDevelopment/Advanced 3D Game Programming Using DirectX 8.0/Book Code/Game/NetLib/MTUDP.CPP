// MTUDP.cpp: implementation of the MTUDP class.
//
//////////////////////////////////////////////////////////////////////

#include "MTUDP.h"
#include "cNetError.h"
#include "Macros.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////

enum eMTUDPMsgType
{
  MTUDPMSGTYPE_ACKS         = 0,
  MTUDPMSGTYPE_RELIABLE     = 1,
  MTUDPMSGTYPE_UNRELIABLE   = 2,
  MTUDPMSGTYPE_CLOCK        = 3,

  MTUDPMSGTYPE_NUMMESSAGES  = 4,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MTUDP::MTUDP()
{
  d_bRunning = false;
  d_bSending = false;
  d_bListening = false;
  d_bIsClientOn = false;
  d_bIsServerOn = false;
  d_listenSocket = 0;
  d_sendSocket = 0;
  d_bytesTransfered = 0;
}


MTUDP::~MTUDP()
{
  StopListening();
  StopSending();
  Cleanup();
}


void MTUDP::Startup( unsigned short localListenPort, unsigned short foreignListenPort )
{
  Cleanup();

  // Start Winsock
  OutputDebugString( "MTUDP::Startup() - Initializing Winsock.\n" );

  WSAData wsaData;
  int     error;

  error = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
  if( error == SOCKET_ERROR )
  {
    char errorBuffer[ 100 ];

    error = WSAGetLastError();
    if( error == WSAVERNOTSUPPORTED )
    {
      sprintf( errorBuffer, "MTUDP::Startup() - WSAStartup() error.\nRequested v2.2, found only v%d.%d.", 
        LOBYTE( wsaData.wVersion ),
        HIBYTE( wsaData.wVersion ) );
      WSACleanup();
    }
    else
      sprintf( errorBuffer, "MTUDP::Startup() - WSAStartup() error %d", WSAGetLastError() );

    throw cNetError( errorBuffer );
  }

  d_localListenPort = localListenPort;
  d_foreignListenPort = foreignListenPort;
  d_bytesTransfered = 0;
  d_bRunning = true;
}


void MTUDP::StartListening()
{
  if( d_bListening == true ||
      d_bRunning == false )
    return;

  d_bListening = true;

  // Start the listen socket.
  OutputDebugString( "MTUPD::StartListening() - Creating the listen socket.\n" );

  d_listenSocket = socket( AF_INET, SOCK_DGRAM, 0 );
  if( d_listenSocket == INVALID_SOCKET )
  {
    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::StartListening() - socket() error %d", WSAGetLastError() );
    throw cNetError( errorBuffer );
  }

  SOCKADDR_IN localAddr;
  int         result;

  memset( &localAddr, 0, sizeof( SOCKADDR_IN ) );
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl( INADDR_ANY );
  localAddr.sin_port = htons( d_localListenPort );

  result = bind( d_listenSocket, (sockaddr *)&localAddr, sizeof( SOCKADDR_IN ) );
  if( result == SOCKET_ERROR )
  {
    closesocket( d_listenSocket );

    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::StartListening() - bind() error %d", WSAGetLastError() );
    throw cNetError( errorBuffer );
  }

  unsigned long int val;

  val = 1;  // Anything non-zero.
  result = ioctlsocket( d_listenSocket, FIONBIO, &val );
  if( result )
  {
    closesocket( d_listenSocket );

    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::StartListening() - ioctlsocket() error %d", WSAGetLastError() );
    throw cNetError( errorBuffer );
  }


  // Start the thread.
  OutputDebugString( "MTUPD::StartListening() - Starting the listen thread.\n" );

  cThread::Begin();
}


void MTUDP::StartSending()
{
  if( d_bSending == true ||
      d_bRunning == false )
    return;

  d_bSending = true;

  // Create send socket
  OutputDebugString( "MTUPD::StartSending() - Creating the send socket.\n" );

  d_sendSocket = socket( AF_INET, SOCK_DGRAM, 0 );
  if( d_sendSocket == INVALID_SOCKET )
  {
    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::StartSending() - socket() error %d", WSAGetLastError() );
    throw cNetError( errorBuffer );
  }

  SOCKADDR_IN localAddr;
  int         result;

  memset( &localAddr, 0, sizeof( SOCKADDR_IN ) );
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl( INADDR_ANY );
  localAddr.sin_port = htons( 0 );

  result = bind( d_sendSocket, (sockaddr *)&localAddr, sizeof( SOCKADDR_IN ) );
  if( result == SOCKET_ERROR )
  {
    closesocket( d_sendSocket );

    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::StartSending() - bind() error %d", WSAGetLastError() );
    throw cNetError( errorBuffer );
  }

  unsigned long int val;

  val = 1;  // Anything non-zero.
  result = ioctlsocket( d_sendSocket, FIONBIO, &val );
  if( result )
  {
    closesocket( d_sendSocket );

    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::StartSending() - ioctlsocket() error %d", WSAGetLastError() );
    throw cNetError( errorBuffer );
  }
}


void MTUDP::StopListening()
{
  if( d_bListening == false )
    return;

  d_bListening = false;

  cThread::End();

  OutputDebugString( "MTUPD::StopListening() - Closing the listen socket.\n" );
  closesocket( d_listenSocket );
}


void MTUDP::StopSending()
{
  if( d_bSending == false )
    return;

  d_bSending = false;

  OutputDebugString( "MTUPD::StopSending() - Closing the send socket.\n" );
  closesocket( d_sendSocket );
}


void MTUDP::Cleanup()
{
  if( d_bRunning == false )
    return;

  d_bRunning = false;

  StopListening();
  StopSending();

  OutputDebugString( "MTUPD::Cleanup() - Cleaning up Winsock.\n" );

  d_hosts.clear();
  d_hostMap.clear();
  WSACleanup();
}
////////////////////////////////////////////////////////////////////////////////

void MTUDP::StartClient()
{
  d_bIsClientOn = true;
}


void MTUDP::StopClient()
{
  d_bIsClientOn = false;
}


void MTUDP::StartServer()
{
  d_bIsServerOn = true;
}


void MTUDP::StopServer()
{
  d_bIsServerOn = false;
}
////////////////////////////////////////////////////////////////////////////////

DWORD MTUDP::ThreadProc()
{
  if( d_bListening == false )
    return 0;  // Quit already?!

  char            inBuffer[ MAX_UDPBUFFERSIZE ];
  timeval         waitTimeStr;
  SOCKADDR_IN     fromAddr;
  int             fromLen;
  unsigned short  result;
  FD_SET          set;

#if defined( _DEBUG )
  memset( inBuffer, 0xFF, MAX_UDPBUFFERSIZE );
#endif

  OutputDebugString( "MTUDP::ThreadProc() - Listening thread started.\n" );

  try
  {
    while( d_bListening == true )
    {
      // Listen to see if there is data waiting to be read.
      FD_ZERO( &set );
      FD_SET( d_listenSocket, &set );

      waitTimeStr.tv_sec = 0;
      waitTimeStr.tv_usec = 0;

      // Select tells us if there is data to be read.
      result = select( FD_SETSIZE, &set, NULL, NULL, &waitTimeStr );
      if( result == 0 )
	  {
		Sleep( 10 );
        continue;
	  }
      if( result == SOCKET_ERROR )
      {
        char errorBuffer[ 100 ];

        sprintf( errorBuffer, "MTUDP::ThreadProc() - select() error %d", WSAGetLastError() );
        throw cNetError( errorBuffer );
      }

      // Recvfrom gets the data and puts it in inBuffer.
      fromLen = sizeof( SOCKADDR );
      result = recvfrom( d_listenSocket, inBuffer, MAX_UDPBUFFERSIZE, 0, (SOCKADDR *)&fromAddr, &fromLen );
      if( result == 0 )
	  {
		Sleep( 10 );
        continue;
	  }
      if( result == SOCKET_ERROR )
      {
        char errorBuffer[ 100 ];

        sprintf( errorBuffer, "MTUDP::ThreadProc() - recvfrom() error %d", WSAGetLastError() );
        throw cNetError( errorBuffer );
      }

      d_bytesTransfered += result;

      // Put the received data in a mutex-protected queue here.
      ProcessIncomingData( inBuffer, result, ntohl( fromAddr.sin_addr.s_addr ), GetTickCount() );

#if defined( _DEBUG )
  memset( inBuffer, 0xFF, MAX_UDPBUFFERSIZE );
#endif
    }  // while
  }  // try
  catch( cNetError &err )
  {
    OutputDebugString( "MTUDP::ThreadProc() - Listening thread aborted!\n" );
    cThread::SetError( err.Text() );
  }

  OutputDebugString( "MTUDP::ThreadProc() - Listening thread ended.\n" );

  // Returns 1 if the close was not graceful.
  return d_bListening == true;
}


void MTUDP::ProcessIncomingData( char *pData, unsigned short length, DWORD address, DWORD receiveTime )
{
  // Find the host that sent this data.
  cHost              *pHost;
  HOSTLIST::iterator iHost;

  cMonitor::MutexOn();

  for( iHost = d_hosts.begin(); iHost != d_hosts.end(); ++iHost )
  {
    if( (*iHost)->GetAddress() == address )
      break;
  }

  if( iHost == d_hosts.end() )
  {
    DWORD hostID;

    hostID = HostCreate( address, d_foreignListenPort );
    if( hostID == 0 )
    {
      cMonitor::MutexOff();
      throw cNetError( "MTUDP::ProcessIncomingData() - Host creation failed." );
    }
    pHost = d_hostMap[ hostID ];
  }
  else
    pHost = *iHost;


  // Process the header for this packet.
  bool          bMessageArrived;
  unsigned char code;
  char          *ptr;

  bMessageArrived = false;
  ptr = pData;

  while( ptr < pData + length )
  {
    code = *ptr;
    ptr++;

    if( code == MTUDPMSGTYPE_ACKS )
    {
      // Process any ACKs in the packet.
      ptr += pHost->ProcessIncomingACKs( ptr, pData + length - ptr, receiveTime );
    }
    else if( code == MTUDPMSGTYPE_RELIABLE )
    {
      bMessageArrived = true;

      // Process reliable message in the packet.
      ptr += pHost->ProcessIncomingReliable( ptr, pData + length - ptr, receiveTime );
    }
    else if( code == MTUDPMSGTYPE_UNRELIABLE )
    {
      // Process UNreliable message in the packet.
      ptr += pHost->ProcessIncomingUnreliable( ptr, pData + length - ptr, receiveTime );
    }
    else if( code == MTUDPMSGTYPE_CLOCK )
    {
      ptr += ProcessIncomingClockData( ptr, pData + length - ptr, pHost, receiveTime );
    }
    else
    {
      char errorBuffer[ 100 ];

      cMonitor::MutexOff();
      sprintf( errorBuffer, "MTUDP::ProcessIncomingData() - unrecognized block type %d.", code );
      throw cNetError( errorBuffer );
    }
  }


  if( bMessageArrived == true )
  {
    // Send an ACK immediately. If this machine is the
    // server, also send a timestamp of the server clock.
    ReliableSendTo( NULL, 0, pHost->GetAddress() );
  }

  cMonitor::MutexOff();
}


unsigned short MTUDP::GetUnreliableData( char *pBuffer, unsigned short maxLen, HOSTHANDLE *pHostID )
{
  if( pBuffer == NULL ||
      pHostID == NULL )
    throw cNetError( "MTUPD::GetUnreliableData() - Invalid parameters." );

  if( maxLen == 0 )
    return 0;

  cDataPacket        *pPacket;
  HOSTLIST::iterator iHost;

  pPacket = NULL;

  cMonitor::MutexOn();

  // Do our list<> stuff here.
  // Is there any queued, ordered data?
  for( iHost = d_hosts.begin(); iHost != d_hosts.end(); ++iHost )
  {
    pPacket = (*iHost)->GetUnreliableInQueue().GetPacket();
    if( pPacket != NULL )
      break;
  }


  if( pPacket != NULL )
  {
    unsigned short length;

    length = pPacket->d_length > maxLen ? maxLen : pPacket->d_length;
    memcpy( pBuffer, pPacket->d_data, length );

    delete pPacket;

    *pHostID = (*iHost)->GetAddress();

    cMonitor::MutexOff();
    return length;
  }

  cMonitor::MutexOff();
  return 0;
}


unsigned short MTUDP::GetReliableData( char *pBuffer, unsigned short maxLen, HOSTHANDLE *pHostID )
{
  if( pBuffer == NULL ||
      pHostID == NULL )
    throw cNetError( "MTUPD::GetReliableData() - Invalid parameters." );

  if( maxLen == 0 )
    return 0;

  cDataPacket        *pPacket;
  HOSTLIST::iterator iHost;

  pPacket = NULL;

  cMonitor::MutexOn();

  // Do our list<> stuff here.
  // Is there any queued, ordered data?
  for( iHost = d_hosts.begin(); iHost != d_hosts.end(); ++iHost )
  {
    pPacket = (*iHost)->GetInQueue().GetPacket();
    if( pPacket != NULL )
      break;
  }

  if( pPacket != NULL )
  {
    unsigned short length;

    length = pPacket->d_length > maxLen ? maxLen : pPacket->d_length;
    memcpy( pBuffer, pPacket->d_data, length );

    delete pPacket;

    *pHostID = (*iHost)->GetAddress();

    cMonitor::MutexOff();
    return length;
  }

  cMonitor::MutexOff();
  return 0;
}


// ResendData is only called from ResendPackets() and is therefore thread safe.
void MTUDP::ResendData( cDataPacket *pPacket, cHost *pHost )
{
  if( pHost == NULL ||
      pPacket == NULL )
    throw cNetError( "MTUDP::ResendData() - Invalid parameters." );

  char            outBuffer[ MAX_UDPBUFFERSIZE ];
  unsigned short  count;
  DWORD           packetID;

  count = 0;
  memset( outBuffer, 0, MAX_UDPBUFFERSIZE );


  // Attach the ACKs.
  if( pHost->GetInQueue().GetCount() != 0 )
  {
    // Flag indicating this block is a set of ACKs.
    outBuffer[ count ] = MTUDPMSGTYPE_ACKS;
    count++;

    count += pHost->AddACKMessage( &outBuffer[ count ], MAX_UDPBUFFERSIZE );
  }


  // Attach the message data.
  char            *pStr;
  unsigned short  length;

  packetID = pPacket->d_id;
  length = pPacket->d_length;
  pStr = pPacket->d_data;


  // Flag indicating this block is a reliable message.
  outBuffer[ count ] = MTUDPMSGTYPE_RELIABLE;
  count++;

  // Append the message
  memcpy( &outBuffer[ count ], &packetID, sizeof( DWORD ) );
  count += sizeof( DWORD );
  memcpy( &outBuffer[ count ], &length, sizeof( unsigned short ) );
  count += sizeof( unsigned short );
  memcpy( &outBuffer[ count ], pStr, length );
  count += length;


  // Attach the previous reliable message, just to ensure that it gets there.
  cDataPacket secondPacket;

  if( pHost->GetOutQueue().GetPreviousPacket( packetID, &secondPacket ) == true )
  {
    // Flag indicating this block is a reliable message.
    outBuffer[ count ] = MTUDPMSGTYPE_RELIABLE;
    count++;

    // Append the message
    memcpy( &outBuffer[ count ], &secondPacket.d_id, sizeof( DWORD ) );
    count += sizeof( DWORD );
    memcpy( &outBuffer[ count ], &secondPacket.d_length, sizeof( unsigned short ) );
    count += sizeof( unsigned short );
    memcpy( &outBuffer[ count ], secondPacket.d_data, secondPacket.d_length );
    count += secondPacket.d_length;
  }


  count += AddClockData( &outBuffer[ count ], MAX_UDPBUFFERSIZE - count, pHost );


  d_bytesTransfered += count;


#if defined( _DEBUG_VERBOSE )
  OUTPUTREPORT3( " >  %04d (%d/%d) 2\n", packetID, length, count );
#endif

#if defined( _DEBUG_DROPTEST ) && _DEBUG_DROPTEST > 1
  if( rand() % _DEBUG_DROPTEST != _DEBUG_DROPTEST - 1 )
    return;
#endif

  // Send
  SOCKADDR_IN     remoteAddr;
  unsigned short  result;

  memset( &remoteAddr, 0, sizeof( SOCKADDR_IN ) );
  remoteAddr.sin_family = AF_INET;
  remoteAddr.sin_addr.s_addr = htonl( pHost->GetAddress() );
  remoteAddr.sin_port = htons( pHost->GetPort() );

  // Send the data.
  result = sendto( d_sendSocket,
                   outBuffer,
                   count,
                   0,
                   (SOCKADDR *)&remoteAddr,
                   sizeof( SOCKADDR ) );
  if( result < count )
  {
    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::ResendData() - Sent %d of %d bytes.", result, count );
    throw cNetError( errorBuffer );
  }
  if( result == SOCKET_ERROR )
  {
    char errorBuffer[ 100 ];

    sprintf( errorBuffer, "MTUPD::ResendData() - sendto() error %d", WSAGetLastError() );
    throw cNetError( errorBuffer );
  }
}


void MTUDP::ReliableSendTo( char *pStr, unsigned short length, HOSTHANDLE hostID )
{
  cHost *pHost;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost == NULL )
    throw cNetError( "MTUDP::ReliableSendTo() - Invalid parameters." );

  char            outBuffer[ MAX_UDPBUFFERSIZE ];
  unsigned short  count;
  DWORD           packetID;

  count = 0;
  memset( outBuffer, 0, MAX_UDPBUFFERSIZE );


  // Attach the ACKs.
  if( pHost->GetInQueue().GetCount() != 0 )
  {
    // Flag indicating this block is a set of ACKs.
    outBuffer[ count ] = MTUDPMSGTYPE_ACKS;
    count++;

    count += pHost->AddACKMessage( &outBuffer[ count ], MAX_UDPBUFFERSIZE );
  }


  // Attach the message data.
  packetID = pHost->GetOutQueue().GetCurrentID();
  if( pStr )
  {
    // Flag indicating this block is a message.
    outBuffer[ count ] = MTUDPMSGTYPE_RELIABLE;
    count++;


    memcpy( &outBuffer[ count ], &packetID, sizeof( DWORD ) );
    count += sizeof( DWORD );
    memcpy( &outBuffer[ count ], &length, sizeof( unsigned short ) );
    count += sizeof( unsigned short );
    memcpy( &outBuffer[ count ], pStr, length );
    count += length;
  }


  // Attach the previous message, just to ensure that it gets there.
  cDataPacket secondPacket;

  if( pHost->GetOutQueue().GetPreviousPacket( packetID, &secondPacket ) == true )
  {
    // Flag indicating this block is a message.
    outBuffer[ count ] = MTUDPMSGTYPE_RELIABLE;
    count++;

    // Append the message
    memcpy( &outBuffer[ count ], &secondPacket.d_id, sizeof( DWORD ) );
    count += sizeof( DWORD );
    memcpy( &outBuffer[ count ], &secondPacket.d_length, sizeof( unsigned short ) );
    count += sizeof( unsigned short );
    memcpy( &outBuffer[ count ], secondPacket.d_data, secondPacket.d_length );
    count += secondPacket.d_length;
  }


  count += AddClockData( &outBuffer[ count ], MAX_UDPBUFFERSIZE - count, pHost );


  d_bytesTransfered += count;


#if defined( _DEBUG_VERBOSE )
    OUTPUTREPORT3( " >  %04d (%d/%d) 1\n", packetID, length, count );
#endif

#if defined( _DEBUG_DROPTEST ) && _DEBUG_DROPTEST > 1
  if( rand() % _DEBUG_DROPTEST != _DEBUG_DROPTEST - 1 )
  {
#endif
    // Send
    SOCKADDR_IN     remoteAddr;
    unsigned short  result;

    memset( &remoteAddr, 0, sizeof( SOCKADDR_IN ) );
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl( pHost->GetAddress() );
    remoteAddr.sin_port = htons( pHost->GetPort() );

    // Send the data.
    result = sendto( d_sendSocket, 
                     outBuffer,
                     count,
                     0,
                     (SOCKADDR *)&remoteAddr,
                     sizeof( SOCKADDR ) );
    if( result < count )
    {
      char errorBuffer[ 100 ];

      cMonitor::MutexOff();
      sprintf( errorBuffer, "MTUPD::ReliableSendTo() - Sent %d of %d bytes.", result, count );
      throw cNetError( errorBuffer );
    }
    if( result == SOCKET_ERROR )
    {
      char errorBuffer[ 100 ];

      cMonitor::MutexOff();
      sprintf( errorBuffer, "MTUPD::ReliableSendTo() - sendto() error %d", WSAGetLastError() );
      throw cNetError( errorBuffer );
    }
#if defined( _DEBUG_DROPTEST )
  }
#endif

  if( pStr )
    pHost->GetOutQueue().AddPacket( pStr, length );

  cMonitor::MutexOff();
}


void MTUDP::UnreliableSendTo( char *pStr, unsigned short length, HOSTHANDLE hostID )
{
  cHost *pHost;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost == NULL )
  {
    cMonitor::MutexOff();
    throw cNetError( "MTUDP::UnreliableSendTo() - Invalid parameters." );
  }

  char            outBuffer[ MAX_UDPBUFFERSIZE ];
  unsigned short  count;
  DWORD           packetID;

  count = 0;
  memset( outBuffer, 0, MAX_UDPBUFFERSIZE );


  // Attach the ACKs.
  if( pHost->GetInQueue().GetCount() != MTUDPMSGTYPE_ACKS )
  {
    // Flag indicating this block is a set of ACKs.
    outBuffer[ count ] = 0;
    count++;

    count += pHost->AddACKMessage( &outBuffer[ count ], MAX_UDPBUFFERSIZE );
  }


  // Attach the previous message, just to ensure that it gets there.
  // This is done first because of the way the unreliableInQueue works.
  cDataPacket secondPacket;

  packetID = pHost->GetUnreliableOutQueue().GetCurrentID();
  if( pHost->GetUnreliableOutQueue().GetPreviousPacket( packetID, &secondPacket ) == true )
  {
    // Flag indicating this block is a message.
    outBuffer[ count ] = MTUDPMSGTYPE_UNRELIABLE;
    count++;

    // Append the message
    memcpy( &outBuffer[ count ], &secondPacket.d_id, sizeof( DWORD ) );
    count += sizeof( DWORD );
    memcpy( &outBuffer[ count ], &secondPacket.d_length, sizeof( unsigned short ) );
    count += sizeof( unsigned short );
    memcpy( &outBuffer[ count ], secondPacket.d_data, secondPacket.d_length );
    count += secondPacket.d_length;
  }


  // Attach the message data.
  if( pStr )
  {
    // Flag indicating this block is a message.
    outBuffer[ count ] = MTUDPMSGTYPE_UNRELIABLE;
    count++;

#if defined( _DEBUG_VERBOSE )
    OUTPUTREPORT2( " >  %04d (%d) 1\n", packetID, length );
#endif

    memcpy( &outBuffer[ count ], &packetID, sizeof( DWORD ) );
    count += sizeof( DWORD );
    memcpy( &outBuffer[ count ], &length, sizeof( unsigned short ) );
    count += sizeof( unsigned short );
    memcpy( &outBuffer[ count ], pStr, length );
    count += length;
  }


  // No clock data is added because the there is no
  // response from the server so synch is impossible.


  d_bytesTransfered += count;


#if defined( _DEBUG_DROPTEST ) && _DEBUG_DROPTEST > 1
  if( rand() % _DEBUG_DROPTEST != _DEBUG_DROPTEST - 1 )
  {
#endif
    // Send
    SOCKADDR_IN     remoteAddr;
    unsigned short  result;

    memset( &remoteAddr, 0, sizeof( SOCKADDR_IN ) );
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl( pHost->GetAddress() );
    remoteAddr.sin_port = htons( pHost->GetPort() );

    // Send the data.
    result = sendto( d_sendSocket, 
                     outBuffer,
                     count,
                     0,
                     (SOCKADDR *)&remoteAddr,
                     sizeof( SOCKADDR ) );
    if( result < count )
    {
      char errorBuffer[ 100 ];

      cMonitor::MutexOff();
      sprintf( errorBuffer, "MTUPD::UnreliableSendTo() - Sent %d of %d bytes.", result, count );
      throw cNetError( errorBuffer );
    }
    if( result == SOCKET_ERROR )
    {
      char errorBuffer[ 100 ];

      cMonitor::MutexOff();
      sprintf( errorBuffer, "MTUPD::UnreliableSendTo() - sendto() error %d", WSAGetLastError() );
      throw cNetError( errorBuffer );
    }
#if defined( _DEBUG_DROPTEST )
  }
#endif

  if( pStr )
    pHost->GetUnreliableOutQueue().AddPacket( pStr, length );

  cMonitor::MutexOff();
}


void MTUDP::ResendPackets()
{
  // If there is any out queue data that has been waiting too long, send it again.
  cDataPacket        packet;
  HOSTLIST::iterator iHost;
  float              ping;

  cMonitor::MutexOn();

  for( iHost = d_hosts.begin(); iHost != d_hosts.end(); ++iHost )
  {
    ping = (*iHost)->GetAverageTransPing( 70 );
    // Is there any queued, ordered data that has been queued for too long?
    if( (*iHost)->GetOutQueue().GetPacketForResend( (DWORD)ping, &packet ) == true )
    {
      // There is, so send it again!
#if defined( _DEBUG_VERBOSE )
      OUTPUTREPORT0( "***********************\n" );
#endif
      ResendData( &packet, *iHost );
    }
  }

  cMonitor::MutexOff();
}
////////////////////////////////////////////////////////////////////////////////

// Improve HostCreate() by only returning a handle (probably the DWORD address)
// and using a map<> to associate handles & cHosts in the list<>.
HOSTHANDLE MTUDP::HostCreate( char *pAddress, unsigned short port )
{
  if( pAddress == NULL ||
      port == 0 )
    return 0;

  cHost *pHost;

  pHost = new cHost;
  if( pHost == NULL )
    return 0;

  if( pHost->SetAddress( pAddress ) )
  {
    delete pHost;
    return 0;
  }

  pHost->SetPort( port );

  cMonitor::MutexOn();

  d_hosts.push_back( pHost );
  d_hostMap[ pHost->GetAddress() ] = pHost;

  cMonitor::MutexOff();

  return pHost->GetAddress();
}


// Improve HostCreate() by only returning a handle (probably the DWORD address)
// and using a map<> to associate handles & cHosts in the list<>.
HOSTHANDLE MTUDP::HostCreate( DWORD address, unsigned short port )
{
  if( address == 0 ||
      port == 0 )
    return 0;

  cHost *pHost;

  pHost = new cHost;
  if( pHost == NULL )
    return 0;

  pHost->SetAddress( address );
  pHost->SetPort( port );

  cMonitor::MutexOn();

  d_hosts.push_back( pHost );
  d_hostMap[ pHost->GetAddress() ] = pHost;

  cMonitor::MutexOff();

  return pHost->GetAddress();
}


void MTUDP::HostDestroy( HOSTHANDLE hostID )
{
  if( d_hosts.begin() == d_hosts.end() )
    return;

  cHost *pHost;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost == NULL )
  {
    cMonitor::MutexOff();
    return;
  }

  // Remove the host from the host map.
  HOSTMAP::iterator iHost;

  iHost = d_hostMap.find( hostID );
  if( iHost != d_hostMap.end() )
    d_hostMap.erase( iHost );

  // Remove the host from the linked list.
  d_hosts.remove( pHost );

  // Delete the host.
  delete pHost;

  cMonitor::MutexOff();
}


float MTUDP::HostGetAverageLinkPing( HOSTHANDLE hostID, float percent )
{
  cHost *pHost;
  float val;

  val = 0.0f;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost != NULL )
    val = pHost->GetAverageLinkPing( percent );

  cMonitor::MutexOff();

  return val;
}


float MTUDP::HostGetAverageTransPing( HOSTHANDLE hostID, float percent )
{
  cHost *pHost;
  float val;

  val = 0.0f;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost != NULL )
    val = pHost->GetAverageTransPing( percent );

  cMonitor::MutexOff();

  return val;
}


bool MTUDP::HostGetIsOutEmpty( HOSTHANDLE hostID )
{
  cHost *pHost;
  bool  val;

  val = true;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost != NULL )
    val = pHost->GetOutQueue().IsEmpty();

  cMonitor::MutexOff();

  return val;
}


unsigned short MTUDP::HostGetPort( HOSTHANDLE hostID )
{
  cHost           *pHost;
  unsigned short  val;

  val = 0;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost != NULL )
    val = pHost->GetPort();

  cMonitor::MutexOff();

  return val;
}


DWORD MTUDP::HostGetAddress( HOSTHANDLE hostID )
{
  cHost *pHost;
  DWORD val;

  val = 0;

  cMonitor::MutexOn();

  pHost = d_hostMap[ hostID ];
  if( pHost != NULL )
    val = hostID;

  cMonitor::MutexOff();

  return val;
}
////////////////////////////////////////////////////////////////////////////////

// Add the net-wide clock synch info.
// Thread safe because it is called from inside ProcessIncomingData()
unsigned short MTUDP::AddClockData( char *pData, unsigned short maxLen, cHost *pHost )
{
  DWORD time;
  char  *ptr;

  ptr = pData;

  if( d_bIsServerOn == true )
  {
    if( pHost->WasClockTimeSet() == true )
    {
      if( maxLen < sizeof( DWORD ) * 2 + 1 )
        throw cNetError( "MTUDP::AddClockData() - Insufficient buffer space." );

      *ptr = MTUDPMSGTYPE_CLOCK;
      ptr++;

      time = pHost->GetLastClockTime();
      memcpy( ptr, &time, sizeof( DWORD ) );
      ptr += sizeof( DWORD );

      time = d_clock.GetTime();
      memcpy( ptr, &time, sizeof( DWORD ) );
      ptr += sizeof( DWORD );
    }
  }
  else if( d_bIsClientOn == true )
  {
    if( maxLen < sizeof( DWORD ) + 1 )
      throw cNetError( "MTUDP::AddClockData() - Insufficient buffer space." );

    *ptr = MTUDPMSGTYPE_CLOCK;
    ptr++;

    time = GetTickCount();
    memcpy( ptr, &time, sizeof( DWORD ) );
    ptr += sizeof( DWORD );
  }

  return ptr - pData;
}


unsigned short MTUDP::ProcessIncomingClockData( char *pData, unsigned short len, cHost *pHost, DWORD receiveTime )
{
  // Process clock time.
  DWORD oldTime;
  char  *ptr;

  ptr = pData;

  if( d_bIsServerOn == true )
  {
    memcpy( &oldTime, ptr, sizeof( DWORD ) );
    ptr += sizeof( DWORD );
    pHost->SetLastClockTime( oldTime );

#if defined( _DEBUG_CLOCK )
    OUTPUTREPORT1( "clock: host sends %d\n", oldTime );
#endif
  }
  else if( d_bIsClientOn == true )
  {
    DWORD serverTime;

    memcpy( &oldTime, ptr, sizeof( DWORD ) );
    ptr += sizeof( DWORD );
    memcpy( &serverTime, ptr, sizeof( DWORD ) );
    ptr += sizeof( DWORD );

    d_clock.Synchronize( serverTime, oldTime, receiveTime, pHost->GetAverageLinkPing( 30 ) );

#if defined( _DEBUG_CLOCK )
    OUTPUTREPORT2( "clock: server sends %d, %d\n", oldTime, serverTime );
#endif
  }

  return ptr - pData;
}
////////////////////////////////////////////////////////////////////////////////

// Counts the number of bits required to hold a given value
unsigned short MTUDP::CountBits( DWORD value )
{
  int count;

  count = 0;
  while( value )
  {
    value >>= 1;
    count++;
  }

  return count;
}