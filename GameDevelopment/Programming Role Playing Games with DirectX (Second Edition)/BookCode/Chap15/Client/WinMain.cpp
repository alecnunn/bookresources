/**************************************************
WinMain.cpp
Chapter 15 Multiplayer Client Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, DINPUT8.LIB, D3DXOF.LIB,
  DXGUID.LIB, and DPLAY.LIB
**************************************************/

#include "Core_Global.h"
#include "Frustum.h"
#include "NodeTree.h"
#include "WinMain.h"
#include "resource.h"

cApp            *g_Application; // Global application pointer
cNetworkAdapter *g_Adapters;    // Global adapter list pointer

BOOL             g_Connected = FALSE;  // Connection status

///////////////////////////////////////////////////////////
// cApp functions
///////////////////////////////////////////////////////////
cApp::cApp()
{ 
  // Setup main window information
  m_Width  = 640; 
  m_Height = 480;
  m_Style  = WS_BORDER | WS_CAPTION |                         \
             WS_MINIMIZEBOX | WS_SYSMENU;
  strcpy(m_Class, "ClientClass");
  strcpy(m_Caption, "Network Client Demo by Jim Adams");

  // Clear class data
  m_Players     = NULL;
  g_Application = this;
  g_Adapters    = &m_Adapters;

  // Create a critical section for updating data
  InitializeCriticalSection(&m_UpdateCS);
}

BOOL cApp::Init()
{
  // Select a network adapter to use (or quit if selected)
  if(SelectAdapter() == FALSE)
    return FALSE;

  // Initialize the game, set video mode, load data, etc
  if(InitializeGame() == FALSE) {
    Error(FALSE, "Unable to initialize game.");
    return FALSE;
  }

  // Join networked game
  if(JoinGame() == FALSE) {
    Error(FALSE, "Unable to connect to server.");
    return FALSE;
  }

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Shutdown network
  m_Adapters.Shutdown();
  m_Client.Disconnect();
  m_Client.Shutdown();

  // Free players
  delete [] m_Players;
  m_Players = NULL;

  // Free meshes
  m_NodeTreeMesh.Free();
  m_TerrainMesh.Free();

  m_CharacterMesh.Free();
  m_WeaponMesh.Free();
  m_CharacterAnim.Free();

  // Shutdown input
  m_Keyboard.Free();
  m_Mouse.Free();
  m_Input.Shutdown();

  // Shutdown graphics
  m_Font.Free();
  m_Graphics.Shutdown();

  // Remove critical section
  DeleteCriticalSection(&m_UpdateCS);

  return TRUE;
}

BOOL cApp::Frame()
{
  static DWORD UpdateCounter  = timeGetTime();
  static long MoveAction = 0, LastMove = 0;
  static BOOL CamMoved = FALSE;
  BOOL AllowMovement;
  long Dir;
  float Angles[13] = { 0.0f, 0.0f, 1.57f, 0.785f, 3.14f,      \
                       0.0f, 2.355f, 0.0f, 4.71f, 5.495f,     \
                       0.0f, 0.0f, 3.925f };

  // Get local input every frame 
  m_Keyboard.Acquire(TRUE);  
  m_Mouse.Acquire(TRUE);     
  m_Keyboard.Read();
  m_Mouse.Read();
  
  // Handle connection screen
  if(g_Connected == FALSE || !m_Players[0].dpnidPlayer) {

    // Display connection message
    m_Graphics.Clear();
    if(m_Graphics.BeginScene() == TRUE) {
      m_Font.Print("Connecting to server...", 0, 0);
      m_Graphics.EndScene();
    }
    m_Graphics.Display();

    return TRUE;
  }

  // Store movements every frame
  if(m_Keyboard.GetKeyState(KEY_UP) == TRUE)
    MoveAction |= 1;
  if(m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE)
    MoveAction |= 2;
  if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE)
    MoveAction |= 4;
  if(m_Keyboard.GetKeyState(KEY_LEFT) == TRUE)
    MoveAction |= 8;
  
  // Store attack action
  if(m_Keyboard.GetKeyState(KEY_SPACE) == TRUE)
    MoveAction |= 16;
  if(m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE)
    MoveAction |= 16;

  // Rotate camera
  if(m_Mouse.GetXDelta() > 0) {
    m_CamAngle -= 0.1f;
    CamMoved = TRUE;
  }
  if(m_Mouse.GetXDelta() < 0) {
    m_CamAngle += 0.1f;
    CamMoved = TRUE;
  }

  // Only update players every 33ms (30 times a second)
  if(timeGetTime() < UpdateCounter + 33)
    return TRUE;

  // Set flag to allow player movement
  AllowMovement = TRUE;

  // Don't allow movement if still swinging weapon
  if(m_Players[0].State == STATE_SWING)
    AllowMovement = FALSE;

  // Don't allow movement if still being hurt
  if(m_Players[0].State == STATE_HURT)
    AllowMovement = FALSE;

  // Handle movements if allowed
  if(AllowMovement == TRUE) {

    // Process attack
    if(MoveAction & 16) {
      MoveAction = 0;  // Clear movement
      LastMove = 0;    // Clear last movement

      // Send attack message - let server signal swing
      sStateChangeMessage Msg;
      Msg.Header.Type     = MSG_STATE_CHANGE;
      Msg.Header.Size     = sizeof(sStateChangeMessage);
      Msg.Header.PlayerID = m_Players[0].dpnidPlayer;
      Msg.State           = STATE_SWING;
      Msg.Direction       = m_Players[0].Direction;

      // Send message to server
      SendNetworkMessage(&Msg, DPNSEND_NOLOOPBACK);
    }

    // Process local player movements
    if((Dir = MoveAction) > 0 && Dir < 13) {

      // Set new player state (w/time and direction)
      EnterCriticalSection(&m_UpdateCS);
      m_Players[0].State     = STATE_MOVE;
      m_Players[0].Direction = Angles[Dir] - m_CamAngle + 4.71f;
      LeaveCriticalSection(&m_UpdateCS);

      // Reset last move if camera moved since last update
      if(CamMoved == TRUE) {
        CamMoved = FALSE;
        LastMove = 0;
      }

      // Send actions to server if changed from last move
      if(MoveAction != LastMove) {
        LastMove = MoveAction;  // Store last action

        m_Players[0].Time = timeGetTime();

        // Construct message
        sStateChangeMessage Msg;
        Msg.Header.Type     = MSG_STATE_CHANGE;
        Msg.Header.Size     = sizeof(sStateChangeMessage);
        Msg.Header.PlayerID = m_Players[0].dpnidPlayer;
        Msg.State           = STATE_MOVE;
        Msg.Direction       = m_Players[0].Direction;

        // Send message to server
        SendNetworkMessage(&Msg, DPNSEND_NOLOOPBACK);
      }
    } else {
      // Change to idle state
      EnterCriticalSection(&m_UpdateCS);
      m_Players[0].State = STATE_IDLE;
      LeaveCriticalSection(&m_UpdateCS);

      // Send update only if player moved last update
      if(LastMove) {
        LastMove = 0;

        sStateChangeMessage Msg;
        Msg.Header.Type     = MSG_STATE_CHANGE;
        Msg.Header.Size     = sizeof(sStateChangeMessage);
        Msg.Header.PlayerID = m_Players[0].dpnidPlayer;
        Msg.State           = STATE_IDLE;
        Msg.Direction       = m_Players[0].Direction;

        // Send message to server
        SendNetworkMessage(&Msg, DPNSEND_NOLOOPBACK);
      }
    }
  }

  // Update all players
  UpdatePlayers();

  // Render the scene
  RenderScene();

  MoveAction = 0; // Clear action data for next frame

  UpdateCounter = timeGetTime(); // Reset update counter

  return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();
}

///////////////////////////////////////////////////////////
// Select adapter dialog and other functions
///////////////////////////////////////////////////////////
BOOL CALLBACK ConnectDialogProc(HWND hWnd, UINT uMsg,         \
                                WPARAM wParam, LPARAM lParam)
{
  int Selection;
  long i;
  char Name[32], IP[16], Text[256];
  static HWND hAdapters;

  switch(uMsg) {
    case WM_INITDIALOG:
      // Get adapter list window handle
      hAdapters = GetDlgItem(hWnd, IDC_ADAPTERS);

      // Add adapter names to list
      for(i=0;i<g_Adapters->GetNumAdapters();i++) {
        g_Adapters->GetName(i, Text);
        SendMessage(hAdapters, CB_INSERTSTRING,i,(LPARAM)Text);
      }

      // Select first adapter in list
      SendMessage(hAdapters, CB_SETCURSEL, 0, 0);

      // Clear fields
      SetWindowText(GetDlgItem(hWnd, IDC_HOSTIP), "192.168.0.2");
      SetWindowText(GetDlgItem(hWnd, IDC_NAME), "");

      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {

        case IDC_OK:
          // Make sure an adapter was selected
          if((Selection = SendMessage(hAdapters,              \
                                      CB_GETCURSEL,           \
                                      0, 0)) == LB_ERR)
            break;

          // Make sure there's an IP entered
          GetWindowText(GetDlgItem(hWnd, IDC_HOSTIP), IP, 16);
          if(!strlen(IP))
            break;

          // Make sure there's a name entered
          GetWindowText(GetDlgItem(hWnd, IDC_NAME), Name, 32);
          if(!strlen(Name))
            break;

          // Assign Adapter, IP, and Name
          if(g_Application != NULL)
            g_Application->SetInfo(                           \
                    g_Adapters->GetGUID(Selection), IP, Name);

          // Quit dialog
          EndDialog(hWnd, TRUE);
          return TRUE;

        case IDC_CANCEL:
          EndDialog(hWnd, FALSE);
          return TRUE;
      }

  }
  return FALSE;
}

void cApp::SetInfo(GUID *Adapter, char *IP, char *Name)
{
  m_guidAdapter = Adapter;
  strcpy(m_HostIP, IP);
  strcpy(m_Name, Name);
}

///////////////////////////////////////////////////////////
// Misc. application functions
///////////////////////////////////////////////////////////
long cApp::GetPlayerNum(DPNID dpnidPlayer)
{
  long i;

  // Error checking
  if(m_Players == NULL)
    return -1;

  // Scan list looking for match
  for(i=0;i<MAX_PLAYERS;i++) {
    if(m_Players[i].dpnidPlayer == dpnidPlayer &&             \
       m_Players[i].Connected == TRUE)
      return i;
  }

  return -1;  // Not found in list
}

///////////////////////////////////////////////////////////
// Network send and receive functions
///////////////////////////////////////////////////////////
BOOL cApp::SendNetworkMessage(void *Msg, long SendFlags)
{
  sMessageHeader *mh = (sMessageHeader*)Msg;
  unsigned long Size;

  // Get size of message to send
  if((Size = mh->Size) == 0)
    return FALSE;

  // Send the mesage
  return m_Client.Send(Msg, Size, SendFlags);
}

BOOL cApp::Receive(DPNMSG_RECEIVE *Msg)
{
  sMessage *MsgPtr;

  // Get pointer to received data
  MsgPtr = (sMessage*)Msg->pReceiveData;

  // Handle packets by type
  switch(MsgPtr->Header.Type) {
    case MSG_PLAYER_INFO:    // Add a player to list
    case MSG_CREATE_PLAYER:
      CreatePlayer(MsgPtr);
      break;

    case MSG_DESTROY_PLAYER: // Remove a player from list
      DestroyPlayer(MsgPtr);
      break;

    case MSG_STATE_CHANGE:   // Change state of player
      ChangeState(MsgPtr);
      break;
  }

  return TRUE;
}

BOOL cApp::SetLocalPlayerID(DPNID dpnid)
{
  if(m_Players == NULL)
    return FALSE;

  EnterCriticalSection(&m_UpdateCS);
  m_Players[0].dpnidPlayer = dpnid;
  LeaveCriticalSection(&m_UpdateCS);

  return TRUE;
}

///////////////////////////////////////////////////////////
// Message parsing functions
///////////////////////////////////////////////////////////
void cApp::CreatePlayer(sMessage *Msg)
{
  sCreatePlayerMessage *cpm;
  long PlayerNum, i;

  // Error checking
  if(m_Players == NULL || !m_Players[0].dpnidPlayer)
    return;

  // Get pointer to message data
  cpm = (sCreatePlayerMessage*)Msg;

  // Don't add local player to list
  if(cpm->Header.PlayerID == m_Players[0].dpnidPlayer)
    return;

  // Make sure player not already in list while at
  // same time finding an empty slot.
  PlayerNum = -1;
  for(i=1;i<MAX_PLAYERS;i++) {
    if(m_Players[i].Connected == TRUE) {
      if(m_Players[i].dpnidPlayer==cpm->Header.PlayerID)
        return;
    } else
      PlayerNum = i;
  }

  // Return error if no open slots
  if(PlayerNum == -1)
    return;

 	// Enter critical section
  EnterCriticalSection(&m_UpdateCS);

  // Add player data
  m_Players[PlayerNum].Connected   = TRUE;
  m_Players[PlayerNum].dpnidPlayer = cpm->Header.PlayerID;
  m_Players[PlayerNum].XPos        = cpm->XPos;
  m_Players[PlayerNum].YPos        = cpm->YPos;
  m_Players[PlayerNum].ZPos        = cpm->ZPos;
  m_Players[PlayerNum].Direction   = cpm->Direction;
  m_Players[PlayerNum].Speed       = 0.0f;
  m_Players[PlayerNum].State       = STATE_IDLE;

  m_NumPlayers++;

  // Leave critical section
	LeaveCriticalSection(&m_UpdateCS);
}

void cApp::DestroyPlayer(sMessage *Msg)
{
  sDestroyPlayerMessage *dpm;
  long PlayerNum;

  // Error checking
  if(m_Players == NULL || !m_Players[0].dpnidPlayer)
    return;

  // Get pointer to message data
  dpm = (sDestroyPlayerMessage*)Msg;

  // Don't remove local player from list
  if(dpm->Header.PlayerID == m_Players[0].dpnidPlayer)
    return;

  // Get player number in list
  if((PlayerNum = GetPlayerNum(dpm->Header.PlayerID)) == -1)
    return;

	// Enter critical section
  EnterCriticalSection(&m_UpdateCS);

  // Set player as disconnected
  m_Players[PlayerNum].Connected = FALSE;
  m_NumPlayers--;

  // Leave critical section
	LeaveCriticalSection(&m_UpdateCS);
}

void cApp::ChangeState(sMessage *Msg)
{
  sStateChangeMessage *scm;
  sRequestPlayerInfoMessage rpim;
  long PlayerNum;

  // Error checking
  if(m_Players == NULL || !m_Players[0].dpnidPlayer)
    return;

  // Get pointer to message data
  scm = (sStateChangeMessage*)Msg;

  // Get player number in list
  if((PlayerNum = GetPlayerNum(scm->Header.PlayerID)) == -1) {

    // Unknown player - request info 
    if(PlayerNum == -1) {
  
      // Construct message
      rpim.Header.Type = MSG_PLAYER_INFO;
      rpim.Header.Size = sizeof(sRequestPlayerInfoMessage);
      rpim.Header.PlayerID = m_Players[0].dpnidPlayer;
      rpim.PlayerID = scm->Header.PlayerID;

      // Send message to server
      SendNetworkMessage(&rpim, DPNSEND_NOLOOPBACK);

      return;
    }
  }

	// Enter critical section
  EnterCriticalSection(&m_UpdateCS);

  // Store new sytate info
  m_Players[PlayerNum].Time      = timeGetTime();
  m_Players[PlayerNum].State     = scm->State;
  m_Players[PlayerNum].XPos      = scm->XPos;
  m_Players[PlayerNum].YPos      = scm->YPos;
  m_Players[PlayerNum].ZPos      = scm->ZPos;
  m_Players[PlayerNum].Direction = scm->Direction;
  m_Players[PlayerNum].Speed     = scm->Speed;
  m_Players[PlayerNum].Latency   = scm->Latency;

  // Bounds latency to 1 second
  if(m_Players[PlayerNum].Latency > 1000)
    m_Players[PlayerNum].Latency = 1000;

  // Adjust time based on latency
  m_Players[PlayerNum].Time -= m_Players[PlayerNum].Latency;

  // Leave critical section
	LeaveCriticalSection(&m_UpdateCS);
}

///////////////////////////////////////////////////////////
// App initialization functions
///////////////////////////////////////////////////////////
BOOL cApp::SelectAdapter()
{
  int Result;

  // Hide main window
  ShowWindow(GethWnd(), SW_HIDE);

  // Build a list of network adapters
  m_Adapters.Init();

  // Open connection dialog
  Result=DialogBox(GethInst(), MAKEINTRESOURCE(IDD_CONNECT),  \
                   GethWnd(), ConnectDialogProc);

  // Don't continue if quit selected
  if(Result == FALSE)
    return FALSE;

  // Show main window
  ShowWindow(GethWnd(), SW_SHOW);

  // Continue if user selected OK
  return TRUE;
}

BOOL cApp::InitializeGame()
{
  long i;

  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
  m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 20000.0f);
  ShowMouse(FALSE);

  // Get a font
  m_Font.Create(&m_Graphics, "Arial", 16);

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);
  m_Mouse.Create(&m_Input, MOUSE, TRUE);

  // Load the mesh and create an NodeTree mesh from it
  m_TerrainMesh.Load(&m_Graphics, "..\\Data\\Arena.x",        \
                                  "..\\Data\\");
  m_NodeTreeMesh.Create(&m_Graphics, &m_TerrainMesh, QUADTREE);

  // Load the meshes and animations
  m_CharacterMesh.Load(&m_Graphics, "..\\Data\\Warrior.x",    \
                                    "..\\Data\\");
  m_WeaponMesh.Load(&m_Graphics, "..\\Data\\Sword.x", "..\\Data\\");
  m_CharacterAnim.Load("..\\Data\\Warrior.x", &m_CharacterMesh);
  m_CharacterAnim.SetLoop(TRUE, "Walk");
  m_CharacterAnim.SetLoop(TRUE, "Idle");
  m_CharacterAnim.SetLoop(FALSE, "Swing");
  m_CharacterAnim.SetLoop(FALSE, "Hurt");

  // Set the camera angle
  m_CamAngle = 0.0f;

  // Create player structures
  m_Players = new sPlayer[MAX_PLAYERS]();

  // Setup player data
  for(i=0;i<MAX_PLAYERS;i++) {
    m_Players[i].Body.Create(&m_Graphics, &m_CharacterMesh);
    m_Players[i].Weapon.Create(&m_Graphics, &m_WeaponMesh);
    m_Players[i].Weapon.AttachToObject(&m_Players[i].Body,    \
                                       "Bip01_R_Finger11");
    m_Players[i].Weapon.Rotate(1.57f, 0.0f, 0.0f);
  }

  // Setup local player structure
  m_NumPlayers = 1;
  m_Players[0].Connected = TRUE;
  m_Players[0].Direction = 0.0f;
  m_Players[0].XPos = 0.0f;
  m_Players[0].YPos = 0.0f;
  m_Players[0].ZPos = 0.0f;
  m_Players[0].Speed = 512.0f;
  m_Players[0].State = STATE_IDLE;

  return TRUE;
}

BOOL cApp::JoinGame()
{
  // Initialize network and try to connect to host
  m_Client.Init();
  if(m_Client.Connect(m_guidAdapter, m_HostIP, 9123,          \
                      m_Name, "RPGGAME", NULL) == FALSE)
    return FALSE;

  return TRUE;  // Return success
}

///////////////////////////////////////////////////////////
// Game logic code
///////////////////////////////////////////////////////////
void cApp::UpdatePlayers()
{
  long  i;
  float XMove, ZMove, Dist, Speed;
  long Elapsed;

  // Process all active player movements
  for(i=0;i<MAX_PLAYERS;i++) {
    if(m_Players[i].Connected == TRUE) {

      // Get elapsed time from now and state time
      Elapsed = timeGetTime() - m_Players[i].Time;

      // Process player movement state
      if(m_Players[i].State == STATE_MOVE) {
        // Calculate amount of movement by time movement processed
        Speed = (float)Elapsed / 1000.0f * m_Players[i].Speed;
        XMove = (float)sin(m_Players[i].Direction) * Speed;
        ZMove = (float)cos(m_Players[i].Direction) * Speed;

        // Check for movement collisions -
        // can't walk past anything blocking path
        if(m_NodeTreeMesh.CheckIntersect(
                            m_Players[i].XPos,
                            m_Players[i].YPos + 16.0f,
                            m_Players[i].ZPos,
                            m_Players[i].XPos + XMove,
                            m_Players[i].YPos + 16.0f,
                            m_Players[i].ZPos + ZMove,
                            &Dist) == TRUE)
          XMove = ZMove = 0.0f;
      
        // Update coordinates
        EnterCriticalSection(&m_UpdateCS);
        m_Players[i].XPos += XMove;
        m_Players[i].YPos = 0.0f;
        m_Players[i].ZPos += ZMove;

        m_Players[i].Time = timeGetTime(); // Reset time

        LeaveCriticalSection(&m_UpdateCS);
      } 

      // Set new animations as needed
      if(m_Players[i].State == STATE_IDLE) {
        if(m_Players[i].LastAnim != ANIM_IDLE) {
          EnterCriticalSection(&m_UpdateCS);
          m_Players[i].LastAnim = ANIM_IDLE;
          m_Players[i].Body.SetAnimation(                     \
               &m_CharacterAnim, "Idle", timeGetTime() / 32);
          LeaveCriticalSection(&m_UpdateCS);
        }
      } else
      if(m_Players[i].State == STATE_MOVE) {
        if(m_Players[i].LastAnim != ANIM_WALK) {
          EnterCriticalSection(&m_UpdateCS);
          m_Players[i].LastAnim = ANIM_WALK;
          m_Players[i].Body.SetAnimation(                     \
               &m_CharacterAnim, "Walk", timeGetTime() / 32);
          LeaveCriticalSection(&m_UpdateCS);
        }
      } else
      if(m_Players[i].State == STATE_SWING) {
        if(m_Players[i].LastAnim != ANIM_SWING) {
          EnterCriticalSection(&m_UpdateCS);
          m_Players[i].LastAnim = ANIM_SWING;
          m_Players[i].Body.SetAnimation(                     \
               &m_CharacterAnim, "Swing", timeGetTime() / 32);
          LeaveCriticalSection(&m_UpdateCS);
        }
      } else
      if(m_Players[i].State == STATE_HURT) {
        if(m_Players[i].LastAnim != ANIM_HURT) {
          EnterCriticalSection(&m_UpdateCS);
          m_Players[i].LastAnim = ANIM_HURT;
          m_Players[i].Body.SetAnimation(                     \
               &m_CharacterAnim, "Hurt", timeGetTime() / 32);
          LeaveCriticalSection(&m_UpdateCS);
        }
      }

    }
  }
}

void cApp::RenderScene()
{
  cFrustum Frustum;
  float Radius;
  long i;

  // Center camera on player using CamAngle
  m_Camera.Point(m_Players[0].XPos +                          \
                     (float)cos(m_CamAngle) * 300.0f,         \
                 m_Players[0].YPos + 100.0f,                  \
                 m_Players[0].ZPos +                          \
                     (float)sin(m_CamAngle) * 300.0f,         \
                 m_Players[0].XPos,                           \
                 m_Players[0].YPos,                           \
                 m_Players[0].ZPos);

  // Set camera and construct frustum
  m_Graphics.SetCamera(&m_Camera);
  Frustum.Construct(&m_Graphics);

  // Render scene
  m_Graphics.Clear();
  if(m_Graphics.BeginScene() == TRUE) {
    
    // Draw the terrain
    m_Graphics.EnableZBuffer(TRUE);
    m_NodeTreeMesh.Render(&Frustum);

    // Draw all characters active and in view
    for(i=0;i<MAX_PLAYERS;i++) {
      if(m_Players[i].Connected == TRUE) {

        // Bounds check if player in view
        m_Players[i].Body.GetBounds(NULL,NULL,NULL,NULL,      \
                                    NULL,NULL,&Radius);

        if(Frustum.CheckSphere(m_Players[i].XPos,             \
                               m_Players[i].YPos,             \
                               m_Players[i].ZPos,             \
                               Radius) == TRUE) {

          // Position character and rotate
          m_Players[i].Body.Move(m_Players[i].XPos,           \
                                 m_Players[i].YPos,           \
                                 m_Players[i].ZPos);
          m_Players[i].Body.Rotate(0.0f,                      \
                                   m_Players[i].Direction,    \
                                   0.0f);


          // Render body and weapon
          m_Players[i].Body.UpdateAnimation(timeGetTime()/32, \
                                            TRUE);
          m_Players[i].Body.Render();
          m_Players[i].Weapon.Render();
        }
      }
    }

    m_Graphics.EndScene();
  }

  m_Graphics.Display();  // Display the scene
}


///////////////////////////////////////////////////////////
// Client class code
///////////////////////////////////////////////////////////
BOOL cClient::ConnectComplete(DPNMSG_CONNECT_COMPLETE *Msg) 
{ 
  // Save connection status
  if(Msg->hResultCode == S_OK)
    g_Connected = TRUE;
  else
    g_Connected = FALSE;

  // Get player id # from connection complete code
  if(g_Application != NULL)
    g_Application->SetLocalPlayerID(Msg->dpnidLocal);

  return TRUE; 
}

BOOL cClient::Receive(DPNMSG_RECEIVE *Msg)
{
  // Send message to application class instance (if any)
  if(g_Application != NULL)
    g_Application->Receive(Msg);
  return TRUE;
}
