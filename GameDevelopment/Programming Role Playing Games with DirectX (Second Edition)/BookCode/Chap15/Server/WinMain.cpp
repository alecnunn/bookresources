/**************************************************
WinMain.cpp
Chapter 15 Multiplayer Server Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, DINPUT8.LIB, D3DXOF.LIB,
  DXGUID.LIB, and DPLAY.LIB
**************************************************/

#include "Core_Global.h"
#include "Server.h"
#include "WinMain.h"
#include "resource.h"

cApp            *g_Application; // Global application pointer
cNetworkAdapter *g_Adapters;    // Global adapter list pointer

///////////////////////////////////////////////////////////
// cApp overidden virtual functions
///////////////////////////////////////////////////////////
cApp::cApp()
{ 
  // Setup main window information
  m_Width  = 320;
  m_Height = 212;
  m_Style  = WS_BORDER | WS_CAPTION |                         \
             WS_MINIMIZEBOX | WS_SYSMENU;
  m_wcex.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
  strcpy(m_Class, "ServerClass");
  strcpy(m_Caption, "Network Server Demo by Jim Adams");

  // Clear class data
  m_Messages    = NULL;
  m_Players     = NULL;
  m_guidAdapter = NULL;
  g_Application = this;
  g_Adapters    = &m_Adapters;

  // Initialize a critical section for messaging
  InitializeCriticalSection(&m_MessageCS);
}

BOOL cApp::Init()
{
  // Select a network adapter to use (or quit if selected)
  if(SelectAdapter() == FALSE)
    return FALSE;

  // Setup the main interface window
  SetupApplicationWindow();

  // Initialize the game, set video mode, load data, etc
  if(InitializeGame() == FALSE) {
    Error(FALSE, "Unable to initialize game.");
    return FALSE;
  }

  // Begin hosting the game session
  if(HostGame() == FALSE) {
    Error(FALSE, "Unable to host network server.");
    return FALSE;
  }

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Disconnect network and free interfaces
  m_Adapters.Shutdown();
  m_Server.Disconnect();
  m_Server.Shutdown();

  // Shutdown graphics
  m_LevelMesh.Free();
  m_Graphics.Shutdown();

  // Delete message array
  delete [] m_Messages;
  m_Messages = NULL;

  // Delete players
  delete [] m_Players;
  m_Players = NULL;

  // Free the critical section
  DeleteCriticalSection(&m_MessageCS);

  return TRUE;
}

BOOL cApp::Frame()
{
  static DWORD PlayerCounter = timeGetTime();
  static DWORD NetworkCounter = timeGetTime();
  static DWORD LatencyCounter = timeGetTime();

  // Process some queued network messages
  ProcessQueuedMessages();

  // Update players every 33ms (30 times a second)
  if(timeGetTime() > PlayerCounter + 33) {
    UpdatePlayers();
    PlayerCounter = timeGetTime(); // Reset counter
  }

  // Send out network updates every 100ms (10 times a second)
  if(timeGetTime() > NetworkCounter + 100) {
    UpdateNetwork();
    NetworkCounter = timeGetTime(); // Reset counter
  }

  // Update player latency values every 10 seconds
  if(timeGetTime() > LatencyCounter + 10000) {
    UpdateLatency();
    LatencyCounter = timeGetTime(); // Reset counter
  }

  return TRUE;
}

FAR PASCAL cApp::MsgProc(HWND hWnd, UINT uMsg,                \
                         WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}    

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();  // Run the application instance
}

///////////////////////////////////////////////////////////
// Select adapter dialog and other functions
///////////////////////////////////////////////////////////
BOOL CALLBACK ConfigDialogProc(HWND hWnd, UINT uMsg,          \
                               WPARAM wParam, LPARAM lParam)
{
  long i;
  char Text[256];
  int Selection;
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

      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {

        case IDC_OK:
          // Make sure an adapter was selected
          if((Selection = SendMessage(hAdapters,              \
                                      CB_GETCURSEL,           \
                                      0, 0)) == LB_ERR)
            break;

          // Assign adapter
          if(g_Application != NULL)
            g_Application->SetAdapter(                        \
                           g_Adapters->GetGUID(Selection));

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

void cApp::SetAdapter(GUID *Adapter)
{
  m_guidAdapter = Adapter;
}

///////////////////////////////////////////////////////////
// Misc. application functions
///////////////////////////////////////////////////////////
void cApp::ListPlayers()
{
  long i;
  char Text[256];

  // Error checking
  if(m_Players == NULL)
    return;

  // Clear player list
  SendMessage(m_Controls[2], LB_RESETCONTENT, 0, 0);

  // Count all players and add names to list
  for(i=0;i<MAX_PLAYERS;i++) {
    if(m_Players[i].Connected == TRUE)
      SendMessage(m_Controls[2], LB_ADDSTRING, 0,             \
                 (LPARAM)m_Players[i].Name);
  }

  // Display player count
  if(!m_NumPlayers)
    SetWindowText(m_Controls[1], "No Connected Players");
  else {
    sprintf(Text, "%lu Players Connected", m_NumPlayers);
    SetWindowText(m_Controls[1], Text);
  }
}

///////////////////////////////////////////////////////////
// Network send and receive functions
///////////////////////////////////////////////////////////
BOOL cApp::SendNetworkMessage(void *Msg, long SendFlags, int To)
{
  sMessageHeader *mh = (sMessageHeader*)Msg;
  unsigned long Size;

  // Get size of message to send
  if((Size = mh->Size) == 0)
    return FALSE;

  // Set destination to all players if To == -1
  if(To == -1)
    To = DPNID_ALL_PLAYERS_GROUP;

  // Send the message
  return m_Server.Send(To, Msg, Size, SendFlags);
}

BOOL cApp::CreatePlayer(DPNMSG_CREATE_PLAYER *Msg)
{
  sCreatePlayerMessage cpm;

  // Setup message data
  cpm.Header.Type     = MSG_CREATE_PLAYER;
  cpm.Header.Size     = sizeof(sCreatePlayerMessage);
  cpm.Header.PlayerID = Msg->dpnidPlayer;

  QueueMessage(&cpm); // Queue the message
  
  return TRUE;
}

BOOL cApp::DestroyPlayer(DPNMSG_DESTROY_PLAYER *Msg)
{
  sDestroyPlayerMessage dpm;

  // Setup message data
  dpm.Header.Type     = MSG_DESTROY_PLAYER;
  dpm.Header.Size     = sizeof(sDestroyPlayerMessage);
  dpm.Header.PlayerID = Msg->dpnidPlayer;
  
  QueueMessage(&dpm); // Queue the message

  return TRUE;
}

BOOL cApp::Receive(DPNMSG_RECEIVE *Msg)
{
  sMessageHeader *mh = (sMessageHeader*)Msg->pReceiveData;

  // Make sure it's a valid message type and queue it
  switch(mh->Type) {
    case MSG_PLAYER_INFO:
    case MSG_STATE_CHANGE:
      // Add message to queue
      QueueMessage((void*)Msg->pReceiveData);
      break;
  }

  return TRUE;
}

///////////////////////////////////////////////////////////
// Message queue functions
///////////////////////////////////////////////////////////
BOOL cApp::QueueMessage(void *Msg)
{
  sMessageHeader *mh = (sMessageHeader*)Msg;

  // Error checking - make sure there's a message array
  if(m_Messages == NULL)
    return FALSE;

  // Return if no room left in queue
  if(((m_MsgHead+1) % NUM_MESSAGES) == m_MsgTail)
    return FALSE;

  // Stuff message into queue
  if(mh->Size <= sizeof(sMessage)) {
    // Start the critical section
    EnterCriticalSection(&m_MessageCS);

    memcpy(&m_Messages[m_MsgHead], Msg, mh->Size);

    // Go to next empty message (flip around if at end)
    m_MsgHead++;
    if(m_MsgHead >= NUM_MESSAGES)
      m_MsgHead = 0;

    // Leave the critical section
    LeaveCriticalSection(&m_MessageCS);
  }

  return TRUE;
}

BOOL cApp::PlayerInfo(sMessage *Msg, DPNID To)
{
  sRequestPlayerInfoMessage *rpim;
  sCreatePlayerMessage cpm;
  long i;

  // Error checking
  if(m_Players == NULL)
    return FALSE;

  // Get pointer to request info
  rpim = (sRequestPlayerInfoMessage*)Msg;

  for(i=0;i<MAX_PLAYERS;i++) {
    // Only send if found in list
    if(m_Players[i].dpnidPlayer == rpim->PlayerID &&          \
       m_Players[i].Connected == TRUE) {
 
      // Send player info to requesting player
      cpm.Header.Type     = MSG_PLAYER_INFO;
      cpm.Header.Size     = sizeof(sCreatePlayerMessage);
      cpm.Header.PlayerID = rpim->PlayerID;

      cpm.XPos            = m_Players[i].XPos;
      cpm.YPos            = m_Players[i].YPos;
      cpm.ZPos            = m_Players[i].ZPos;
      cpm.Direction       = m_Players[i].Direction;

      SendNetworkMessage(&cpm, DPNSEND_NOLOOPBACK, To);

      break;
    }
  }

  return TRUE;
}

BOOL cApp::AddPlayer(sMessage *Msg)
{
  long i;
  DWORD Size = 0;
  DPN_PLAYER_INFO *dpi = NULL;
  HRESULT hr;
  DPNID PlayerID;

  // Error checking
  if(m_Players == NULL)
    return FALSE;

  // Get the player ID #
  PlayerID = Msg->Header.PlayerID;

  // Get the player information 
  hr = m_Server.GetServerCOM()->GetClientInfo(PlayerID, dpi,  \
                                              &Size, 0);

  // Return on error or if adding server
  if(FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL)
    return FALSE;

  // Allocate player data buffer and try again
  if((dpi = (DPN_PLAYER_INFO*)new BYTE[Size]) == NULL)
    return FALSE;
  ZeroMemory(dpi, Size);
  dpi->dwSize = sizeof(DPN_PLAYER_INFO);
  if(FAILED(m_Server.GetServerCOM()->GetClientInfo(           \
                                PlayerID, dpi, &Size, 0))) {
    delete [] dpi;
    return FALSE;
  }

  // Make sure not already in list
  for(i=0;i<MAX_PLAYERS;i++) {
    if(m_Players[i].dpnidPlayer == PlayerID &&                \
       m_Players[i].Connected == TRUE) {
      delete [] dpi;
      m_Server.DisconnectPlayer(PlayerID);
      return FALSE;
    }
  }

  // Search for an empty slot to put player
  for(i=0;i<MAX_PLAYERS;i++) {
    if(m_Players[i].Connected == FALSE) {

      m_Players[i].Connected = TRUE; // Flag as connected

      // Save DirectPlay DPNID # and name of player
      m_Players[i].dpnidPlayer = PlayerID;
      wcstombs(m_Players[i].Name, dpi->pwszName, 256);

      // Setup player data
      m_Players[i].XPos      = 0.0f;
      m_Players[i].YPos      = 0.0f;
      m_Players[i].ZPos      = 0.0f;
      m_Players[i].Direction = 0.0f;
      m_Players[i].Speed     = 512.0f;
      m_Players[i].State     = STATE_IDLE;
      m_Players[i].Latency   = 0;

      // Send add player info to all players in area
      sCreatePlayerMessage cpm;
      cpm.Header.Type     = MSG_CREATE_PLAYER;
      cpm.Header.Size     = sizeof(sCreatePlayerMessage);
      cpm.Header.PlayerID = PlayerID;
      cpm.XPos            = m_Players[i].XPos;
      cpm.YPos            = m_Players[i].YPos;
      cpm.ZPos            = m_Players[i].ZPos;
      cpm.Direction       = m_Players[i].Direction;
      SendNetworkMessage(&cpm, DPNSEND_NOLOOPBACK, -1);

      // Increase number of players
      m_NumPlayers++;

      ListPlayers();        // List all players

      delete [] dpi;        // Free player data

      return TRUE;          // Return success
    }
  }

  delete[] dpi;  // Free player data

  // Disconnect player - not allowed to connect
  m_Server.DisconnectPlayer(PlayerID);

  return FALSE;  // Return failure
}

BOOL cApp::RemovePlayer(sMessage *Msg)
{
  long i;

  // Error checking
  if(m_Players == NULL)
    return FALSE;

  // Search for player in list
  for(i=0;i<MAX_PLAYERS;i++) {
    if(m_Players[i].dpnidPlayer == Msg->Header.PlayerID &&    \
       m_Players[i].Connected == TRUE) {

      m_Players[i].Connected = FALSE;  // Disconnect player

      // Send remove player message to all players in area
      sDestroyPlayerMessage dpm;
      dpm.Header.Type     = MSG_DESTROY_PLAYER;
      dpm.Header.Size     = sizeof(sDestroyPlayerMessage);
      dpm.Header.PlayerID = Msg->Header.PlayerID;
      SendNetworkMessage(&dpm, DPNSEND_NOLOOPBACK, -1);

      // Decrease number of players
      m_NumPlayers--;

      // List all players
      ListPlayers();

      return TRUE;
    }
  }

  return FALSE;  // Return failure
}

BOOL cApp::PlayerStateChange(sMessage *Msg)
{
  sStateChangeMessage *scm, uscm;
  long i, PlayerNum;
  BOOL AllowChange;
  float XDiff, ZDiff, Dist, Angle;

  // Error checking
  if(m_Players == NULL)
    return FALSE;

  // Get pointer to state change message
  scm = (sStateChangeMessage*)Msg;

  // Get player number in list
  PlayerNum = -1;
  for(i=0;i<MAX_PLAYERS;i++) {
    if(m_Players[i].dpnidPlayer == Msg->Header.PlayerID &&    \
       m_Players[i].Connected == TRUE) {
      PlayerNum = i;
      break;
    }
  }
  if(PlayerNum == -1)
    return FALSE;

  AllowChange = TRUE;  // Flag to allow changes in state

  // Refuse to update player if swinging sword
  if(m_Players[PlayerNum].State == STATE_SWING)
    AllowChange = FALSE;

  // Refuse to update player if hurt
  if(m_Players[PlayerNum].State == STATE_HURT)
    AllowChange = FALSE;

  // Only change state if allowed
  if(AllowChange == TRUE) {

    // Update selected player
    m_Players[PlayerNum].Time      = timeGetTime();
    m_Players[PlayerNum].State     = scm->State;
    m_Players[PlayerNum].Direction = scm->Direction;

    // Adjust action time based on latency
    m_Players[PlayerNum].Time -= m_Players[PlayerNum].Latency;

    // Send player data to all clients
    uscm.Header.Type     = MSG_STATE_CHANGE;
    uscm.Header.Size     = sizeof(sStateChangeMessage);
    uscm.Header.PlayerID = scm->Header.PlayerID;
    uscm.State           = m_Players[PlayerNum].State;
    uscm.XPos            = m_Players[PlayerNum].XPos;
    uscm.YPos            = m_Players[PlayerNum].YPos;
    uscm.ZPos            = m_Players[PlayerNum].ZPos;
    uscm.Direction       = m_Players[PlayerNum].Direction;
    uscm.Speed           = m_Players[PlayerNum].Speed;
    SendNetworkMessage(&uscm, DPNSEND_NOLOOPBACK);

    // If swinging sword, determine who's hurt
    if(scm->State == STATE_SWING) {

      // Check all players
      for(i=0;i<MAX_PLAYERS;i++) {
        
        // Only check against other players that are connected
        if(i != PlayerNum && m_Players[i].Connected == TRUE) {

          // Get distance to player
          XDiff = (float)fabs(m_Players[PlayerNum].XPos -     \
                              m_Players[i].XPos);
          ZDiff = (float)fabs(m_Players[PlayerNum].ZPos -     \
                              m_Players[i].ZPos);
          Dist = XDiff*XDiff + ZDiff*ZDiff;

          // Continue if distance between players acceptable
          if(Dist < 10000.0f) {
            
            // Get angle between players
            Angle = -(float)atan2(                            \
                           (m_Players[i].ZPos -               \
                            m_Players[PlayerNum].ZPos),       \
                           (m_Players[i].XPos -               \
                            m_Players[PlayerNum].XPos)) +     \
                            1.570796f;

             // Adjust for attacker's direction
            Angle -= m_Players[PlayerNum].Direction;

            Angle += 0.785f; // Adjust for FOV

            // Bounds angle value
            if(Angle < 0.0f)
              Angle += 6.28f;
            if(Angle >= 6.28f)
              Angle -= 6.28f;
                       
            // Player hit if in front of attacker (90 FOV)
            if(Angle >= 0.0f && Angle <= 1.570796f) {

              // Set victim's state to hurt (if not already)
              if(m_Players[i].State != STATE_HURT) {
                m_Players[i].State = STATE_HURT;
                m_Players[i].Time = timeGetTime();

                // Send network message
                uscm.Header.Type = MSG_STATE_CHANGE;
                uscm.Header.Size = sizeof(sStateChangeMessage);
                uscm.Header.PlayerID = m_Players[i].dpnidPlayer;
                uscm.State = m_Players[i].State;
                uscm.XPos = m_Players[i].XPos;
                uscm.YPos = m_Players[i].YPos;
                uscm.ZPos = m_Players[i].ZPos;
                uscm.Direction = m_Players[i].Direction;
                uscm.Speed = m_Players[i].Speed;
                SendNetworkMessage(&uscm, DPNSEND_NOLOOPBACK);
              }
            }
          }
        }
      }
    }
  }

  return TRUE;
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

  // Open configure dialog
  Result = DialogBox(GethInst(), MAKEINTRESOURCE(IDD_CONFIG), \
                     GethWnd(), ConfigDialogProc);

  // Don't continue if quit selected
  if(Result == FALSE)
    return FALSE;

  // Continue if user selected OK
  return TRUE;
}

void cApp::SetupApplicationWindow()
{
  // Create the windows controls

  // Window to hold host IP address
  m_Controls[0] = CreateWindow("STATIC", "",                  \
          WS_CHILD | WS_VISIBLE,                              \
          4, 4, 312, 18,                                      \
          GethWnd(), NULL, GethInst(), NULL);

  // Window to hold # of connected players
  m_Controls[1] = CreateWindow("STATIC",                      \
          "No Connected Players",                             \
          WS_CHILD | WS_VISIBLE,                              \
          4, 26, 312, 18,                                     \
          GethWnd(), NULL, GethInst(), NULL);

  // List box to display connect player's names
  m_Controls[2] = CreateWindow("LISTBOX", "",                 \
          WS_CHILD | WS_BORDER | WS_VSCROLL | WS_VISIBLE,     \
          4, 48, 312, 154,                                    \
          GethWnd(), NULL, GethInst(), NULL);   

  // Show main window
  ShowWindow(GethWnd(), SW_SHOW);
}

BOOL cApp::InitializeGame()
{
  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd());

  // Load the level mesh for collision checking
  m_LevelMesh.Load(&m_Graphics, "..\\Data\\ArenaCM.x");

  // Allocate message queue and reset message pointers
  m_Messages = new sMessage[NUM_MESSAGES]();
  m_MsgHead = m_MsgTail = 0;

  // Create player structures
  m_Players = new sPlayer[MAX_PLAYERS]();

  // Reset # of players
  m_NumPlayers = 0;

  return TRUE;
}

BOOL cApp::HostGame()
{
  char Text[33], IP[16];

  // Configure server and begin hosting
  m_Server.Init();
  if(m_Server.Host(m_guidAdapter, 9123,                       \
                  "RPGGAME", NULL, MAX_PLAYERS) == FALSE)
    return FALSE;

  // Get Server IP address and display in application window
  m_Server.GetIP(IP);
  sprintf(Text, "Host IP Address: %s", IP);
  SetWindowText(m_Controls[0], Text);

  return TRUE;  // Return success
}

///////////////////////////////////////////////////////////
// Game message queue processing and update functions
///////////////////////////////////////////////////////////
void cApp::ProcessQueuedMessages()
{
  sMessage *Msg;
  long Count = 0;

  // Pull out messages to process
  while(Count != MESSAGES_PER_FRAME && m_MsgHead != m_MsgTail) {

    // Get pointer to 'tail' message
    EnterCriticalSection(&m_MessageCS);
    Msg = &m_Messages[m_MsgTail];
    LeaveCriticalSection(&m_MessageCS);

    // Process a single message based on type
    switch(Msg->Header.Type) {
      case MSG_PLAYER_INFO:    // Request player info
        PlayerInfo(Msg, Msg->Header.PlayerID);
        break;

      case MSG_CREATE_PLAYER:  // Add a player
        AddPlayer(Msg);
        break;

      case MSG_DESTROY_PLAYER: // Remove a player
        RemovePlayer(Msg);
        break;

      case MSG_STATE_CHANGE:   // Change state of player
        PlayerStateChange(Msg);
        break;
    }

    Count++; // Increase processed message count

    // Goto next message in list
    EnterCriticalSection(&m_MessageCS);
    m_MsgTail = (m_MsgTail + 1) % NUM_MESSAGES;
    LeaveCriticalSection(&m_MessageCS);
  }
}

void cApp::UpdatePlayers()
{
  long i;
  float XMove, ZMove, Speed;
  sStateChangeMessage scm;
  long Elapsed;

  // Loop through all players
  for(i=0;i<MAX_PLAYERS;i++) {

    // Only update connected players
    if(m_Players[i].Connected == TRUE) {

      // Get elapsed time from now and state time
      Elapsed = timeGetTime() - m_Players[i].Time;

      // Process player movement state
      if(m_Players[i].State == STATE_MOVE) {
        // Calculate amount of movement by time passed
        Speed = (float)Elapsed / 1000.0f * m_Players[i].Speed;
        XMove = (float)sin(m_Players[i].Direction) * Speed;
        ZMove = (float)cos(m_Players[i].Direction) * Speed;

        // Check for movement collisions -
        // can't walk past anything blocking path.
        if(CheckIntersect(&m_LevelMesh,                       \
                          m_Players[i].XPos,                  \
                          m_Players[i].YPos + 16.0f,          \
                          m_Players[i].ZPos,                  \
                          m_Players[i].XPos + XMove,          \
                          m_Players[i].YPos + 16.0f,          \
                          m_Players[i].ZPos + ZMove) == TRUE)
          XMove = ZMove = 0.0f;

        // Update player coordinates
        m_Players[i].XPos += XMove;
        m_Players[i].YPos = 0.0f;    // Stay on ground
        m_Players[i].ZPos += ZMove;

        m_Players[i].Time = timeGetTime();  // Reset time
      }

      // Clear swing status after 1 second
      if(m_Players[i].State == STATE_SWING) {
        if(Elapsed > 1000) {
          m_Players[i].State = STATE_IDLE;

          // Send network message to player to clear
          scm.Header.Type     = MSG_STATE_CHANGE;
          scm.Header.Size     = sizeof(sStateChangeMessage);
          scm.Header.PlayerID = m_Players[i].dpnidPlayer;
          scm.XPos            = m_Players[i].XPos;
          scm.YPos            = m_Players[i].YPos;
          scm.ZPos            = m_Players[i].ZPos;
          scm.Direction       = m_Players[i].Direction;
          scm.Speed           = m_Players[i].Speed;
          scm.State           = m_Players[i].State;

          // Send the message over network
          SendNetworkMessage(&scm, DPNSEND_NOLOOPBACK, -1);
        }
      }

      // Clear hurt status after 1 second
      if(m_Players[i].State == STATE_HURT) {
        if(Elapsed > 1000) {
          m_Players[i].State = STATE_IDLE;

          // Send network message to player to clear
          scm.Header.Type     = MSG_STATE_CHANGE;
          scm.Header.Size     = sizeof(sStateChangeMessage);
          scm.Header.PlayerID = m_Players[i].dpnidPlayer;
          scm.XPos            = m_Players[i].XPos;
          scm.YPos            = m_Players[i].YPos;
          scm.ZPos            = m_Players[i].ZPos;
          scm.Direction       = m_Players[i].Direction;
          scm.Speed           = m_Players[i].Speed;
          scm.State           = m_Players[i].State;

          // Send the message over network
          SendNetworkMessage(&scm, DPNSEND_NOLOOPBACK, -1);
        }
      }
    }
  }
}

void cApp::UpdateNetwork()
{
  long i;
  sStateChangeMessage scm;

  // Send all player updates
  for(i=0;i<MAX_PLAYERS;i++) {

    // Only send data about connected players
    if(m_Players[i].Connected == TRUE) {
      scm.Header.Type     = MSG_STATE_CHANGE;
      scm.Header.Size     = sizeof(sStateChangeMessage);
      scm.Header.PlayerID = m_Players[i].dpnidPlayer;
      scm.XPos            = m_Players[i].XPos;
      scm.YPos            = m_Players[i].YPos;
      scm.ZPos            = m_Players[i].ZPos;
      scm.Direction       = m_Players[i].Direction;
      scm.Speed           = m_Players[i].Speed;
      scm.State           = m_Players[i].State;
      scm.Latency         = m_Players[i].Latency;

      // Send the message over network
      SendNetworkMessage(&scm, DPNSEND_NOLOOPBACK);
    }
  }
}

void cApp::UpdateLatency()
{
  long i;
  DPN_CONNECTION_INFO dpci;
  HRESULT hr;

  // Go through all players
  for(i=0;i<MAX_PLAYERS;i++) {
    
    // Only process connected players
    if(m_Players[i].Connected == TRUE) {

      // Request player connection settings
      hr = m_Server.GetServerCOM()->GetConnectionInfo(        \
                   m_Players[i].dpnidPlayer, &dpci, 0);

      if(SUCCEEDED(hr)) {
        m_Players[i].Latency = dpci.dwRoundTripLatencyMS / 2;

        // Bounds latency to 1 second
        if(m_Players[i].Latency > 1000)
          m_Players[i].Latency = 1000;

      } else {
        m_Players[i].Latency = 0;
      }
    }
  }
}

///////////////////////////////////////////////////////////
// Game logic functions
///////////////////////////////////////////////////////////
BOOL cApp::CheckIntersect(cMesh *Mesh,                        \
                 float XStart, float YStart, float ZStart,    \
                 float XEnd,   float YEnd,   float ZEnd)
{
  sMesh *MeshPtr;
  BOOL  Hit;
  float u, v, Dist;
  float XDiff, YDiff, ZDiff, Size;
  DWORD FaceIndex;
  D3DXVECTOR3 vecDir;

  // Error checking
  if(Mesh == NULL)
    return FALSE;

  // Start with parent mesh
  if((MeshPtr = Mesh->GetParentMesh()) == NULL)
    return FALSE;

  // Calculate ray
  XDiff = XEnd - XStart;
  YDiff = YEnd - YStart;
  ZDiff = ZEnd - ZStart;
  D3DXVec3Normalize(&vecDir, &D3DXVECTOR3(XDiff, YDiff, ZDiff));

  // Go through each mesh looking for intersection
  while(MeshPtr != NULL) {
    D3DXIntersect(MeshPtr->m_Mesh,                            \
                &D3DXVECTOR3(XStart,YStart,ZStart), &vecDir,  \
                &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);

    if(Hit == TRUE) {
      Size = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
      if(Dist <= Size)
        return TRUE;
    }

    MeshPtr = MeshPtr->m_Next;
  }

  return FALSE;
}

///////////////////////////////////////////////////////////
// Server class code
///////////////////////////////////////////////////////////
BOOL cServer::CreatePlayer(DPNMSG_CREATE_PLAYER *Msg)
{
  // Send message to application class instance (if any)
  if(g_Application != NULL)
    g_Application->CreatePlayer(Msg);
 
  return TRUE;
}

BOOL cServer::DestroyPlayer(DPNMSG_DESTROY_PLAYER *Msg)
{
  // Send message to application class instance (if any)
  if(g_Application != NULL)
    g_Application->DestroyPlayer(Msg);
  return TRUE;
}

BOOL cServer::Receive(DPNMSG_RECEIVE *Msg)
{
  // Send message to application class instance (if any)
  if(g_Application != NULL)
    g_Application->Receive(Msg);
  return TRUE;
}
