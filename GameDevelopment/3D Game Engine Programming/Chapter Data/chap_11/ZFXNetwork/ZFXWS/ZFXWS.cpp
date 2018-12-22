// File: ZFXWS.cpp

#include <windows.h>       // type definitions
#include "ZFXWS.h"         // class definition

bool g_bLF=false;
int  g_PkgSize=sizeof(ZFXPACKAGE);


/*-----------------------------------------------------------*/
/* DLL stuff implementation                                  *
/*-----------------------------------------------------------*/

/**
 * DLL Entry Point similar to WinMain()/main()
 */
BOOL WINAPI DllEntryPoint(HINSTANCE hDll,
                          DWORD     fdwReason,
                          LPVOID    lpvReserved) {
   switch(fdwReason) {
      // called when we attach to the DLL
      case DLL_PROCESS_ATTACH:
         /* dll init/setup stuff */
         break;
      case DLL_PROCESS_DETACH:
         /* dll shutdown/release stuff */
         break;
      default:
         break;
      };
   
   return TRUE;
   } // DllEntryPoint
/*----------------------------------------------------------------*/

/**
 * Exported create function: Creates a new ZFXNetworkDevice object.
 */
HRESULT CreateNetworkDevice(HINSTANCE hDLL, ZFXNetworkDevice **pDevice) {
   if(!*pDevice) {
      *pDevice = new ZFXWS(hDLL);
      return ZFX_OK;
      }
   return ZFX_FAIL;
   }
/*----------------------------------------------------------------*/

/**
 * Exported release function: Realeses the given ZFXNetworkDevice object.
 */
HRESULT ReleaseNetworkDevice(ZFXNetworkDevice **pDevice) {
   if(!*pDevice) {
      return ZFX_FAIL;
      }
   delete *pDevice;
   *pDevice = NULL;
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/





/*-----------------------------------------------------------*/
/* ZFXWS class implementation                                *
/*-----------------------------------------------------------*/

/**
 * Constructor
 */
ZFXWS::ZFXWS(HINSTANCE hDLL) {
   m_hDLL         = hDLL;
   m_pLog         = NULL;
   m_pSockObj      = NULL;
   m_Buffer       = NULL;
   m_nPort        = 0;
   m_ClID         = 1;        // 0 reserved for server
   m_ClCount      = 0;
   m_ClMax        = 0;        // 0 = unlimited
   m_bRunning     = false;

   // open a new logfile
   m_pLog = fopen("Log_ZFXNetworkDevice.txt", "w");
   Log("online (waiting for initialization call)");
   }
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXWS::~ZFXWS() {
   Release();
   }
/*----------------------------------------------------------------*/


/**
 * Release all stuff.
 */
void ZFXWS::Release() {
   Log("shutting down WinSock");

   if (m_Mode == NMD_SERVER) {
      for (int i=0; i<m_ClCount; i++) {
         shutdown(m_Clients[i].skToClient,0x02);
         closesocket(m_Clients[i].skToClient);
         m_Clients[i].skToClient = INVALID_SOCKET;
         }
      }

   if (m_pSockObj) {
      delete m_pSockObj;
      m_pSockObj = NULL;
      }

   if (m_Buffer) {
      delete [] m_Buffer;
      m_Buffer = NULL;
      }

   // clean up winsock
   WSACleanup();
   m_bRunning = false;
   Log("offline (ok)");
   fclose(m_pLog);
   }
/*----------------------------------------------------------------*/


/**
 * Initialize WInSock first, then the appropriate socketobjects.
 * -> IN: HWND       - handle to application main window
 *        ZFXNETMODE - put NMD_CLIENT or NMD_SERVER here
 *        int        - number of port to use
 *        char       - ip address needed for client construction
 *        UINT       - maximum size of one package
 *        UINT       - maximum number of clients for one server
 *        bool       - flush log at each write operation
 */
HRESULT ZFXWS::Init(HWND hWnd, ZFXNETMODE nmd, int nPort,
                    char *pIP, UINT nMaxPkgSize, 
                    UINT nMaxClients, bool bSaveLog) {
   WSADATA wsaData;
   UINT    nEvents=0;
   WORD    wVersion;
   int     nRes;

   Log("calling initialization");
   
   m_nMaxSize   = nMaxPkgSize;
   m_Buffer     = new char[m_nMaxSize+g_PkgSize];
   m_hWndMain   = hWnd;
   m_nPort      = nPort;
   m_Mode       = nmd;
   m_ClMax      = nMaxClients;
   g_bLF        = bSaveLog;

   if (pIP) sprintf(m_pIP, "%s", pIP);

   wVersion = MAKEWORD(2,0);

   // startup WinSock
   if ((nRes=WSAStartup(wVersion, &wsaData))!=0) {
      if (nRes==WSAEINPROGRESS)
         Log("error: blocking WinSock app is running");
      else if (nRes==WSAVERNOTSUPPORTED)
         Log("error: WinSock version not supported");
      else if (nRes==WSASYSNOTREADY)
         Log("error: network subsystem not ready");
      else if (nRes==WSAEPROCLIM)
         Log("error: WinSock too buisy for another task");
      return ZFX_FAIL;
      }
   Log("WinSock 2.0 initialized (NMD: %d)", m_Mode);

   // create SockectObject as server listening to port
   if (m_Mode==NMD_SERVER) { 
      if (FAILED(CreateServer(&m_pSockObj))) {
         Log("error: CreateServer(Port:%d) failed", m_nPort);
         return ZFX_FAIL;
         }
      }
   // create SocketObject as client sending data to IP::Port
   else if (m_Mode==NMD_CLIENT) {
      if (strcmp(m_pIP, "")==0)
         sprintf(m_pIP, "LOCALHOST");
      if (FAILED(CreateClient(&m_pSockObj))) {
         Log("error: CreateClient(Port:%d to Server-IP:%s) failed", m_nPort, m_pIP);
         return ZFX_FAIL;
         }
      }
   else
      return ZFX_INVALIDPARAM;

   Log("initialized (online and ready)");
   m_bRunning = true;
   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 * Create socketobject as client and make it ready for traffic. A
 * client needs to be connected to the server IP at the port the
 * server is listening to.
 */
HRESULT ZFXWS::CreateClient(ZFXSocketObject **ppSkObject) {
   UINT nEvents=0;

   Log("firing up client");
   (*ppSkObject) = new ZFXSocketObject(m_pLog);

   if (!(*ppSkObject)) {
      Log("error: ZFXSocketObject construction failed in ZFXWS::CreateClient");
      return ZFX_FAIL;
      }
   Log("ZFXSocketObject constructed");

   // 1. step: create a socket object
   if (FAILED( (*ppSkObject)->CreateSocket() ))
      return ZFX_FAIL;
   Log("socket created");


   if (m_pIP == NULL) {
      gethostname(m_pIP, 10);
      }
   
   // 2. step: try to connect to the server
   if (FAILED( (*ppSkObject)->Connect(m_pIP, m_nPort) ))
      return ZFX_FAIL;
   Log("ZFXSocketObject connected as client to port: %d", m_nPort);

   nEvents |= FD_READ | FD_CLOSE;

   // 3. step: notification from windows WM_ about incoming events
   if (WSAAsyncSelect( (*ppSkObject)->GetSocket(), m_hWndMain, 
                      WM_ZFXCLIENT, nEvents)==SOCKET_ERROR) {
      Log("error: WSAAsyncSelect(WM_ZFXCLIENT) failed");
      m_pSockObj->Disconnect();
      return ZFX_FAIL;
      }
   return ZFX_OK;
   } // Create Client
/*----------------------------------------------------------------*/


/**
 * Create socketobjects as server and make them ready for traffic.
 * The server just needs to listen to the port the client is sending
 * its data to.
 */
HRESULT ZFXWS::CreateServer(ZFXSocketObject **ppSkObject) {
   UINT nEvents=0;

   Log("firing up server");
   (*ppSkObject) = new ZFXSocketObject(m_pLog);
   
   if (!(*ppSkObject)) {
      Log("error: m_pSkObject construction failed in ZFXWS::CreateServer");
      return ZFX_FAIL;
      }
   Log("ZFXSocketObjects constructed");

   // 1. step: create a socket object
   if (FAILED( (*ppSkObject)->CreateSocket() ))
      return ZFX_FAIL;
   Log("socket created");
   
   // 2. step: name the socket cause it's a server
   if (FAILED( (*ppSkObject)->Bind(m_nPort) ))
      return ZFX_FAIL;
   Log("server bind to port: %d", m_nPort);

   // 3. step: listen for incoming connection requests
   if (FAILED( (*ppSkObject)->Listen() ))
      return ZFX_FAIL;
   Log("server is now listening for incoming connection...");

   nEvents |= FD_READ | FD_WRITE | FD_CONNECT | FD_ACCEPT | FD_CLOSE;

   // 4. step: notification from windows WM_ about incoming events
   if (WSAAsyncSelect((*ppSkObject)->GetSocket(), m_hWndMain, 
                      WM_ZFXSERVER, nEvents)==SOCKET_ERROR) {
      Log("error: WSAAsyncSelect(WM_ZFXSERVER) failed");
      m_pSockObj->Disconnect();
      return ZFX_FAIL;
      }

   // initialize all client slots as invalid
   for (int i=0; i<256; i++) {
      m_Clients[i].skToClient = INVALID_SOCKET;
      m_Clients[i].nID = 0;
      }

   return ZFX_OK;
   } // CreateServer
/*----------------------------------------------------------------*/


/**
 * Send a data package from the server to all connected clients.
 * We need to loop through all active connections and send it.
 */
HRESULT ZFXWS::SendToClients(const ZFXPACKAGE *pPkg) {
   HRESULT hr=ZFX_OK;
   int     nBytes=0;
   int     nSize=g_PkgSize+pPkg->nLength;

   // security checks
   if (m_Mode != NMD_SERVER) return ZFX_FAIL;
   if (nSize > m_nMaxSize) return ZFX_OUTOFMEMORY;

   // serialize data in order to send from one mem segment
   memcpy(m_Buffer, pPkg, g_PkgSize);
   memcpy(m_Buffer+g_PkgSize, pPkg->pData, pPkg->nLength);

   // now go send it actually
   for (UINT i=0; i<m_ClCount; i++) {
      if (m_Clients[i].skToClient != INVALID_SOCKET) {
         nBytes = m_pSockObj->Send(m_Buffer, nSize, m_Clients[i].skToClient);
         // if failed report but go on with next clients
         if ( (nBytes==SOCKET_ERROR) || (nBytes<nSize) )
            hr = ZFX_FAIL;
         }
      }
   return hr;
   } // SendToClients
/*----------------------------------------------------------------*/


/**
 * Send a data package from the server to the given client. We
 * need to loop through all active connections to find that one.
 */
HRESULT ZFXWS::SendToClient(const ZFXPACKAGE *pPkg, UINT nCID) {
   HRESULT hr=ZFX_OK;
   int     nBytes=0;
   int     nSize=g_PkgSize+pPkg->nLength;

   // security checks
   if (m_Mode != NMD_SERVER) return ZFX_FAIL;
   if (nSize > m_nMaxSize) return ZFX_OUTOFMEMORY;

   // serialize data in order to send from one mem segment
   memcpy(m_Buffer, pPkg, g_PkgSize);
   memcpy(m_Buffer+g_PkgSize, pPkg->pData, pPkg->nLength);

   // now go send it actually
   for (UINT i=0; i<m_ClCount; i++) {
      if ( (m_Clients[i].skToClient != INVALID_SOCKET) &&
           (m_Clients[i].nID == nCID) ) {
         nBytes = m_pSockObj->Send(m_Buffer, nSize, m_Clients[i].skToClient);
         // if failed report but go on with next clients
         if ( (nBytes==SOCKET_ERROR) || (nBytes<nSize) )
            hr = ZFX_FAIL;
         }
      }
   return hr;
   } // SendToClient
/*----------------------------------------------------------------*/


/**
 * A client just sends the package over its socket that is
 * connected to the one and only server. No problems here.
 */
HRESULT ZFXWS::SendToServer(const ZFXPACKAGE *pPkg) {
   int nBytes=0;
   int nSize=g_PkgSize+pPkg->nLength;

   // security checks
   if (nSize > m_nMaxSize) return ZFX_OUTOFMEMORY;
   
   // serialize data in order to send from one mem segment
   memcpy(m_Buffer, pPkg, g_PkgSize);
   memcpy(m_Buffer+g_PkgSize, pPkg->pData, pPkg->nLength);

   // if pkg was sent by server then 
   // feed it directly into its own queue
   if (m_Mode == NMD_SERVER) {
      m_pSockObj->FeedByHand( (ZFXPACKAGE*)m_Buffer );
      return ZFX_OK;
      }

   // now go send it actually
   nBytes = m_pSockObj->Send(m_Buffer, nSize);
   if ( (nBytes==SOCKET_ERROR) || (nBytes<nSize) ) {
      Log("error: SendToServer() failed");
      return ZFX_FAIL;
      }

   return ZFX_OK;
   } // SendToServer
/*----------------------------------------------------------------*/


/**
 * Application must call this if a WM_ is coming from network.
 */
HRESULT ZFXWS::MsgProc(WPARAM wp, LPARAM lp) {
   WORD  wEvent, wError;
   
   // get data
   wError = HIWORD(lp);
   wEvent = LOWORD(lp);
   
   // which event
   switch (wEvent) {
      // confirmation after successful connection
      case FD_CONNECT: break;
         
      // client wants to be accepted
      case FD_ACCEPT:  { return OnAccept(); } break;
         
      // we are receiving something
      case FD_READ:    { return OnReceive(wp); } break;
         
      // a client wants to go offline
      case FD_CLOSE:   { return OnDisconnect(wp); } break;
         
      // we sent something?
      case FD_WRITE:   break;
         
      }
   return ZFX_OK;
   } // MsgProc
/*----------------------------------------------------------------*/


/**
 * There is someone is the queue of our server waiting to be accepted
 * so do him the favor.
 */
HRESULT ZFXWS::OnAccept(void) {
   int nSize=0, nBytes=0, i=m_ClCount;

   // maximum of 255 clients at all
   if (m_ClCount >= 255) return ZFX_OUTOFMEMORY;

   // application defined maximum number
   if ( (m_ClMax > 0) &&
        (m_ClCount >= m_ClMax) ) 
      return ZFX_OUTOFMEMORY;

   if (FAILED(m_pSockObj->Accept( &(m_Clients[i].skToClient) )))
      return ZFX_FAIL;

   // SEND ITS ID TO THE NEW CLIENT:
   ZFXPACKAGE *pPkg = (ZFXPACKAGE*)m_Buffer;
   pPkg->pData = &m_Buffer[g_PkgSize];
   pPkg->nLength = sizeof(UINT);
   pPkg->nType   = 0;   // ID Msg
   pPkg->nSender = 0;   // Server
   memcpy(pPkg->pData, &m_ClID, sizeof(UINT));

   // add counters
   m_Clients[i].nID = m_ClID;
   m_ClCount++;
   m_ClID++;

   nSize = g_PkgSize + pPkg->nLength;
   nBytes = m_pSockObj->Send(m_Buffer, nSize, m_Clients[i].skToClient);
   if ( (nBytes==SOCKET_ERROR) || (nBytes<nSize) )
      return ZFX_FAIL;

   // INFORM ALL CLIENTS AND THE SERVER ABOUT THE NEW KID IN TOWN
   pPkg->nType = 1;
   SendToClients(pPkg);

   // feed msg into server msg-queue
   m_pSockObj->FeedByHand(pPkg);
   return ZFX_OK;
   } // OnAccept
/*----------------------------------------------------------------*/


/**
 * The giving socket has data to be received in its queue. Just do it!
 */
HRESULT ZFXWS::OnReceive(SOCKET skReceiving) {
   if (m_bRunning) {
      return m_pSockObj->Receive(skReceiving);
      }
   else {
      Log("error: m_bRunning=0 in ZFXWS::OnReceive");
      return ZFX_FAIL;
      }
   } // OnReceive
/*----------------------------------------------------------------*/


/**
 * Close the given socket if he wants to disconnect.
 */
HRESULT ZFXWS::OnDisconnect(SOCKET skDisconnecting) {
   UCHAR      i=0;

   if (skDisconnecting==INVALID_SOCKET) 
      return ZFX_FAIL;

   if (m_Mode==NMD_SERVER) {

      // delete from the list of active clients
      for (i=0; i<m_ClCount; i++) {
         if (m_Clients[i].skToClient == skDisconnecting)
            break;
         }
     
      if (i>=m_ClCount) {
         Log("error: not listed client wants to be disconnected");
         return ZFX_FAIL;
         }
      
      // shutdown and close socket
      if ( shutdown(m_Clients[i].skToClient,0x02)==SOCKET_ERROR )
         m_pSockObj->LogLastWSAError("shutdown() in ZFXWS::OnDisconnect");
      if ( closesocket(m_Clients[i].skToClient)==SOCKET_ERROR )
         m_pSockObj->LogLastWSAError("closesocket() in ZFXWS::OnDisconnect");
      m_Clients[i].skToClient = INVALID_SOCKET;
      
      // inform clients and the server about disconnect
      ZFXPACKAGE *pPkg = (ZFXPACKAGE*)m_Buffer;
      pPkg->pData   = &m_Buffer[g_PkgSize];
      pPkg->nLength = sizeof(UINT);
      pPkg->nType   = 2;   // ID Msg disconnecting
      pPkg->nSender = 0;   // Server
      memcpy(pPkg->pData, &m_Clients[i].nID, sizeof(UINT));
      SendToClients(pPkg);
      
      // feed msg into server msg-queue
      m_pSockObj->FeedByHand(pPkg);

      // copy last struct to deleted field, adjust counter
      memcpy(&m_Clients[i], &m_Clients[m_ClCount-1], sizeof(ZFXCLIENT));
      m_ClCount--;
      }
   else {
      if ( shutdown(m_pSockObj->GetSocket(),0x02)==SOCKET_ERROR )
         m_pSockObj->LogLastWSAError("shutdown() in ZFXWS::OnDisconnect");
      if ( closesocket(m_pSockObj->GetSocket())==SOCKET_ERROR )
         m_pSockObj->LogLastWSAError("closesocket() in ZFXWS::OnDisconnect");
      }

   Log("one client disconnected");
   return ZFX_OK;
   } // OnDisconnect
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: bool - flush immediately
 *        char - format string to output
 *        ...  - output values
 */
void ZFXWS::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXWSDevice]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/

