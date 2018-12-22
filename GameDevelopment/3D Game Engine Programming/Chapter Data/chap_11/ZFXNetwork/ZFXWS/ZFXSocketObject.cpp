// File: ZFXSocketObject.cpp

#include <windows.h>       // type definitions
#include "ZFXWS.h"         // class definition


extern bool g_bLF;
extern int g_PkgSize;


/**
 * Initialize this socket object and let it know the logfile.
 */
ZFXSocketObject::ZFXSocketObject(FILE *pLog) {
   m_skSocket   = INVALID_SOCKET;
   m_pLog       = pLog;
   m_bRunning   = false;
   m_pInbox     = NULL;
   m_Buffer     = NULL;
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Destructor: Just disconnect this socket object.
 */
ZFXSocketObject::~ZFXSocketObject(void) { 
   if (IsRunning()) {
      Disconnect();
      m_bRunning = false;
      }
   if (m_pInbox) {
      delete m_pInbox;
      m_pInbox = NULL;
      }
   if (m_Buffer) {
      delete [] m_Buffer;
      m_Buffer = NULL;
      }
   m_skSocket = INVALID_SOCKET;
   } // destructor
/*----------------------------------------------------------------*/


/**
 * Now we go and create a socket, this is needed for clients and 
 * servers as first step.
 */
HRESULT ZFXSocketObject::CreateSocket(void) {
   // if socket is in use then close it
   if (m_skSocket != INVALID_SOCKET) Disconnect();

   m_skSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (m_skSocket==INVALID_SOCKET) {
      Log("error: socket() failed");
      return ZFX_FAIL;
      }
   m_pInbox = new ZFXQueue();
   m_Buffer = new char[65536];
   memset(m_Buffer,0,65536);
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/


/**
 * Bind the socket of this object to a given port.
 */
HRESULT ZFXSocketObject::Bind(int nPort) {
   sockaddr_in saServerAddress;

   memset(&saServerAddress, 0, sizeof(sockaddr_in));
   saServerAddress.sin_family = AF_INET;
   saServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
   saServerAddress.sin_port = htons(nPort);

   if ( bind(m_skSocket, (sockaddr*)&saServerAddress, 
             sizeof(sockaddr)) == SOCKET_ERROR) {
      LogLastWSAError("bind() failed");
      Disconnect();
      return ZFX_FAIL;
      }

   return ZFX_OK;
   } // Bind
/*----------------------------------------------------------------*/


/**
 * Listen on the connected socket.
 */
HRESULT ZFXSocketObject::Listen(void) {
   if (listen(m_skSocket, 32) != 0) {
      Log("error: listen() failed");
      return ZFX_FAIL;
      }
   m_bRunning = true;
   return ZFX_OK;
   } // Listen
/*----------------------------------------------------------------*/


/**
 * Accept the connection from a connection waiting in the queue.
 */
HRESULT ZFXSocketObject::Accept(SOCKET *skToNewClient) {
   sockaddr_in saClientAddress;
   int         nClientSize = sizeof(sockaddr_in);

   (*skToNewClient) = accept(m_skSocket, 
                             (sockaddr*)&saClientAddress, 
                             &nClientSize);
   if ((*skToNewClient) == INVALID_SOCKET) {
      Log("error: accept() failed");
      return ZFX_FAIL;
      }
   Log("client from %d.%d.%d.%d now succesfully connected to us",
      saClientAddress.sin_addr.S_un.S_un_b.s_b1,
      saClientAddress.sin_addr.S_un.S_un_b.s_b2,
      saClientAddress.sin_addr.S_un.S_un_b.s_b3,
      saClientAddress.sin_addr.S_un.S_un_b.s_b4);

   return ZFX_OK;
   } // accept
/*----------------------------------------------------------------*/


/**
 * Connect the socket attribute to a given server address/name at
 * the given port number. 
 */
HRESULT ZFXSocketObject::Connect(char *chServer, int nPort) {
   sockaddr_in saServerAddress;
   LPHOSTENT   pHost=NULL;

   // try to find the server by address or name
   memset(&saServerAddress,0,sizeof(sockaddr_in));
   saServerAddress.sin_port        = htons(nPort);
   saServerAddress.sin_family      = AF_INET;
   saServerAddress.sin_addr.s_addr = inet_addr(chServer);

   if (saServerAddress.sin_addr.s_addr==INADDR_NONE) {
      pHost = gethostbyname(chServer);
      if (pHost != NULL) {
         saServerAddress.sin_addr.s_addr = 
            ((LPIN_ADDR)pHost->h_addr)->s_addr;
         }
      else {
         Log("error: server \"%s\" not found", chServer);
         return ZFX_FAIL;
         }
      }

   // connect the socket to the server address
   if (connect(m_skSocket, (sockaddr*)&saServerAddress, 
               sizeof(sockaddr)) == SOCKET_ERROR) {
      LogLastWSAError("connect() in ZFXSocketObject::Connect failed");
      Disconnect();
      return ZFX_FAIL;
      }

   // now the SocketObject is active
   m_bRunning = true;
   return ZFX_OK;
   } // connect
/*----------------------------------------------------------------*/


/**
 * Just skip the connection of the active socket.
 */
void ZFXSocketObject::Disconnect(void) {
   if (m_skSocket != INVALID_SOCKET) {
      shutdown(m_skSocket, 0x02);
      closesocket(m_skSocket);
      m_skSocket = INVALID_SOCKET;
      Log("socket closed");
      }
   }
/*----------------------------------------------------------------*/


/**
 * Send data encapsulation for WinSock.
 */
int ZFXSocketObject::Send(const char *pPkg, UINT nSize) {
   UINT nSent=0;
   UINT n=0;

   while (nSent < nSize) {
      n = send(m_skSocket, pPkg+nSent, nSize-nSent, 0);
      if (n==SOCKET_ERROR) {
         LogLastWSAError("send() in ZFXSocketObject");
         return n;
         }
      else nSent += n;
      }
   return nSent;
   } // Send
/*----------------------------------------------------------------*/
int ZFXSocketObject::Send(const char *pPkg, UINT nSize, SOCKET skReceiver) {
   UINT nSent=0;
   UINT n=0;

   while (nSent < nSize) {
      n = send(skReceiver, pPkg+nSent, nSize-nSent, 0);
      if (n==SOCKET_ERROR) {
         LogLastWSAError("send() in ZFXSocketObject");
         return n;
         }
      else nSent += n;
      }
   return nSent;
   } // Send
/*----------------------------------------------------------------*/


/**
 * Receive waiting data from socket in reaction to FD_READ message.
 */
HRESULT ZFXSocketObject::Receive(SOCKET sk) {
   HRESULT hr         = ZFX_OK;
   UINT    nSize      = 65536;   // max size to read in one call
   UINT    nBytesRead = 0;       // actual bytes read in one call
   UINT    nReadHead  = 0;       // reading position in m_Buffer
   UINT    n          = 0;       // data in buffer remaining
   bool    bDone      = false;   // ready
  
   ZFXPACKAGE *pPkg          = NULL;
   UINT        nPkgSizeTotal = 0;

   // read up to 65536 bytes each call and loop until
   // no more data is waiting at socket to be received
   while (!bDone) {

      nBytesRead = recv(sk, &m_Buffer[n], nSize-n, 0);

      if (nBytesRead == SOCKET_ERROR) {
         int WSAError = WSAGetLastError();

         // ignore non-critical errors
         if ( (WSAError != WSAEMSGSIZE) &&
              (WSAError != WSAEWOULDBLOCK) ) {
            LogLastWSAError("recv() in ZFXSocketObject::Receive");
            hr = ZFX_FAIL;
            bDone = true;
            break;
            }
         }

      // new got nBytesRead bytes in m_Buffer so enqueue it
      if (nBytesRead <= 0) {
         bDone = true;
         }
      else {
         // take care of old data in the buffer
         nBytesRead += n;

         // loop as long as we find another complete header
         // in our buffer, note that a package could have
         // been split so take care of that
         while ( (nBytesRead - nReadHead) > g_PkgSize ) {
            // mask next chunk of data as ZFXPACKAGE
            pPkg = (ZFXPACKAGE*)&m_Buffer[nReadHead];
            pPkg->pData = &m_Buffer[nReadHead] + g_PkgSize;

            // how big is this package?
            nPkgSizeTotal = g_PkgSize + pPkg->nLength;

            // do we have the whole package received
            if ( (nBytesRead-nReadHead) >= (nPkgSizeTotal) ) {
               m_pInbox->Enqueue(pPkg, nPkgSizeTotal);
               nReadHead += nPkgSizeTotal;
               }
            // no just part of the package so go back to recv
            else {
               // copy the split package to start of the buffer
               memcpy(m_Buffer, &m_Buffer[nReadHead], nBytesRead-nReadHead);
               n = nBytesRead-nReadHead;
               break;
               }
            } // while

         // if we already got all data that was waiting
         if (nBytesRead < nSize) bDone = true;
         }
      } // while
   return hr;
   } // Receive
/*----------------------------------------------------------------*/


/**
 * Get the next element waiting in our inbox for processing.
 */
HRESULT ZFXSocketObject::GetNextPkg(ZFXPACKAGE *pPkg) {
   // is there a package at all?
   if (m_pInbox->GetCount() > 0) {
      // stream serialized data into our buffer
      m_pInbox->Front(m_Buffer, true);

      // fill it into the structure
      memcpy(pPkg, m_Buffer, g_PkgSize-sizeof(PVOID));
      memcpy(pPkg->pData, m_Buffer + g_PkgSize, pPkg->nLength);

      return ZFX_OK;
      }
   return ZFX_FAIL;
   } // GetNextPkg
/*----------------------------------------------------------------*/


/**
 * Feed a package by hand into the sockets inbox
 */
void ZFXSocketObject::FeedByHand(ZFXPACKAGE *pPkg) {
   int nPkgSizeTotal = g_PkgSize + pPkg->nLength;
   m_pInbox->Enqueue(pPkg, nPkgSizeTotal);
   } // FeedByHand
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: bool - flush immediately
 *        char - format string to output
 *        ...  - output values
 */
void ZFXSocketObject::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXSocktObj]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF) fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/


/**
 * Get the last WSA error an put it out to the log file together
 * with the given string an prefix "error: ".
 */
void ZFXSocketObject::LogLastWSAError(const char *error) {
   int n=WSAGetLastError();

   if (n==WSANOTINITIALISED)
      Log("error: %s | WSANOTINITIALISED", error);
   else if (n==WSAENETDOWN)
      Log("error: %s | WSAENETDOWN", error);
   else if (n==WSAEADDRINUSE)
      Log("error: %s | WSAEADDRINUSE", error);
   else if (n==WSAEINTR)
      Log("error: %s | WSAEINTR", error);
   else if (n==WSAEINPROGRESS)
      Log("error: %s | WSAEINPROGRESS", error);
   else if (n==WSAEALREADY)
      Log("error: %s | WSAEALREADY", error);
   else if (n==WSAEADDRNOTAVAIL)
      Log("error: %s | WSAEADDRNOTAVAIL", error);
   else if (n==WSAEAFNOSUPPORT)
      Log("error: %s | WSAEAFNOSUPPORT", error);
   else if (n==WSAECONNREFUSED)
      Log("error: %s | WSAECONNREFUSED", error);
   else if (n==WSAEFAULT)
      Log("error: %s | WSAEFAULT", error);
   else if (n==WSAEINVAL)
      Log("error: %s | WSAEINVAL", error);
   else if (n==WSAEISCONN)
      Log("error: %s | WSAEISCONN", error);
   else if (n==WSAENETUNREACH)
      Log("error: %s | WSAENETUNREACH", error);
   else if (n==WSAENOBUFS)
      Log("error: %s | WSAENOBUFS", error);
   else if (n==WSAENOTSOCK)
      Log("error: %s | WSAENOTSOCK", error);
   else if (n==WSAENOTCONN)
      Log("error: %s | WSAENOTCONN", error);
   else if (n==WSAETIMEDOUT)
      Log("error: %s | WSAETIMEDOUT", error);
   else if (n==WSAEWOULDBLOCK)
      Log("error: %s | WSAEWOULDBLOCK", error);
   else if (n==WSAEACCES)
      Log("error: %s | WSAEACCES", error);
   else if (n==WSAEMFILE)
      Log("error: %s | WSAEMFILE", error);
   else if (n==WSAEOPNOTSUPP)
      Log("error: %s | WSAEOPNOTSUPP", error);
   else
      Log("error: %s | unknown error code", error);
   } // LogLastWSAError
/*----------------------------------------------------------------*/
