// File: ZFXNetworkDevice.h
#ifndef ZFXNETWORKINTERFACE_H
#define ZFXNETWORKINTERFACE_H


// I N C L U D E S /////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "ZFX.h"



// D E F I N E S ///////////////////////////////////////////////////

#define WM_ZFXSERVER (WM_USER + 1)
#define WM_ZFXCLIENT (WM_USER + 2)


// S T R U C T S ///////////////////////////////////////////////////

/**
 * nType reserved:
 *  0 - server message to new client giving its ID, pData = UINT nID
 *  1 - new client connected, pData = UINT nID
 *  2 - client disconnected,  pData = UINT nID
 */
typedef struct ZFXPACKAGE_TYPE {
   UINT   nLength;    // length of pData field in bytes
   UCHAR  nType;      // use for type (0 is ID notification)
   UINT   nSender;    // put senders ID here (0 is server)
   void  *pData;      // actual data of nLength
   } ZFXPACKAGE;
/*----------------------------------------------------------------*/


/**
 * 
 */
class ZFXNetworkDevice {
   protected:
      HWND       m_hWndMain;          // application main window
      HINSTANCE  m_hDLL;              // dll module handle
      bool       m_bRunning;          // after succesful initialization
      int        m_nPort;             // port for communication
      char       m_pIP[256];          // IP address (valid for clients)
      UINT       m_nMaxSize;          // max package size for send/receive
      FILE      *m_pLog;              // log file
      
      
   public:
      ZFXNetworkDevice(void) {};
      virtual ~ZFXNetworkDevice(void) {};
      
      
      // INIT/RELEASE STUFF:
      // ===================
      
      // feed: main window, server or client, port, ip (for clients only)
      //       max size of packages send over network, flush log
      virtual HRESULT Init(HWND, ZFXNETMODE, int nPort, char *pIP, 
                           UINT nMaxPkgSize, bool)=0;
      
      virtual void    Release(void)=0;
      virtual bool    IsRunning(void)=0;
      
      // proceed incoming notifications from windows
      virtual HRESULT MsgProc(WPARAM, LPARAM)=0;

      // send methods for clients and servers
      virtual HRESULT SendToServer(const ZFXPACKAGE*)=0;
      virtual HRESULT SendToClients(const ZFXPACKAGE*)=0;

      // get information about queued packages
      virtual bool    IsPkgWaiting(void)=0;
      virtual UINT    GetNextPkgSize(void)=0;
      virtual HRESULT GetNextPkg(ZFXPACKAGE*)=0;

      // don't know yet not implemented
      virtual HRESULT ServerUpdate(void)=0;
      
   }; // class

typedef class ZFXNetworkDevice *LPZFXNETWORKDEVICE;
/*----------------------------------------------------------------*/


extern "C" {
   HRESULT CreateNetworkDevice(HINSTANCE hDLL, ZFXNetworkDevice **pInterface);
   typedef HRESULT (*CREATENETWORKDEVICE)(HINSTANCE hDLL, ZFXNetworkDevice **pInterface);
   
   HRESULT ReleaseNetworkDevice(ZFXNetworkDevice **pInterface);
   typedef HRESULT (*RELEASENETWORKDEVICE)(ZFXNetworkDevice **pInterface);
   }
/*----------------------------------------------------------------*/

#endif