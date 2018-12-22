// File: ZFXAudio.cpp

#include "ZFXAudio.h"

/**
 * Constructor: Nothing spectacular.
 */
ZFXAudio::ZFXAudio(HINSTANCE hInst) {
   m_hInst   = hInst;
   m_pDevice = NULL;
   m_hDLL    = NULL;
   }
/*----------------------------------------------------------------*/

/**
 * Destructor: Just call the Release method
 */
ZFXAudio::~ZFXAudio(void) { 
   Release(); 
   }
/*----------------------------------------------------------------*/

/**
 * Create the dll objects. This functions loads the appropriate dll.
 */
HRESULT ZFXAudio::CreateDevice(void) {
   // load the DLL containing interface implementation
   m_hDLL = LoadLibraryEx("ZFXDA.dll",NULL,0);
   if(!m_hDLL) {
      MessageBox(NULL,
         "Loading ZFXDA.dll from lib failed.",
         "ZFXEngine - error", MB_OK | MB_ICONERROR);
      return E_FAIL;
      }
   
   CREATEAUDIODEVICE _CreateAudioDevice = 0;
   HRESULT hr;
   
   // function pointer to dll's 'CreateAudioDevice' function
   _CreateAudioDevice = (CREATEAUDIODEVICE)
                          GetProcAddress(m_hDLL,
                                 "CreateAudioDevice");
   // call dll's create function
   hr = _CreateAudioDevice(m_hDLL, &m_pDevice);
   if(FAILED(hr)){
      MessageBox(NULL,
         "CreateAudioDevice() from lib failed.",
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
void ZFXAudio::Release(void) {
   RELEASEAUDIODEVICE _ReleaseAudioDevice = 0;
   HRESULT hr;
   
   if (m_hDLL) {
      // function pointer to dll 'ReleaseAudioDevice' function
      _ReleaseAudioDevice = (RELEASEAUDIODEVICE)
         GetProcAddress(m_hDLL, "ReleaseAudioDevice");
      }
   // call dll's release function
   if (m_pDevice) {
      hr = _ReleaseAudioDevice(&m_pDevice);
      if(FAILED(hr)){
         m_pDevice = NULL;
         }
      }
   } // Release
/*----------------------------------------------------------------*/