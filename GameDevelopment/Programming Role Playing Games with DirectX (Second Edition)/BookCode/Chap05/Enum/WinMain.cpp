/**************************************************
WinMain.cpp
Chapter 5 Enum Network Adapters Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  DPLAY.LIB and DXGUID.LIB
**************************************************/

#include <windows.h>

#include <dplay8.h>
#include <dpaddr.h>

#include "resource.h"

// Application variables ////////////////////////
HWND g_hWnd;                    // Window handle
char g_szClass[] = "EnumDemo";  // Class name

IDirectPlay8Client *g_pDPClient = NULL;    // DirectPlay Client
DPN_SERVICE_PROVIDER_INFO *g_pSPList = NULL; // SP list
DWORD g_dwNumSP = 0;                         // and count

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
void EnumSP();
void EnumAdapters(GUID *guidSP);

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

  // Initialize DirectPlay and enumerate service providers
  if(InitDP() == TRUE)
    EnumSP();
  else {
    MessageBox(NULL, "Error initializing DirectPlay.",        \
               "ERROR", MB_OK | MB_ICONEXCLAMATION);
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
  DPN_SERVICE_PROVIDER_INFO *pSPPtr;

  switch(uMsg) {
    case WM_COMMAND:
      if(LOWORD(wParam) == IDC_SERVICEPROVIDERS &&            \
                           HIWORD(wParam) == LBN_SELCHANGE) {
        // Get the selection from the list
        nSelected = SendMessage(GetDlgItem(hWnd,              \
                  IDC_SERVICEPROVIDERS), LB_GETCURSEL, 0, 0);

        // Make sure it's valid and enumerate the adapters
        if(nSelected < g_dwNumSP) {
          pSPPtr = g_pSPList;
          pSPPtr += nSelected;
          EnumAdapters(&pSPPtr->guid);
        }
      }
      break;

    case WM_DESTROY:
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
  // Delete Client SP list memory
  delete[] g_pSPList;
  g_pSPList = NULL;
  g_dwNumSP = 0;

  // Release client component
  if(g_pDPClient != NULL) {
    g_pDPClient->Close(0);
    g_pDPClient->Release();
  }
  g_pDPClient = NULL;
}

HRESULT WINAPI NetMsgHandler(PVOID pvUserContext,             \
                             DWORD dwMessageId,               \
                             PVOID pMsgBuffer)
{
  // Return S_OK to signify the message was handled OK
  return S_OK;
}

void EnumSP()
{
  HWND hLB;
  DPN_SERVICE_PROVIDER_INFO *pSPPtr;
  DWORD dwSize = 0;
  DWORD i;
  char szName[1024];
  HRESULT hr;

  // Get a handle to the list box
  hLB = GetDlgItem(g_hWnd, IDC_SERVICEPROVIDERS);

  // Clear the list box
  SendMessage(hLB, LB_RESETCONTENT, 0, 0);

  // Release SP list memory
  delete[] g_pSPList;
  g_pSPList = NULL;
  g_dwNumSP = 0;

  // Return if no Server object
  if(g_pDPClient == NULL)
    return;

  // Query the required size of the data buffer
  hr = g_pDPClient->EnumServiceProviders(NULL,NULL,g_pSPList, \
                                     &dwSize, &g_dwNumSP, 0);
  if(hr != DPNERR_BUFFERTOOSMALL)
    return;

  // Allocate a buffer
  if((g_pSPList = (DPN_SERVICE_PROVIDER_INFO*)new             \
                                       BYTE[dwSize]) == NULL)
    return;

  // Enumerate again
  if(SUCCEEDED(g_pDPClient->EnumServiceProviders(NULL,NULL,   \
                       g_pSPList, &dwSize, &g_dwNumSP, 0))) {

    // Enumeration is complete, scan through entries
    pSPPtr = g_pSPList;
    for(i=0;i<g_dwNumSP;i++) {

      // Convert wide string into multi-byte string
      wcstombs(szName, pSPPtr->pwszName, 1024);

      // Add the service provider into box
      SendMessage(hLB, LB_ADDSTRING, 0, (LPARAM)szName);

      // Go to next service provider in buffer
      pSPPtr++;
    }
  }
}

void EnumAdapters(GUID *guidSP)
{
  HWND hLB;
  DPN_SERVICE_PROVIDER_INFO *pAdapterList = NULL;
  DPN_SERVICE_PROVIDER_INFO *pAdapterPtr;
  DWORD dwNumAdapters = 0;
  DWORD dwSize = 0;
  DWORD i;
  char szName[1024];
  HRESULT hr;

  // Get a handle to the list box
  hLB = GetDlgItem(g_hWnd, IDC_ADAPTERS);

  // Clear the list box
  SendMessage(hLB, LB_RESETCONTENT, 0, 0);

  // Return if no Server object or GUID
  if(g_pDPClient == NULL || guidSP == NULL)
    return;

  // Query the required size of the data buffer
  hr = g_pDPClient->EnumServiceProviders(guidSP, NULL,        \
                    pAdapterList, &dwSize, &dwNumAdapters, 0);
  if(hr != DPNERR_BUFFERTOOSMALL)
    return;

  // Allocate a buffer
  if((pAdapterList = (DPN_SERVICE_PROVIDER_INFO*)new          \
                                        BYTE[dwSize]) == NULL)
    return;

  // Enumerate again
  if(SUCCEEDED(g_pDPClient->EnumServiceProviders(guidSP,NULL, \
                pAdapterList, &dwSize, &dwNumAdapters, 0))) {

    // Enumeration is complete, scan through entries
    pAdapterPtr = pAdapterList;
    for(i=0;i<dwNumAdapters;i++) {

      // Convert wide string into multi-byte string
      wcstombs(szName, pAdapterPtr->pwszName, 1024);

      // Add the service provider into box
      SendMessage(hLB, LB_ADDSTRING, 0, (LPARAM)szName);

      // Go to next service provider in buffer
      pAdapterPtr++;
    }
  }

  // Delete the list memory resources
  delete[] pAdapterList;
}
