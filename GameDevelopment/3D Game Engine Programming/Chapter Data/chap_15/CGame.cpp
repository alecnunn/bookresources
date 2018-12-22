// FILE: CGame.cpp


#include "include/CGame.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

const UINT MAX_PKG_SIZE    = 4096;



CGame::CGame(void) {
   m_pAudio        = NULL;
   m_pADevice      = NULL;
   m_pRenderer     = NULL;
   m_pRDevice      = NULL;
   m_pInput        = NULL;
   m_pIDevice      = NULL;
   m_pTimer        = NULL;
   m_pLevel        = NULL;
   m_pMe           = NULL;
   m_pBullets      = NULL;
   m_nmd           = NMD_CLIENT;
   m_nPort         = 0;
   m_OmniVSH       = 0;
   m_OmniPSH       = 0;
   m_BaseVSH       = 0;
   m_BasePSH       = 0;
   m_DepthVSH      = 0;
   m_DepthPSH      = 0;
   m_NumP          = 0;
   m_fDT           = 0.0f;
   m_NID           = 99999;
   m_bShadows      = true;

   // init rando number function
   srand( GetTickCount() );

   memset(m_pPlayers, 0, sizeof(LPGAMEPLAYER)*MAX_PCS);

   strcpy(m_chMsg, "no network message yet");
   strcpy(m_chWarn, "");
   strcpy(m_chIP, "192.168.000.000");
   } // Release
/*----------------------------------------------------------------*/

CGame::~CGame(void) {
   Release();
   } // Release
/*----------------------------------------------------------------*/


// call this after initializing the window
HRESULT CGame::Init(HWND hWnd, HINSTANCE hInst, 
                    ZFXNETMODE nmd, char nPort,  
                    const char *chIP) {
   HRESULT hr=ZFX_OK;

   m_hWnd  = hWnd;
   m_hInst = hInst;
   m_nmd   = nmd;
   m_nPort = nPort;

   // copy server IP
   strcpy(m_chIP, chIP);

   m_pTimer = new ZFXTimer(12, 0);

   // establish network connection
   if (FAILED (hr = InitNetwork())) {
      if (nmd == NMD_CLIENT) 
         return ZFX_NOSERVERFOUND;
      else return hr;
      }

   // cranck up engine
   if (FAILED (hr = InitInput())) return hr;
   if (FAILED (hr = InitRenderer())) return hr;
   if (FAILED (hr = InitAudio())) return hr;

   // show the window now
   ShowWindow(hWnd, SW_SHOW);

   // initialize omni shaders
   if (m_pRDevice->CanDoShaders())
      if (FAILED (hr = InitShaders())) return hr;

   // create a levelobject
   m_pLevel = new CGameLevel(m_pRDevice);

   // load a level into the object
   if ( !m_pLevel->Load("data\\level\\Aliens Maincomplex.pbl") ) {
      SAFE_DELETE(m_pLevel);
      return ZFX_FAIL;
      }

   // security check
   if ( !m_pLevel->AllPortalsConnectedOk() )
      strcpy(m_chWarn, "Warning: Not all portals are connected");
     
   if (m_pRDevice->CanDoShaders()) {
      m_pLevel->BroadcastShaders(m_BaseVSH,  m_BasePSH,
                                 m_OmniVSH,  m_OmniPSH,
                                 m_DepthVSH, m_DepthPSH);
      }

   // create player object for the server
   if (nmd==NMD_SERVER) {
      m_NID = 0;  
      AddPlayer(0);
      m_pMe = m_pPlayers[0];
      }

   m_pBullets = new CGameBulletList( m_pRDevice, m_pADevice );
   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


// call this on exiting the application
void CGame::Release(void) {
   SAFE_DELETE(m_pRenderer);
   SAFE_DELETE(m_pInput);
   SAFE_DELETE(m_pNetwork);
   SAFE_DELETE(m_pAudio);
   SAFE_DELETE(m_pTimer);
   SAFE_DELETE(m_pLevel);
   SAFE_DELETE(m_pBullets);

   for (UINT i=0; i<m_NumP; i++) 
      SAFE_DELETE(m_pPlayers[i]);
   } // Release
/*----------------------------------------------------------------*/
 

void CGame::Update(void) {
   float fET = m_pTimer->GetElapsed();

   m_pTimer->Update();
   m_pLevel->Update( fET );

   m_pBullets->Update( m_pLevel, 
                       (LPGAMECHARACTER*)
                       m_pPlayers, m_NumP,
                       NULL, 0, fET );

   m_fDT += fET;

   ProcessNetwork();
   ProcessInput();

   // update positions on network
   if ( m_fDT >= 0.01f ) {
      UpdateNetwork();
      m_fDT = 0.0f;
      }

   // update player movement and stuff
   for (UINT i=0; i<m_NumP; i++) {
      m_pPlayers[i]->Update( m_pLevel, 
                             m_pPlayers[i] == m_pMe, 
                             (LPGAMECHARACTER*)
                             m_pPlayers,
                             m_NumP, NULL, 0, fET );
      }

   m_vcPos.Set(m_pMe->GetPos().x, 
               m_pMe->GetPos().y - 0.25f,
               m_pMe->GetPos().z);

   } // Update
/*----------------------------------------------------------------*/
   
   
HRESULT CGame::Render(void) { 
   HRESULT hr=ZFX_FAIL;
   float   fB=0.0f;
   UINT    NumTris=0, 
           NumLights=0;
     
   // set first viewport (whole window)
   m_pRDevice->SetClearColor(0.0f,0.0f,0.0f);

   // update cam position
   m_pRDevice->SetView3D(m_pMe->GetRight(),
                         m_pMe->GetUp(),
                         m_pMe->GetDir(),
                         m_pMe->GetPos());

   m_pRDevice->SetMode(EMD_PERSPECTIVE, 0);

   m_pRDevice->BeginRendering(true,true,true);

RenderPlayers();
m_pBullets->Render( m_pMe->GetDir() );

//      m_pLevel->RenderOctree(false, true);
      m_pLevel->RenderPortals( m_pMe->GetPos(), true );

      hr = m_pLevel->Render( m_pMe->GetCS(), &NumTris, &NumLights, m_vcPos );
      if (FAILED( hr )) {
         m_pRDevice->DrawText(m_nZFXFont,10,10,255,255,255, 
                              "CGameLevel::Render failed");
         m_pRDevice->EndRendering();
         return hr;
         }
      else {

         // render the shadows
         if (m_bShadows) {
            hr = m_pLevel->RenderShadows(m_pMe->GetCS(), m_vcPos);
            
            if (FAILED( hr) ) {
               m_pRDevice->DrawText(m_nZFXFont,10,25,255,255,255, 
                              "CGameLevel::RenderShadows failed");
               m_pRDevice->EndRendering();
               return hr;
               }
            }

         // if frozen render some blue on top
         if (m_pMe->IsFrozen()) {
            fB = 0.3f + (0.1f/(rand()%10));
            m_pRDevice->FadeScreen(0,0,fB,0.5f);
            }

         // render some statistics
         m_pRDevice->DrawText(m_nZFXFont,10,10,255,255,255, 
                              "FPS: %f   Height: %f   Sector: %d", 
                              m_pTimer->GetFPS(),
                              m_pMe->GetPos().y, m_pMe->GetCS());

         m_pRDevice->DrawText(m_nZFXFont,10,25,255,255,255, 
                              "Tris: %d (per sec: %d)   Lights: %d", 
                              NumTris, NumTris *
                              ((int)m_pTimer->GetFPS()),
                              NumLights);

         m_pRDevice->DrawText(m_nZFXFont,10,40,255,255,255, 
                              "Collision Vector: (%2.5f %2.5f %2.5f)", 
                              m_vcTemp.x, m_vcTemp.y, m_vcTemp.z);

         m_pRDevice->DrawText(m_nZFXFont,10,55,255,255,255, m_chMsg);

         m_pRDevice->DrawText(m_nZFXFont,10,70,255,255,255, 
                              "Energy: %d", m_pMe->GetEnergy());

         m_pRDevice->DrawText(m_nZFXFont,10,85,255,255,255, m_chWarn);

         
         m_pRDevice->DrawText(m_nZFXFont,10,100,255,255,255, "Position: %f %f %f",
                              m_pMe->GetPos().x, m_pMe->GetPos().y, m_pMe->GetPos().z );

         }

   m_pRDevice->EndRendering();
   return ZFX_OK;
   } // Render
/*----------------------------------------------------------------*/


// Startup ZFXRenderer and ZFXRenderDevice
HRESULT CGame::InitRenderer(void) {
   HRESULT  hr=ZFX_OK;
   
   // RENDER DEVICE:
   // ==============
   m_pRenderer = new ZFXRenderer(m_hInst);
   if (FAILED( m_pRenderer->CreateDevice("Direct3D") ))
      return ZFX_FAIL;

   if ( !(m_pRDevice = m_pRenderer->GetDevice()) )
      return ZFX_FAIL;

   // init render device
   if (FAILED( hr = m_pRDevice->Init(m_hWnd, NULL, 0, 16, 8, true) )) {
      SAFE_DELETE(m_pRenderer);
      return hr;
      }
 
   // create a font object to render stats
   if (FAILED( m_pRDevice->CreateFont("Arial", 0, false, false,
                                      false, 8, &m_nZFXFont) )) {
      return ZFX_FAIL;
      }

   return ZFX_OK;
   } // InitRenderer
/*----------------------------------------------------------------*/


// Startup ZFXInput and ZFXInputDevice
HRESULT CGame::InitInput(void) {
   HRESULT  hr=ZFX_OK;
 
   // build mouse cage
   RECT rcCage = { 0, 0, 0, 0 };
   HDC hdc = GetDC(NULL);
   rcCage.right  = GetDeviceCaps(hdc, HORZRES) - 1;
   rcCage.bottom = GetDeviceCaps(hdc, VERTRES) - 1;
   ReleaseDC(NULL, hdc);

   // INPUT DEVICE:
   // =============
   m_pInput = new ZFXInput(m_hInst);
   if (FAILED( m_pInput->CreateDevice() ))
      return ZFX_FAIL;

   if ( !(m_pIDevice = m_pInput->GetDevice()) )
      return ZFX_FAIL;

   // init render device
   if (FAILED( hr = m_pIDevice->Init(m_hWnd, &rcCage, true) )) {
      SAFE_DELETE(m_pInput);
      return hr;
      }

   return ZFX_OK;
   } // InitInput
/*----------------------------------------------------------------*/


// Startup ZFXAudio and ZFXAudioDevice
HRESULT CGame::InitAudio(void) {
   char Buffer[MAX_PATH];

   if ( !GetCurrentDirectory( MAX_PATH-1, Buffer ) )
      {
      return ZFX_FAIL;
      }

   // create an audio objekt
   m_pAudio = new ZFXAudio(m_hInst);

   // create an audio device object
   if ( FAILED( m_pAudio->CreateDevice() )) return E_FAIL;
   
   // get a pointer on that device
   m_pADevice = m_pAudio->GetDevice();
   if(m_pADevice == NULL) return E_FAIL;

   // init audio device
   if (FAILED( m_pADevice->Init(m_hWnd, Buffer, true) )) 
      {
      return ZFX_FAIL;
      }

   return ZFX_OK;
   } // InitAudio
/*----------------------------------------------------------------*/


// startup ZFXNetwork and ZFXNetworkDevice
HRESULT CGame::InitNetwork(void) {
   HRESULT hr=ZFX_OK;

   // create a network object and device
   m_pNetwork = new ZFXNetwork(m_hInst);
   if (FAILED( m_pNetwork->CreateDevice() )) 
      return ZFX_FAIL;
   
   // get a pointer on that device
   if ( !(m_pNDevice = m_pNetwork->GetDevice()) )
      return ZFX_FAIL;

   // init input device
   if (FAILED( hr = m_pNDevice->Init(m_hWnd,       // window handle
                                     m_nmd,        // client or server
                                     m_nPort,      // port number
                                     m_chIP,       // server's IP
                                     MAX_PKG_SIZE, // max package size
                                     32,           // max num of clients
                                     true) )) {
      SAFE_DELETE(m_pNetwork);
      return hr;
      }

   return ZFX_OK;
   } // InitNetwork
/*----------------------------------------------------------------*/


// Create the shaders for the omni light
HRESULT CGame::InitShaders(void) {
   HRESULT  hr=ZFX_OK;
 
   if (!m_pRDevice->CanDoShaders()) return hr;

   const char DepthVSH[] =
         "vs.1.1                 \n"\
         "dcl_position0 v0       \n"\
         "dcl_normal0   v3       \n"\
         "dcl_texcoord0 v6       \n"\
         "m4x4 oPos, v0, c0      \n"\
         "mov oD0, c4            \n";

   const char DepthPSH[] =
         "ps.1.1                 \n"\
         "mov r0, v0             \n";

   const char BaseVSH[] =
         "vs.1.1                 \n"\
         "dcl_position0 v0       \n"\
         "dcl_normal0   v3       \n"\
         "dcl_texcoord0 v6       \n"\
         "m4x4 oPos, v0, c0      \n"\
         "mov oD0, c4            \n"\
         "mov oT0, v6            \n";

   const char BasePSH[] =
         "ps.1.1                 \n"\
         "tex t0                 \n"\
         "mul r1, t0, c3         \n"\
         "mad r0, t0, v0, r1     \n";

   const char OmniVSH[] =
         "vs.1.1                 \n"\
         "dcl_position0 v0       \n"\
         "dcl_normal0   v3       \n"\
         "dcl_texcoord0 v6       \n"\
         "m4x4 oPos, v0, c0      \n"\
         "mov oT0, v6            \n"\
         "m4x4 oT1, v0, c20      \n";

   const char OmniPSH[] =
         "ps.1.1                       \n"\
         "tex      t0                  \n"\
         "texcoord t1                  \n"\
         "dp3_sat r0, t1_bx2, t1_bx2	\n"\
         "mul r0, c0, 1-r0             \n"\
         "mul r0, r0, t0               \n";

   hr = m_pRDevice->CreateVShader(DepthVSH, sizeof(DepthVSH), 
                                  false, false, &m_DepthVSH);
   if (FAILED(hr)) return hr;

   hr = m_pRDevice->CreatePShader(DepthPSH, sizeof(DepthPSH), 
                                  false, false, &m_DepthPSH);
   if (FAILED(hr)) return hr;

   hr = m_pRDevice->CreateVShader(BaseVSH, sizeof(BaseVSH), 
                                  false, false, &m_BaseVSH);
   if (FAILED(hr)) return hr;

   hr = m_pRDevice->CreatePShader(BasePSH, sizeof(BasePSH), 
                                  false, false, &m_BasePSH);
   if (FAILED(hr)) return hr;

   hr = m_pRDevice->CreateVShader(OmniVSH, sizeof(OmniVSH), 
                                  false, false, &m_OmniVSH);
   if (FAILED(hr)) return hr;

   hr = m_pRDevice->CreatePShader(OmniPSH, sizeof(OmniPSH), 
                                  false, false, &m_OmniPSH);
   if (FAILED(hr)) return hr;

   return ZFX_OK;
   } // InitShaders
/*----------------------------------------------------------------*/


// process the user's input and apply it to movement controller
HRESULT CGame::ProcessInput(void) {
   // get input device states
   if (FAILED( m_pIDevice->Update() ))
      return ZFX_FAIL;

   // get change of mouse position
   POINT ptMouse = m_pIDevice->GetMouseDelta();

   // check for V button
   if (m_pIDevice->IsReleased(IDV_KEYBOARD, ZVK_V))
      m_bShadows = !m_bShadows;

   // check for ESC button
   if (m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_ESCAPE))
      m_bDone = true;

   // check for RETURN button
   if (m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_RETURN))
      m_pMe->TouchAndUse();

   // check for CTRL button
   if (m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_RCTRL))
      m_pMe->FireBullet();

   // rotate left and right (on Y axis)
   m_pMe->SetRotaSpeedY(-((float)ptMouse.x)*0.5f);
   
   // rotate up and down (on X axis)
   m_pMe->SetRotaSpeedX(-((float)ptMouse.y)*0.5f);

   // move forwards and backwards
   if ( m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_UP) ||
        m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_W) ) {
      m_pMe->SetSpeed(4.0f);
      }
   else if (m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_DOWN) ||
            m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_S) ) {
      m_pMe->SetSpeed(-4.0f);
      }
   else m_pMe->SetSpeed(0.0f);

   // strafe left or right
   if ( m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_LEFT) ||
        m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_A) ) {
      m_pMe->SetSlideSpeed(-2.0f);
      }
   else if (m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_RIGHT) ||
            m_pIDevice->IsPressed(IDV_KEYBOARD, ZVK_D) ) {
      m_pMe->SetSlideSpeed(2.0f);
      }
   else m_pMe->SetSlideSpeed(0.0f);

   return ZFX_OK;
   } // ProcessInput
/*----------------------------------------------------------------*/


/**
 * Check if there is a pakage waiting for us and process it if true.
 */
HRESULT CGame::ProcessNetwork(void) {
   HRESULT    hr=ZFX_OK;
   ZFXPACKAGE msgPkg = { 0, 0, 0, 0 };
   UINT       nNID=0;

   BYTE pBuffer[MAX_PKG_SIZE];
   memset(pBuffer, 0, MAX_PKG_SIZE);
   msgPkg.pData = pBuffer;

   // as long as packages are waiting for us 
   while (m_pNDevice->IsPkgWaiting()) {
      if (FAILED( m_pNDevice->GetNextPkg(&msgPkg) )) {
         hr = ZFX_FAIL;
         }
      else {
         // receiving our ID from server
         if (msgPkg.nType == 0) {
            m_NID = *((UINT*)msgPkg.pData);
            AddPlayer(m_NID);
            m_pMe = m_pPlayers[m_NumP-1];
            }
         // be advised that a new client has connected
         else if (msgPkg.nType == 1) {
            nNID = *((UINT*)msgPkg.pData);
            sprintf(m_chMsg, "new client has connected <%d>", nNID);
            if (m_NID!=nNID) AddPlayer(nNID);

            // inform new client about all 
            // already connected players
            if (m_nmd==NMD_SERVER) SendMsg_PlayerListTo(nNID);
            }
         // be advised that a client has disconnected
         else if (msgPkg.nType == 2) {
            nNID = *((UINT*)msgPkg.pData);
            sprintf(m_chMsg, "client <%d> has disconnected", nNID);
            RemovePlayer(nNID);
            }

         // receiving update package from one player
         else if (msgPkg.nType == MSG_UPDATE) {
            nNID = msgPkg.nSender;
            UpdatePlayer(nNID, (UPDATEPKG*)msgPkg.pData);

            if (m_nmd==NMD_SERVER) {
               m_pNDevice->SendToClients(&msgPkg);
               }
            }

         // receiving player list from server
         else if (msgPkg.nType == MSG_PLAYERLIST) {
            InitPlayerListOnClient((CHARACTERLIST*)msgPkg.pData);
            }

         // receiving touch and use message from one player
         else if (msgPkg.nType == MSG_TOUCHANDUSE) {
            m_pLevel->TouchAndUse( *((ZFXVector*)msgPkg.pData) );
            if (m_nmd==NMD_SERVER) {
               m_pNDevice->SendToClients(&msgPkg);
               }
            }

         // receiving fire bullet message from one player
         else if (msgPkg.nType == MSG_FIREBULLET) {
            m_pBullets->AddBullet( *((BULLET*)msgPkg.pData) );
            if (m_nmd==NMD_SERVER) {
               m_pNDevice->SendToClients(&msgPkg);
               }
            }

         // receiving fire bullet message from one player
         else if (msgPkg.nType == MSG_PLAYERFROZEN) {
            UINT nBy = *((UINT*)msgPkg.pData);
            sprintf(m_chMsg, "Player <%d> was frozen by Player <%d>",
                    msgPkg.nSender, nBy);
            if (m_nmd==NMD_SERVER) {
               m_pNDevice->SendToClients(&msgPkg);
               }
            }
         
         } // get pkg succeeded
      } // while msg incoming

   return hr;
   } // ProcessNetwork
/*----------------------------------------------------------------*/


/**
 * Create a new instance for a new connected client
 */
HRESULT CGame::AddPlayer(UINT CID) {
   // security check
   if (m_NumP >= (MAX_PCS-1))
      return ZFX_OUTOFMEMORY;

   m_pPlayers[m_NumP] = new CGamePlayer(
                                 "data/monsters/alien.cbf", 
                                 m_pRDevice, 
                                 m_pNDevice,
                                 m_BaseVSH, 
                                 m_BasePSH,
                                 m_pLevel);
   
   m_pPlayers[m_NumP]->SetID(CID);
   m_pPlayers[m_NumP]->SetTouchDown(-0.35f);

   m_NumP++;
   return ZFX_OK;
   } // AddPlayer
/*----------------------------------------------------------------*/


/**
 * Remove a player instance on client disconnect
 */
HRESULT CGame::RemovePlayer(UINT CID) {
   UINT i=0;

   // loop through players and find the right one
   for (i=0; i<m_NumP; i++) {
      if (m_pPlayers[i]->GetID() == CID) 
         break;
      }

   if (i>=m_NumP) return ZFX_INVALIDID;

   SAFE_DELETE( m_pPlayers[i] );
   
   if (i < (m_NumP-1)) 
      m_pPlayers[i] = m_pPlayers[m_NumP-1];

   m_NumP--;
   return ZFX_OK;
   } // RemovePlayer
/*----------------------------------------------------------------*/


/**
 * Render all players except for myself :)
 */
HRESULT CGame::RenderPlayers(void) {
   HRESULT hr=ZFX_OK;

   // loop through players and render'em
   for (UINT i=0; i<m_NumP; i++) {
      if (m_pPlayers[i] == m_pMe) continue;

      if (FAILED( m_pPlayers[i]->Render() ))
         hr = ZFX_FAIL;
      }

   return ZFX_OK;
   } // RenderPlayers
/*----------------------------------------------------------------*/


/**
 * Send our own player information over the network
 */
HRESULT CGame::UpdateNetwork(void) {
   ZFXPACKAGE msgPkg;

   UPDATEPKG upkg = m_pMe->GetUpdate();

   msgPkg.nLength   = sizeof(UPDATEPKG);
   msgPkg.nType     = MSG_UPDATE;
   msgPkg.nSender   = m_NID;
   msgPkg.pData     = &upkg;

   if (m_nmd == NMD_SERVER)
      return m_pNDevice->SendToClients(&msgPkg);
   else
      return m_pNDevice->SendToServer(&msgPkg);
   } // UpdateNetwork
/*----------------------------------------------------------------*/


/**
 * Make an update to the appropriate player.
 */
void CGame::UpdatePlayer(UINT CID, UPDATEPKG *upkg) {

   // don't update ourselves with server data
   if (CID == m_pMe->GetID()) return;

   for (UINT i=0; i<m_NumP; i++) {
      if (m_pPlayers[i]->GetID() == CID) 
         {
         m_pPlayers[i]->SetUpdate(upkg);
         break;
         }
      }
   } // UpdatePlayer
/*----------------------------------------------------------------*/


/**
 * Update list of player we got from the server.
 */
HRESULT CGame::InitPlayerListOnClient(CHARACTERLIST *pList) {
   HRESULT hr=ZFX_OK;
   UINT    NID=0;

   if (m_nmd == NMD_SERVER) return ZFX_OK;

   for (UINT i=0; i<pList->Num; i++) {
      NID = pList->upkg[i].NID;

      // dont update ourselves with server data
      if (NID == m_pMe->GetID()) continue;
      
      // add new player and update it
      if (FAILED( AddPlayer(NID) )) hr = ZFX_FAIL;
      else UpdatePlayer(NID, &pList->upkg[i]);
      }

   return hr;
   } // UpdatePlayerList
/*----------------------------------------------------------------*/


/**
 * Send a list of all conntected players to a new client.
 */
HRESULT CGame::SendMsg_PlayerListTo(UINT CID) {
   // only servers do that kind of job
   if (m_nmd != NMD_SERVER) return ZFX_OK;

   ZFXPACKAGE    msgPkg;
   CHARACTERLIST List;
   HRESULT       hr=ZFX_OK;

   List.Num = m_NumP;

   for (UINT i=0; i<m_NumP; i++)
      List.upkg[i] = m_pPlayers[i]->GetUpdate();
  

   msgPkg.nLength   = sizeof(CHARACTERLIST);
   msgPkg.nType     = MSG_PLAYERLIST;
   msgPkg.nSender   = m_NID;
   msgPkg.pData     = &List;

   return m_pNDevice->SendToClient(&msgPkg, CID);
   } // SendMsg_PlayerListTo
/*----------------------------------------------------------------*/


