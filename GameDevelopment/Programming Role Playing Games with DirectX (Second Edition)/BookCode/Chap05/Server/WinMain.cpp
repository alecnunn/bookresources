/**************************************************
WinMain.cpp
Chapter 5 Server Network Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  DPLAY.LIB and DXGUID.LIB
**************************************************/

#include <windows.h>
#include <stdio.h>

#include <dplay8.h>
#include <dpaddr.h>

#include "resource.h"

// Application variables ////////////////////////
typedef struct sPlayerInfo {
  DPNID dpnidPlayerId;  // DirectPlay Player ID #
  char  szName[256];    // Player Name

  sPlayerInfo() { dpnidPlayerId = 0; }
} sPlayerInfo;

// Application variables ////////////////////////
HWND g_hWnd;                      // Window handle
char g_szClass[] = "ServerDemo";  // Class name

// Aplication GUID
GUID g_ApplicationGUID = { 0xababbe60, 0x1ac0, 0x11d5,        \
             { 0x90, 0x89, 0x44, 0x45, 0x53, 0x54, 0x0, 0x1 }
                         };

IDirectPlay8Server *g_pDPServer = NULL;  // DirectPlay Server
DPN_SERVICE_PROVIDER_INFO *g_pAdapterList = NULL; // Adapters
DWORD g_dwNumAdapters = 0;                       // # Adapters

sPlayerInfo Players[256];  // Maximum # of players

BOOL g_Hosting = FALSE;  // Flag is host started or not

// Application prototypes ///////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg,              \
                           WPARAM wParam, LPARAM lParam);

BOOL InitDP();
void ReleaseDP();
HRESULT WINAPI NetMsgHandler(PVOID pvUserContext,             \
                             DWORD dwMessageId,               \
                             PVOID pMsgBuffer);
BOOL StartSession(GUID *guidAdapter);
void StopSession();
void EnumAdapters();
void SendTextMsg(DPNID dpnidPlayer, char *Text);

// Application //////////////////////////////////
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow)
{
  WNDCLASS wc;
  MSG      Msg;

  // Register window class
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = WindowProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = DLGWINDOWEXTRA;
  wc.hInstance     = hInst;
  wc.hIcon         = LoadIcon(hInst, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = g_szClass;
  RegisterClass(&wc);

  CoInitialize(NULL);

  // Create the dialog box window and show it
  g_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ENUM),     \
                        0, NULL);

  // Initialize DirectPlay and enumerate TCP/IP adapters
  if(InitDP() == TRUE) {
    EnumAdapters();
  } else {
    MessageBox(g_hWnd, "Error initializing DirectPlay.",      \
                        "ERROR", MB_OK | MB_ICONEXCLAMATION);
    UnregisterClass(g_szClass, hInst);
    CoUninitialize();
    return FALSE;
  }

  // Make sure there's an adapter to use
  if(!g_dwNumAdapters) {
    MessageBox(g_hWnd, "There's no TCP/IP adapters to use!",  \
                       "ERROR", MB_OK | MB_ICONEXCLAMATION);
    UnregisterClass(g_szClass, hInst);
    CoUninitialize();
    return FALSE;
  }

  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, nCmdShow);

  // Message loop
  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  // Clean up
  UnregisterClass(g_szClass, hInst);
  CoUninitialize();

  return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg,             \
                            WPARAM wParam, LPARAM lParam)
{
  unsigned int nSelected, i;
  char         szName[256];
  DPN_SERVICE_PROVIDER_INFO *pAdapterPtr;

  switch(uMsg) {
    case WM_COMMAND:
  
      switch(LOWORD(wParam)) {
  
        case IDC_STARTSTOP:
          if(g_Hosting == FALSE) {
            // Get adapter to use
            if((nSelected = SendMessage(GetDlgItem(hWnd,      \
                IDC_ADAPTERS), CB_GETCURSEL, 0, 0)) == LB_ERR)
              break;

            // Make sure it's valid and start session
            if(nSelected < g_dwNumAdapters) {
              pAdapterPtr = g_pAdapterList;
              pAdapterPtr += nSelected;
              if(StartSession(&pAdapterPtr->guid) == FALSE)
                MessageBox(hWnd, "Unable to start server!",   \
                         "Error", MB_OK | MB_ICONEXCLAMATION);
            }
          } else
            StopSession();
          break;

        case IDC_DISCONNECT:
            // Make sure we're hosting
          if(g_Hosting == TRUE) {

            // Get user from list to disconnect
            nSelected = SendMessage(GetDlgItem(hWnd,          \
                             IDC_USERS), LB_GETCURSEL, 0, 0);
            if(nSelected != LB_ERR) {
              // Find the player in list
              SendMessage(GetDlgItem(hWnd, IDC_USERS),        \
                      LB_GETTEXT, nSelected, (LPARAM)szName);

              for(i=0;i<256;i++) {
                if(Players[i].dpnidPlayerId != 0 &&           \
                        !strcmp(Players[i].szName, szName)) {
                  // Disconnect them
                  g_pDPServer->DestroyClient(                 \
                       Players[i].dpnidPlayerId, NULL, 0, 0);
                  break;
                }
              }
            }
          }

          break;
      }
      break;

    case WM_DESTROY:
      StopSession();
      ReleaseDP();
      PostQuitMessage(0);
      break;

    default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}

BOOL InitDP()
{
  // Create DirectPlay Server component
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Server, NULL,    \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Server,           \
                             (void**)&g_pDPServer)))
    return FALSE;

  // Assign a message handler to network component
  if(FAILED(g_pDPServer->Initialize(NULL, NetMsgHandler, 0)))
    return FALSE;

  return TRUE;
}

void ReleaseDP()
{
  // Delete Server adapter list memory
  delete[] g_pAdapterList;
  g_pAdapterList = NULL;
  g_dwNumAdapters = 0;

  // Release Server component
  if(g_pDPServer != NULL) {
    g_pDPServer->Close(0);
    g_pDPServer->Release();
  }
  g_pDPServer = NULL;
}

BOOL StartSession(GUID *guidAdapter)
{
  DPN_APPLICATION_DESC dpad;
  IDirectPlay8Address *pDPAddress;
  DWORD dwPort;

  // Make sure there's an adapter
  if(guidAdapter == NULL)
    return FALSE;

  // Need to re-assign a network handler as quitting a previous
  // session clears it. Close the connection first before
  // assigning a new network handler.
  g_pDPServer->Close(0);
  if(FAILED(g_pDPServer->Initialize(NULL, NetMsgHandler, 0)))
    return FALSE;

  // Create an address object and fill it with information
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Address, NULL,   \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Address,          \
                             (void**)&pDPAddress)))
    return FALSE;

  // Set the protocol to TCP/IP
  if(FAILED(pDPAddress->SetSP(&CLSID_DP8SP_TCPIP))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Set the port
  dwPort = 21234;
  if(FAILED(pDPAddress->AddComponent(DPNA_KEY_PORT, &dwPort,  \
                      sizeof(DWORD), DPNA_DATATYPE_DWORD))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Set the adapter
  if(FAILED(pDPAddress->AddComponent(DPNA_KEY_DEVICE,         \
           guidAdapter, sizeof(GUID), DPNA_DATATYPE_GUID))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Setup the application description structure
  ZeroMemory(&dpad, sizeof(DPN_APPLICATION_DESC));
  dpad.dwSize          = sizeof(DPN_APPLICATION_DESC);
  dpad.dwFlags         = DPNSESSION_CLIENT_SERVER;
  dpad.guidApplication = g_ApplicationGUID;
  dpad.pwszSessionName = L"ServerSession";
  dpad.dwMaxPlayers    = 256;

  // Start hosting
  if(FAILED(g_pDPServer->Host(&dpad, &pDPAddress, 1, NULL,    \
                              NULL, NULL, 0))) {
    pDPAddress->Release();
    return FALSE;
  }

  // Release the address component
  pDPAddress->Release();

  // Setup the dialog information
  EnableWindow(GetDlgItem(g_hWnd, IDC_ADAPTERS), FALSE);
  SetWindowText(GetDlgItem(g_hWnd, IDC_STARTSTOP), "Stop");

  g_Hosting = TRUE;

  return TRUE;
}

void StopSession()
{
  // Close the connection
  if(g_pDPServer != NULL)
    g_pDPServer->Close(0);

  // Setup the dialog information
  EnableWindow(GetDlgItem(g_hWnd, IDC_ADAPTERS), TRUE);
  SetWindowText(GetDlgItem(g_hWnd, IDC_STARTSTOP), "Start");

  g_Hosting = FALSE;
}

HRESULT WINAPI NetMsgHandler(PVOID pvUserContext,             \
                             DWORD dwMessageId,               \
                             PVOID pMsgBuffer)
{
  DPNMSG_CREATE_PLAYER  *CreatePlayer;
  DPNMSG_DESTROY_PLAYER *DestroyPlayer;
  DPNMSG_RECEIVE        *ReceiveData;
  DPN_PLAYER_INFO       *PlayerInfo;
  DPN_BUFFER_DESC        dpbd;
  DPNHANDLE              hAsync;
  sPlayerInfo           *pPlayer;
  int                    i, nIndex;
  DWORD                  dwSize;
  char                   szMessage[512];
  HRESULT                hr;

  switch(dwMessageId) {
    case DPN_MSGID_CREATE_PLAYER:
      CreatePlayer = (DPNMSG_CREATE_PLAYER*)pMsgBuffer;

      // Get player name and save it
      dwSize = 0;
      PlayerInfo = NULL;
      hr = g_pDPServer->GetClientInfo(                        \
                      CreatePlayer->dpnidPlayer, PlayerInfo,  \
                      &dwSize, 0);
      if(FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL) {
        // Skip this if it's a host player
        if(hr == DPNERR_INVALIDPLAYER)
          break;
        return E_FAIL;
      }
      if((PlayerInfo = (DPN_PLAYER_INFO*)new                  \
                                      BYTE[dwSize]) == NULL)
        return E_FAIL;
      ZeroMemory(PlayerInfo, dwSize);
      PlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
      if(FAILED(g_pDPServer->GetClientInfo(                   \
                       CreatePlayer->dpnidPlayer,             \
                       PlayerInfo, &dwSize, 0))) {
        delete[] PlayerInfo;
        return E_FAIL;
      }

      // Find an empty player structure to use
      nIndex = -1;
      for(i=0;i<256;i++) {
        if(!Players[i].dpnidPlayerId) {
          nIndex = i;
          break;
        }
      }
      if(nIndex == -1)
        return E_FAIL;

      // Set player context
      CreatePlayer->pvPlayerContext=(void*)&Players[nIndex];

      // Save player ID
      Players[nIndex].dpnidPlayerId=CreatePlayer->dpnidPlayer;

      // Add player to list
      wcstombs(Players[nIndex].szName,PlayerInfo->pwszName,256);
      SendMessage(GetDlgItem(g_hWnd,IDC_USERS),LB_ADDSTRING,  \
                          0, (LPARAM)Players[nIndex].szName);

      // Send a message to all players notifying someone joined
      sprintf(szMessage, "%s joined!", Players[nIndex].szName);
      SendTextMsg(DPNID_ALL_PLAYERS_GROUP, szMessage);

      delete[] PlayerInfo;

      break;

    case DPN_MSGID_DESTROY_PLAYER:
      DestroyPlayer = (DPNMSG_DESTROY_PLAYER*)pMsgBuffer;

      // Make sure it's not the host
      if((pPlayer =                                           \
          (sPlayerInfo*)DestroyPlayer->pvPlayerContext)==NULL)
        break;

      // Remove the player from list
      pPlayer->dpnidPlayerId = 0;
      nIndex = SendMessage(GetDlgItem(g_hWnd, IDC_USERS),     \
                           LB_FINDSTRING, -1,                 \
                           (LPARAM)pPlayer->szName);
      if(nIndex != LB_ERR)
        SendMessage(GetDlgItem(g_hWnd, IDC_USERS),            \
                               LB_DELETESTRING, nIndex, 0);

      // Send message to remaining players notifying player left
      sprintf(szMessage, "%s quit.", pPlayer->szName);
      SendTextMsg(DPNID_ALL_PLAYERS_GROUP, szMessage);

      break;

    case DPN_MSGID_RECEIVE:
      ReceiveData = (DPNMSG_RECEIVE*)pMsgBuffer;

      // Forward message to all player except host
      dpbd.dwBufferSize = ReceiveData->dwReceiveDataSize;
      dpbd.pBufferData  = (BYTE*)ReceiveData->pReceiveData;
      g_pDPServer->SendTo(DPNID_ALL_PLAYERS_GROUP, &dpbd, 1,  \
                        0, NULL, &hAsync, DPNSEND_NOLOOPBACK);
      break;
  }

  return S_OK;
}

void EnumAdapters()
{
  HWND hCB;
  DPN_SERVICE_PROVIDER_INFO *pAdapterPtr;
  DWORD dwSize = 0;
  DWORD i;
  char szName[1024];
  HRESULT hr;

  // Get a handle to the list box
  hCB = GetDlgItem(g_hWnd, IDC_ADAPTERS);

  // Clear the list box
  SendMessage(hCB, CB_RESETCONTENT, 0, 0);

  // Free prior adapter list
  delete[] g_pAdapterList;
  g_pAdapterList = NULL;
  g_dwNumAdapters = 0;

  // Return if no Server object
  if(g_pDPServer == NULL)
    return;

  // Query the required size of the data buffer
  hr = g_pDPServer->EnumServiceProviders(&CLSID_DP8SP_TCPIP,  \
          NULL, g_pAdapterList, &dwSize, &g_dwNumAdapters, 0);
  if(hr != DPNERR_BUFFERTOOSMALL)
    return;

  // Allocate a buffer
  if((g_pAdapterList = (DPN_SERVICE_PROVIDER_INFO*)new        \
                                        BYTE[dwSize]) == NULL)
    return;

  // Enumerate again
  if(SUCCEEDED(g_pDPServer->EnumServiceProviders(             \
               &CLSID_DP8SP_TCPIP, NULL, g_pAdapterList,      \
               &dwSize, &g_dwNumAdapters, 0))) {

    // Enumeration is complete, scan through entries
    pAdapterPtr = g_pAdapterList;
    for(i=0;i<g_dwNumAdapters;i++) {

      // Convert wide string into multi-byte string
      wcstombs(szName, pAdapterPtr->pwszName, 1024);

      // Add the service provider into box
      SendMessage(hCB, CB_ADDSTRING, 0, (LPARAM)szName);

      // Go to next service provider in buffer
      pAdapterPtr++;
    }
  }

  // Select first adapter
  SendMessage(hCB, CB_SETCURSEL, 0, 0);
}

void SendTextMsg(DPNID dpnidPlayer, char *Text)
{
  DPNHANDLE       hAsync;
  DPN_BUFFER_DESC dpbd;

  if(g_pDPServer == NULL)
    return;

  // Build a data structure
  dpbd.dwBufferSize = strlen(Text)+1;
  dpbd.pBufferData = (BYTE*)Text;

  // Send message (async method - reason for handle)
  g_pDPServer->SendTo(dpnidPlayer,&dpbd,1,0,NULL,&hAsync,     \
                      DPNSEND_NOLOOPBACK);
}
