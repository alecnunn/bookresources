/**************************************************
WinMain.cpp
Chapter 5 Client Network Demo

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
HWND g_hWnd;                      // Window handle
char g_szClass[] = "ClientDemo";  // Class name

// Aplication GUID
GUID g_ApplicationGUID = { 0xababbe60, 0x1ac0, 0x11d5,        \
           { 0x90, 0x89, 0x44, 0x45, 0x53, 0x54, 0x0, 0x1 } };

IDirectPlay8Client *g_pDPClient = NULL;  // DirectPlay Client
DPN_SERVICE_PROVIDER_INFO *g_pAdapterList = NULL; // Adapters
DWORD g_dwNumAdapters = 0;                       // # Adapters

BOOL      g_Connected = FALSE;  // Flag is connected or not
DPNHANDLE g_hAsync = NULL;      // async connection handle

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
BOOL StartSession(GUID *guidAdapter, char *szIP);
void StopSession();
void EnumAdapters();
void SendTextMsg(char *Text);

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
    MessageBox(g_hWnd, "Error initializing DirectPlay.",       \
                        "ERROR", MB_OK | MB_ICONEXCLAMATION);
    UnregisterClass(g_szClass, hInst);
    CoUninitialize();
    return FALSE;
  }

  // Make sure there's an adapter to use
  if(!g_dwNumAdapters) {
    MessageBox(g_hWnd,"There are no TCP/IP adapters to use!", \
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
  unsigned int nSelected;
  DPN_SERVICE_PROVIDER_INFO *pAdapterPtr;
  char szMsg[256];
  char szIP[16];

  switch(uMsg) {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_STARTSTOP:
          if(g_Connected == FALSE) {

            // Check if attempting connect and cancel
            if(g_hAsync != NULL) {
              if(g_pDPClient != NULL) {
                EnableWindow(GetDlgItem(hWnd, IDC_STARTSTOP), \
                             FALSE);
                g_pDPClient->CancelAsyncOperation(g_hAsync,0);
              }
              g_hAsync = NULL;
              break;
            }

            // Get adapter to use
            nSelected = SendMessage(GetDlgItem(hWnd,          \
                           IDC_ADAPTERS), CB_GETCURSEL, 0, 0);

            // Get IP address to use
            GetWindowText(GetDlgItem(g_hWnd,IDC_IP),szIP,16);

            // Make sure IP address is valid and start session
            if(nSelected < g_dwNumAdapters) {
              pAdapterPtr = g_pAdapterList;
              pAdapterPtr += nSelected;
              if(StartSession(&pAdapterPtr->guid,szIP)==FALSE)
                MessageBox(hWnd, "Unable to start Client!",   \
                         "Error", MB_OK | MB_ICONEXCLAMATION);
            }
          } else
            StopSession();
          break;

        case IDC_SEND:
          GetWindowText(GetDlgItem(hWnd, IDC_MESSAGE),        \
                        szMsg, 256);
          SetWindowText(GetDlgItem(hWnd, IDC_MESSAGE), "");
          SendTextMsg(szMsg);

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
  // Create DirectPlay Client component
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Client, NULL,    \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Client,           \
                             (void**)&g_pDPClient)))
    return FALSE;

  // Assign a message handler to network component
  if(FAILED(g_pDPClient->Initialize(NULL, NetMsgHandler, 0)))
    return FALSE;

  return TRUE;
}

void ReleaseDP()
{
  // Delete Client adapter list memory
  delete[] g_pAdapterList;
  g_pAdapterList = NULL;
  g_dwNumAdapters = 0;

  // Release Client component
  if(g_pDPClient != NULL) {
    g_pDPClient->Close(0);
    g_pDPClient->Release();
  }
  g_pDPClient = NULL;
}

BOOL StartSession(GUID *guidAdapter, char *szIP)
{
  DPN_APPLICATION_DESC dpad;
  DPN_PLAYER_INFO      dppi;
  IDirectPlay8Address *pDPHostAddress, *pDPDeviceAddress;
  DWORD dwPort;
  char  szPlayerName[256];
  WCHAR wszIP[128], wszPlayerName[256];

  // Make sure there's an adapter and IP
  if(guidAdapter == NULL || szIP == NULL)
    return FALSE;

  // Need to re-assign a network handler as quitting a previous
  // session clears it. Close the connection first before
  // assigning a new network handler.
  g_pDPClient->Close(0);
  if(FAILED(g_pDPClient->Initialize(NULL, NetMsgHandler, 0)))
    return FALSE;

  // Assign player information
  ZeroMemory(&dppi, sizeof(DPN_PLAYER_INFO));
  dppi.dwSize = sizeof(DPN_PLAYER_INFO);
  dppi.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
  GetWindowText(GetDlgItem(g_hWnd,IDC_NAME),szPlayerName,256);
  mbstowcs(wszPlayerName,szPlayerName,strlen(szPlayerName)+1);
  dppi.pwszName = wszPlayerName;
  g_pDPClient->SetClientInfo(&dppi, NULL, NULL,               \
                             DPNSETCLIENTINFO_SYNC);

  // Create address objects
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Address, NULL,   \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Address,          \
                             (void**)&pDPHostAddress)))
    return FALSE;
  if(FAILED(CoCreateInstance(CLSID_DirectPlay8Address, NULL,   \
                             CLSCTX_INPROC,                    \
                             IID_IDirectPlay8Address,          \
                             (void**)&pDPDeviceAddress))) {
    pDPHostAddress->Release();
    return FALSE;
  }

  // Set the protocol to TCP/IP
  pDPHostAddress->SetSP(&CLSID_DP8SP_TCPIP);
  pDPDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP);

  // Set the port
  dwPort = 21234;
  pDPHostAddress->AddComponent(DPNA_KEY_PORT, &dwPort,        \
                          sizeof(DWORD), DPNA_DATATYPE_DWORD);

  // Set the host
  mbstowcs(wszIP, szIP, strlen(szIP)+1);
  pDPHostAddress->AddComponent(DPNA_KEY_HOSTNAME, wszIP,      \
                         (wcslen(wszIP)+1)*sizeof(WCHAR),     \
                         DPNA_DATATYPE_STRING);

  // Set the adapter
  pDPHostAddress->AddComponent(DPNA_KEY_DEVICE, guidAdapter,  \
                         sizeof(GUID), DPNA_DATATYPE_GUID);
  pDPDeviceAddress->AddComponent(DPNA_KEY_DEVICE,             \
                         guidAdapter, sizeof(GUID),           \
                         DPNA_DATATYPE_GUID);

  // Setup the application description structure
  ZeroMemory(&dpad, sizeof(DPN_APPLICATION_DESC));
  dpad.dwSize          = sizeof(DPN_APPLICATION_DESC);
  dpad.dwFlags         = DPNSESSION_CLIENT_SERVER;
  dpad.guidApplication = g_ApplicationGUID;
  dpad.pwszSessionName = L"ServerSession";

  // Connect
  if(FAILED(g_pDPClient->Connect(&dpad, pDPHostAddress,       \
                                 pDPDeviceAddress, NULL,      \
                                 NULL, NULL, 0, NULL,         \
                                 &g_hAsync, 0))) {
    pDPHostAddress->Release();
    pDPDeviceAddress->Release();
    return FALSE;
  }

  // Setup the dialog information
  EnableWindow(GetDlgItem(g_hWnd, IDC_ADAPTERS), FALSE);
  EnableWindow(GetDlgItem(g_hWnd, IDC_IP), FALSE);
  EnableWindow(GetDlgItem(g_hWnd, IDC_NAME), FALSE);
  EnableWindow(GetDlgItem(g_hWnd, IDC_STARTSTOP), TRUE);
  SetWindowText(GetDlgItem(g_hWnd, IDC_STARTSTOP),            \
                           "Cancel Connect");

  // Release the address components
  pDPHostAddress->Release();
  pDPDeviceAddress->Release();

  return TRUE;
}

void StopSession()
{
  // Close the connection
  if(g_pDPClient != NULL)
    g_pDPClient->Close(0);

  // Clear async handle
  g_hAsync = NULL;

  // Setup the dialog information
  EnableWindow(GetDlgItem(g_hWnd, IDC_ADAPTERS), TRUE);
  EnableWindow(GetDlgItem(g_hWnd, IDC_IP), TRUE);
  EnableWindow(GetDlgItem(g_hWnd, IDC_NAME), TRUE);
  SetWindowText(GetDlgItem(g_hWnd, IDC_STARTSTOP), "Connect");

  // Flag as disconnected
  g_Connected = FALSE;
}

HRESULT WINAPI NetMsgHandler(PVOID pvUserContext,             \
                         DWORD dwMessageId, PVOID pMsgBuffer)
{
  DPNMSG_CONNECT_COMPLETE  *ConnectComplete;
  DPNMSG_TERMINATE_SESSION *TerminateSession;
  DPNMSG_RECEIVE           *ReceiveData;

  switch(dwMessageId) {
    case DPN_MSGID_CONNECT_COMPLETE:
      ConnectComplete = (DPNMSG_CONNECT_COMPLETE*)pMsgBuffer;

      // Make sure connection complete
      if(ConnectComplete->hResultCode == S_OK) {
        // Setup the dialog information
        EnableWindow(GetDlgItem(g_hWnd, IDC_ADAPTERS), FALSE);
        EnableWindow(GetDlgItem(g_hWnd, IDC_IP), FALSE);
        EnableWindow(GetDlgItem(g_hWnd, IDC_NAME), FALSE);
        EnableWindow(GetDlgItem(g_hWnd, IDC_STARTSTOP), TRUE);
        SetWindowText(GetDlgItem(g_hWnd, IDC_STARTSTOP),      \
                                 "Disconnect");
        g_hAsync = NULL;

        // Flag as connected
        g_Connected = TRUE;
      } else {
        // Setup the dialog information
        EnableWindow(GetDlgItem(g_hWnd, IDC_ADAPTERS), TRUE);
        EnableWindow(GetDlgItem(g_hWnd, IDC_IP), TRUE);
        EnableWindow(GetDlgItem(g_hWnd, IDC_NAME), TRUE);
        EnableWindow(GetDlgItem(g_hWnd, IDC_STARTSTOP), TRUE);
        SetWindowText(GetDlgItem(g_hWnd, IDC_STARTSTOP),      \
                                "Connect");

        // Clear async handle
        g_hAsync = NULL;

        // Flag as disconnected
        g_Connected = FALSE;
      }

      break;

    case DPN_MSGID_TERMINATE_SESSION:
      TerminateSession=(DPNMSG_TERMINATE_SESSION*)pMsgBuffer;

      // Setup the dialog information
      EnableWindow(GetDlgItem(g_hWnd, IDC_ADAPTERS), TRUE);
      EnableWindow(GetDlgItem(g_hWnd, IDC_IP), TRUE);
      EnableWindow(GetDlgItem(g_hWnd, IDC_NAME), TRUE);
      SetWindowText(GetDlgItem(g_hWnd, IDC_STARTSTOP),        \
                    "Connect");

      // Clear async handle
      g_hAsync = NULL;

      // Flag as disconnected
      g_Connected = FALSE;

      // Print a disconnect message
      SendMessage(GetDlgItem(g_hWnd, IDC_CHATTER),            \
                  LB_ADDSTRING, 0, (LPARAM)"Disconnected.");

      break;

    case DPN_MSGID_RECEIVE:
      ReceiveData = (DPNMSG_RECEIVE*)pMsgBuffer;

      // Write out text
      SendMessage(GetDlgItem(g_hWnd, IDC_CHATTER),            \
                  LB_ADDSTRING, 0,                            \
                  (LPARAM)ReceiveData->pReceiveData);

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

  // Return if no Client object
  if(g_pDPClient == NULL)
    return;

  // Query the required size of the data buffer
  hr = g_pDPClient->EnumServiceProviders(                     \
                   &CLSID_DP8SP_TCPIP, NULL, g_pAdapterList,  \
                   &dwSize, &g_dwNumAdapters, 0);
  if(hr != DPNERR_BUFFERTOOSMALL)
    return;

  // Allocate a buffer
  if((g_pAdapterList = (DPN_SERVICE_PROVIDER_INFO*)new        \
                                       BYTE[dwSize]) == NULL)
    return;

  // Enumerate again
  if(SUCCEEDED(g_pDPClient->EnumServiceProviders(             \
                   &CLSID_DP8SP_TCPIP, NULL, g_pAdapterList,  \
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

void SendTextMsg(char *Text)
{
  DPNHANDLE       hAsync;
  DPN_BUFFER_DESC dpbd;
  char            szName[64];
  char            szMsg[1024];

  // Make sure there's a network object
  if(g_pDPClient == NULL)
    return;

  // Get my name
  GetWindowText(GetDlgItem(g_hWnd, IDC_NAME), szName, 64);

  // Rebuild string based on my name
  sprintf(szMsg, "%s> %s", szName, Text);

  // Build a data structure
  dpbd.dwBufferSize = strlen(szMsg)+1;
  dpbd.pBufferData = (BYTE*)szMsg;

  // Send message (async method - reason for handle)
  g_pDPClient->Send(&dpbd, 1, 0, NULL, &hAsync,               \
                    DPNSEND_NOLOOPBACK);
}
