// File: ZFXRenderer.cpp

#include "ZFXRenderer.h"

/**
 * Constructor: Nothing spectacular.
 */
ZFXRenderer::ZFXRenderer(HINSTANCE hInst) {
   m_hInst   = hInst;
   m_pDevice = NULL;
   m_hDLL    = NULL;
   }
/*----------------------------------------------------------------*/

/**
 * Destructor: Just call the Release method
 */
ZFXRenderer::~ZFXRenderer(void) { 
   Release(); 
   }
/*----------------------------------------------------------------*/

/**
 * Create the dll objects. This functions loads the appropriate dll.
 */
HRESULT ZFXRenderer::CreateDevice(const char *chAPI) {
   char buffer[300];
   
   // decide which API should be used
   if (strcmp(chAPI, "Direct3D") == 0) {
      m_hDLL = LoadLibrary("ZFXD3D.dll");
      if(!m_hDLL) {
         MessageBox(NULL,
            "Loading ZFXD3D.dll from lib failed.",
            "ZFXEngine - error", MB_OK | MB_ICONERROR);
         return E_FAIL;
         }
      }
   else {
      _snprintf(buffer, 300, "API '%s' not yet supported.", chAPI);
      MessageBox(NULL, buffer, "ZFXEngine - error", MB_OK |
                 MB_ICONERROR);
      return E_FAIL;
      }
   
   CREATERENDERDEVICE _CreateRenderDevice = 0;
   HRESULT hr;
   
   // function pointer to dll's 'CreateRenderDevice' function
   _CreateRenderDevice = (CREATERENDERDEVICE)
                          GetProcAddress(m_hDLL,
                                 "CreateRenderDevice");

   if ( !_CreateRenderDevice ) return E_FAIL;

   // call dll's create function
   hr = _CreateRenderDevice(m_hDLL, &m_pDevice);
   if(FAILED(hr)){
      MessageBox(NULL,
         "CreateRenderDevice() from lib failed.",
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
void ZFXRenderer::Release(void) {
   RELEASERENDERDEVICE _ReleaseRenderDevice = 0;
   HRESULT hr;
   
   if (m_hDLL) {
      // function pointer to dll 'ReleaseRenderDevice' function
      _ReleaseRenderDevice = (RELEASERENDERDEVICE)
         GetProcAddress(m_hDLL, "ReleaseRenderDevice");
      }
   // call dll's release function
   if (m_pDevice) {
      hr = _ReleaseRenderDevice(&m_pDevice);
      if(FAILED(hr)){
         m_pDevice = NULL;
         }
      }
   } // Release
/*----------------------------------------------------------------*/
