// File: ZFXNetwork.cpp

#include "ZFXNetwork.h"

/**
 * Constructor: Nothing spectacular.
 */
ZFXNetwork::ZFXNetwork(HINSTANCE hInst) {
   m_hInst   = hInst;
   m_pDevice = NULL;
   m_hDLL    = NULL;
   }
/*----------------------------------------------------------------*/

/**
 * Destructor: Just call the Release method
 */
ZFXNetwork::~ZFXNetwork(void) { 
   Release(); 
   }
/*----------------------------------------------------------------*/

/**
 * Create the dll objects. This functions loads the appropriate dll.
 */
HRESULT ZFXNetwork::CreateDevice(void) {
   // load the DLL containing interface implementation
   m_hDLL = LoadLibraryEx("ZFXWS.dll",NULL,0);
   if(!m_hDLL) {
      MessageBox(NULL,
         "Loading ZFXWS.dll from lib failed.",
         "ZFXEngine - error", MB_OK | MB_ICONERROR);
      return E_FAIL;
      }
   
   CREATENETWORKDEVICE _CreateNetworkDevice = 0;
   HRESULT hr;
   
   // function pointer to dll's 'CreateNetworkDevice' function
   _CreateNetworkDevice = (CREATENETWORKDEVICE)
                          GetProcAddress(m_hDLL,
                                 "CreateNetworkDevice");
   // call dll's create function
   hr = _CreateNetworkDevice(m_hDLL, &m_pDevice);
   if(FAILED(hr)){
      MessageBox(NULL,
         "CreateNetworkDevice() from lib failed.",
         "ZFXEngine - error", MB_OK | MB_ICONERROR);
      m_pDevice = NULL;
      return E_FAIL;
      }
   
   return S_OK;
   } // CreateDevice
/*----------------------------------------------------------------*/

/**
 * Cleanup the dll objects.
 */
void ZFXNetwork::Release(void) {
   RELEASENETWORKDEVICE _ReleaseNetworkDevice = 0;
   HRESULT hr;
   
   if (m_hDLL) {
      // function pointer to dll 'ReleaseNetworkDevice' function
      _ReleaseNetworkDevice = (RELEASENETWORKDEVICE)
         GetProcAddress(m_hDLL, "ReleaseNetworkDevice");
      }
   // call dll's release function
   if (m_pDevice) {
      hr = _ReleaseNetworkDevice(&m_pDevice);
      if(FAILED(hr)){
         m_pDevice = NULL;
         }
      }
   } // Release
/*----------------------------------------------------------------*/