// File: ZFXWS.h
#ifndef ZFXWS_H
#define ZFXWS_H

#include <windows.h>

#pragma comment(lib, "ws2_32.lib")


BOOL WINAPI DllEntryPoint( HINSTANCE hDll,
                           DWORD     fdwReason,
                           LPVOID    lpvRserved );

#include "..\ZFXNetworkDevice.h"
#include "..\ZFX.h"
#include "ZFXQueue.h"



class ZFXSocketObject;


// for server to keep list of connected clients
typedef struct ZFXCLIENT_TYPE {
   SOCKET skToClient;
   UINT   nID;
   } ZFXCLIENT;
/*----------------------------------------------------------------*/


/**
 *
 */
class ZFXSocketObject {
   public:
      ZFXSocketObject(FILE *pLog);
      ~ZFXSocketObject(void);

      bool    IsRunning(void) { return m_bRunning; }

      // socket misc operations
      HRESULT CreateSocket(void);
      HRESULT Bind(int nPort);
      HRESULT Listen(void);
      HRESULT Accept(SOCKET *skToNewClient);
      HRESULT Connect(char *chServer, int nPort);
      void    Disconnect(void);

      // send and receive data
      int     Send(const char*,UINT);
      int     Send(const char*,UINT,SOCKET);
      HRESULT Receive(SOCKET sk);

      void    FeedByHand(ZFXPACKAGE *pPkg);

      // for informational purposes
      SOCKET  GetSocket(void) { return m_skSocket; }
      void    LogLastWSAError(const char*);

      // information about inbox
      bool    IsPkgWaiting(void)   { return (m_pInbox->GetCount() > 0); }
      UINT    GetNextPkgSize(void) { return (m_pInbox->GetFrontSize()); }
      HRESULT GetNextPkg(ZFXPACKAGE*);

   private:
      ZFXQueue   *m_pInbox;
      SOCKET      m_skSocket;
      char       *m_Buffer;
      bool        m_bRunning;
      FILE       *m_pLog;

      void Log(char *chString, ...);
   }; // class
/*----------------------------------------------------------------*/


/**
 * Class definition for a NetworkDevice object, implementing the
 * interface ZFXNetworkDevice
 */
class ZFXWS : public ZFXNetworkDevice {
   public:
      ZFXWS(HINSTANCE hDLL);
      ~ZFXWS(void);
      
      // initialize the engine stuff
      HRESULT Init(HWND, ZFXNETMODE, int, char*, UINT, UINT, bool);
     
      // Interface functions
      void    Release(void);
      bool    IsRunning(void) { return m_bRunning; }
      HRESULT MsgProc(WPARAM wp, LPARAM lp);
      HRESULT SendToServer(const ZFXPACKAGE*);
      HRESULT SendToClients(const ZFXPACKAGE*);
      HRESULT SendToClient(const ZFXPACKAGE*,UINT);
      bool    IsPkgWaiting(void)           { return m_pSockObj->IsPkgWaiting();   }
      UINT    GetNextPkgSize(void)         { return m_pSockObj->GetNextPkgSize(); }
      HRESULT GetNextPkg(ZFXPACKAGE *pPkg) { return m_pSockObj->GetNextPkg(pPkg); }

   private:
      ZFXSocketObject *m_pSockObj;
      ZFXNETMODE       m_Mode;
      ZFXCLIENT        m_Clients[256];
      char             m_ClCount;
      UINT             m_ClID;
      UINT             m_ClMax;
      char            *m_Buffer;
      FILE            *m_pLog;

      // initialize socketobjects
      HRESULT CreateServer(ZFXSocketObject **ppSkObject);
      HRESULT CreateClient(ZFXSocketObject **ppSkObject);

      // during message processing
      HRESULT OnAccept(void);
      HRESULT OnReceive(SOCKET skReceiving);
      HRESULT OnDisconnect(SOCKET skDisconnecting);

      void    Log(char *, ...);
   }; // class
/*----------------------------------------------------------------*/

#endif