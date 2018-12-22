// FILE: CGame.h

#ifndef CGAME_H
#define CGAME_H


#include <windows.h>
#include "ZFXGeneral.h"
#include "ZFXRenderer.h"
#include "ZFXRenderDevice.h"
#include "ZFXInput.h"
#include "ZFXInputDevice.h"
#include "ZFXAudio.H"
#include "ZFXAudioDevice.h"
#include "ZFXNetwork.h"
#include "ZFXNetworkDevice.h"
#include "CZFXModel.h"
#include "CGameSector.h"
#include "CGameLevel.h"
#include "CGameCharacter.h"
#include "CGamePlayer.h"
#include "CGameBulletList.h"


#define MAX_PCS 32      // max number of player characters


// used for complete player list
typedef struct CHARACTERLIST_TYPE {
   UINT      Num;
   UPDATEPKG upkg[MAX_PCS];
   } CHARACTERLIST;
/*----------------------------------------------------------------*/


// P R O T O T Y P E S /////////////////////////////////////////////

class CGame {
   public: 
      CGame(void);
      virtual ~CGame(void);

      HRESULT Init(HWND, HINSTANCE, ZFXNETMODE, char, const char*);
      void    Update(void);
      HRESULT Render(void);
      void    Release(void);

      HRESULT MsgProcNetwork(WPARAM w, LPARAM l)
               { return m_pNDevice->MsgProc(w, l); }
 
      void    Done(void)     { m_bDone = true;   }
      bool    IsDone(void)   { return m_bDone;   }
      void    Active(bool b) { m_bActive = b;    }
      bool    IsActive(void) { return m_bActive; }
      bool    IsWindowed(void) { return m_pRDevice->IsWindowed(); }

      LPZFXRENDERDEVICE GetRenderDevice(void) { return m_pRDevice; }


   private:
      // ZFXEngine Interfaces
      LPZFXRENDERER      m_pRenderer;
      LPZFXRENDERDEVICE  m_pRDevice;
      LPZFXINPUT         m_pInput;
      LPZFXINPUTDEVICE   m_pIDevice;
      LPZFXNETWORK       m_pNetwork;
      LPZFXNETWORKDEVICE m_pNDevice;
      LPZFXAUDIO         m_pAudio;
      LPZFXAUDIODEVICE   m_pADevice;

      LPGAMEBULLETLIST   m_pBullets;
      LPGAMEPLAYER       m_pPlayers[MAX_PCS];
      LPGAMEPLAYER       m_pMe;
      
      ZFXTimer          *m_pTimer;
      float              m_fDT;

      // general misc stuff
      HWND              m_hWnd;
      HINSTANCE         m_hInst;

      char              m_chIP[16];          // server IP
      char              m_nPort;             // networking port
      ZFXNETMODE        m_nmd;               // client or server
      UINT              m_NID;               // client network ID
      UINT              m_NumP;              // number of players
      char              m_chMsg[1024];       // to be shown each frame
      char              m_chWarn[1024];      // to be shown each frame

      bool              m_bDone;             // application is ready to exit
      bool              m_bActive;           // application is active
      bool              m_bShadows;          // render shadows?
      UINT              m_nZFXFont;          // id to ZFXRenderDevice created font
      UINT              m_BaseVSH;           // basic vertex shader
      UINT              m_BasePSH;           // basic pixel shader
      UINT              m_OmniVSH;           // omni light vertex shader
      UINT              m_OmniPSH;           // omni light pixel shader
      UINT              m_DepthVSH;          // depth rendering vertex shader
      UINT              m_DepthPSH;          // depth rendering pixel shader

ZFXVector m_vcTemp;
      ZFXVector         m_vcPos;             // current camera position

      CGameLevel       *m_pLevel;

      // PRIVATE METHODS
      HRESULT           InitRenderer(void);
      HRESULT           InitInput(void);
      HRESULT           InitAudio(void);
      HRESULT           InitNetwork(void);
      HRESULT           InitShaders(void);
      HRESULT           AddPlayer(UINT CID);
      HRESULT           RemovePlayer(UINT CID);
      HRESULT           RenderPlayers(void);
      HRESULT           ProcessInput(void);
      HRESULT           ProcessNetwork(void);
      HRESULT           InitPlayerListOnClient(CHARACTERLIST*);
      HRESULT           UpdateNetwork(void);
      void              UpdatePlayer(UINT, UPDATEPKG*);
      
      // networking messages
      HRESULT           SendMsg_PlayerListTo(UINT CID);
   }; // class
/*----------------------------------------------------------------*/

#endif