// File: ZFXDA.cpp

#include <windows.h>       // type definitions
#include "ZFXDA.h"         // class definition

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")


bool g_bLF=false;
size_t g_szoff3 = sizeof(float)*3;


/*-----------------------------------------------------------*/
/* DLL stuff implementation                                  *
/*-----------------------------------------------------------*/

/**
 * DLL Entry Point similar to WinMain()/main()
 */
BOOL WINAPI DllEntryPoint(HINSTANCE hDll,
                          DWORD     fdwReason,
                          LPVOID    lpvReserved) {
   switch(fdwReason) {
      // called when we attach to the DLL
      case DLL_PROCESS_ATTACH:
         /* dll init/setup stuff */
         break;
      case DLL_PROCESS_DETACH:
         /* dll shutdown/release stuff */
         break;
      default:
         break;
      };
   
   return TRUE;
   } // DllEntryPoint
/*----------------------------------------------------------------*/

/**
 * Exported create function: Creates a new ZFXAudioDevice object.
 */
HRESULT CreateAudioDevice(HINSTANCE hDLL, ZFXAudioDevice **pDevice) {
   if(!*pDevice) {
      *pDevice = new ZFXDA(hDLL);
      return ZFX_OK;
      }
   return ZFX_FAIL;
   }
/*----------------------------------------------------------------*/

/**
 * Exported release function: Realeses the given ZFXAudioDevice object.
 */
HRESULT ReleaseAudioDevice(ZFXAudioDevice **pDevice) {
   if(!*pDevice) {
      return ZFX_FAIL;
      }
   delete *pDevice;
   *pDevice = NULL;
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/





/*-----------------------------------------------------------*/
/* ZFXDA class implementation                                *
/*-----------------------------------------------------------*/

/**
 * Constructor
 */
ZFXDA::ZFXDA(HINSTANCE hDLL) {
   m_hDLL         = hDLL;
   m_pLoader      = NULL;
   m_pListener    = NULL;
   m_pPerformance = NULL;
   m_pSounds      = NULL;
   m_pLog         = NULL;
   m_bRunning     = false;
   m_NumSounds    = 0;

   // init structs
   m_dsListener.dwSize = sizeof(DS3DLISTENER);
   m_dsBuffer.dwSize = sizeof(DS3DBUFFER);

   // open a new logfile
   m_pLog = fopen("Log_ZFXAudioDevice.txt", "w");
   Log("online (waiting for initialization call)");
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXDA::~ZFXDA() {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


/**
 * Release all stuff.
 */
void ZFXDA::Release() {
   Log("shutting down DirectAudio");

   if (m_pSounds) {
      for (UINT i=0; i<m_NumSounds; i++) {
         if (m_pSounds[i].pSegment) {
            m_pSounds[i].pSegment->Unload(m_pPerformance);
            m_pSounds[i].pSegment->Release();
            m_pSounds[i].pSegment = NULL;
            delete [] m_pSounds[i].chName;
            m_pSounds[i].chName = NULL;
            }
         }
      free(m_pSounds);
      }

   if (m_pLoader) {
      m_pLoader->Release();
      m_pLoader = NULL;
      }
   if (m_pListener) {
      m_pListener->Release();
      m_pListener = NULL;
      }

   if (m_pPerformance) {
      m_pPerformance->Stop(NULL,NULL,0,0);
      m_pPerformance->CloseDown();
      m_pPerformance->Release();
      m_pPerformance = NULL;
      }

   // uninitialize COM
   if (m_bRunning) CoUninitialize();
   m_bRunning = false;

   Log("offline (ok)");
   fclose(m_pLog);
   } // release
/*----------------------------------------------------------------*/


/**
 * Initialize DirectMusic. We need a Performance and a Loader.
 */
HRESULT ZFXDA::Init(HWND hWnd, const char *chPath, bool bSaveLog) {
   HRESULT hr;
   WCHAR   wPath[MAX_PATH];

   Log("calling initialization");
   
   m_hWndMain = hWnd;
   g_bLF      = bSaveLog;

   // initialize COM
   CoInitialize(NULL);
   
   // create the loader object
   hr = CoCreateInstance(CLSID_DirectMusicLoader,       // class ID
                         NULL,                          // part of aggregate
                         CLSCTX_INPROC,                 // exe context
                         IID_IDirectMusicLoader8,       // ref ID
                         (void**)&m_pLoader);           // create adress
   if (FAILED(hr)) {
      if (hr==REGDB_E_CLASSNOTREG)
         Log("error: CoCreateInst(DMLoader) unknown class");
      else Log("error: CoCreateInst(DMLoader) failed");
      return ZFX_FAIL;
      }

   // create the performance object
   hr = CoCreateInstance(CLSID_DirectMusicPerformance,  // class ID
                         NULL,                          // part of aggregate
                         CLSCTX_INPROC,                 // exe context
                         IID_IDirectMusicPerformance8,  // ref ID
                         (void**)&m_pPerformance);      // create adress
   if (FAILED(hr)) {
      if (hr==REGDB_E_CLASSNOTREG)
         Log("error: CoCreateInst(DMPerformance) unknown class");
      else Log("error: CoCreateInst(DMPerformance) failed");
      return ZFX_FAIL;
      }

   // set the default file path for sounds
   if (MultiByteToWideChar(CP_ACP, 0, chPath, -1, wPath, MAX_PATH)==0)
      return ZFX_FAIL;

   if (FAILED(hr=m_pLoader->SetSearchDirectory(
                                    GUID_DirectMusicAllTypes, 
                                    wPath, false))) {
      if ( hr == DMUS_E_LOADER_BADPATH )
         Log("error: SetSearchDirectory() DMUS_E_LOADER_BADPATH");
      else if ( hr == E_OUTOFMEMORY )
         Log("error: SetSearchDirectory() E_OUTOFMEMORY");
      else 
         Log("error: SetSearchDirectory() E_POINTER");
      return ZFX_FAIL;
      }
   
   // initialize performance object
   if (FAILED(hr=m_pPerformance->InitAudio(
                      NULL, NULL, hWnd, 
                      DMUS_APATH_SHARED_STEREOPLUSREVERB, 
                      64, DMUS_AUDIOF_ALL, NULL))) {
      if (hr==DSERR_NODRIVER) {
         Log("error: no soundcard found");
         return ZFX_FAIL;
         }
      Log("error: InitAudio() failed");
      return ZFX_FAIL;
      }

   // get audiopath
   if (FAILED(m_pPerformance->GetDefaultAudioPath(&m_pDAPath))) {
      Log("error: GetDefaultAudioPath() failed");
      return ZFX_FAIL;
      }

   // get the listener object from the primary buffer
   if (FAILED(m_pDAPath->GetObjectInPath(
                           0, DMUS_PATH_PRIMARY_BUFFER,
                           0, GUID_NULL, 0, 
                           IID_IDirectSound3DListener8, 
                           (void**)&m_pListener))) {
      Log("error: GetObjectInPath(listener) failed");
      return ZFX_FAIL;
      }

   Log("initialized (online and ready)");
   m_bRunning = true;
   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 * Set parameters to position the listener in 3D space.
 */
void ZFXDA::SetListener(ZFXVector vPos, ZFXVector vDir, 
                        ZFXVector vUp,  ZFXVector vSpeed) {
   m_pListener->GetAllParameters(&m_dsListener);

   memcpy(&m_dsListener.vPosition,    &vPos,   g_szoff3);
   memcpy(&m_dsListener.vOrientFront, &vDir,   g_szoff3);
   memcpy(&m_dsListener.vOrientTop,   &vUp,    g_szoff3);
   memcpy(&m_dsListener.vVelocity,    &vSpeed, g_szoff3);
  
   if (m_pListener)
      m_pListener->SetAllParameters(&m_dsListener, DS3D_IMMEDIATE);
   } // SetListener
/*----------------------------------------------------------------*/


/**
 * Loads a sound file into a DirectMusic segment pointer.
 */
HRESULT ZFXDA::LoadSound(const char *chName, UINT *nID) {
   WCHAR    wName[MAX_PATH];
   HRESULT  hr;

   // build wide string from normal char
   if (MultiByteToWideChar(CP_ACP, 0, chName, -1, wName, MAX_PATH)==0)
      return ZFX_FAIL;

   // check if we already got that file
   for (UINT i=0; i<m_NumSounds; i++) {
      if (strcmp(chName, m_pSounds[i].chName)==0) {
         *nID = i;
         return ZFX_OK;
         }
      } // for

   // 50 new memory slots for sounds if necessary
   if ( (m_NumSounds%50) == 0 ) {
      int n = (m_NumSounds+50)*sizeof(ZFXSOUND);
      m_pSounds = (ZFXSOUND*)realloc(m_pSounds, n);
      if (!m_pSounds) return ZFX_OUTOFMEMORY;
      }

   m_pSounds[m_NumSounds].chName = new char[strlen(chName)+1];
   memcpy(m_pSounds[m_NumSounds].chName, chName, strlen(chName)+1);

   m_pSounds[m_NumSounds].bChanged = false;

   // load the file if exists and mem available
   if (FAILED(hr=m_pLoader->LoadObjectFromFile(
                              CLSID_DirectMusicSegment,
                              IID_IDirectMusicSegment8,
                              wName, (void**)
                              &m_pSounds[m_NumSounds].pSegment))) 
      {
      if ( (hr==DMUS_E_LOADER_FAILEDOPEN)  ||
           (hr==DMUS_E_LOADER_FAILEDCREATE)  ) {
         Log("error: LoadObjFromFile(%s) file not found", wName);
         return ZFX_FILENOTFOUND;
         }
      else if (hr==DMUS_E_LOADER_FORMATNOTSUPPORTED) {
         Log("error: LoadObjFromFile(%s) wrong format", wName);
         return ZFX_INVALIDPARAM;
         }
      else if (hr==E_OUTOFMEMORY) {
         Log("error: LoadObjFromFile(%s) out of memory", wName);
         return ZFX_OUTOFMEMORY;
         }
      return ZFX_FAIL;
      }
   
   // download instruments from performance
   if (FAILED( m_pSounds[m_NumSounds].pSegment->Download(m_pPerformance) )) {
      m_pSounds[m_NumSounds].pSegment->Release();
      m_pSounds[m_NumSounds].pSegment = NULL;
      Log("error: Download() failed");
      return ZFX_FAIL;
      }

   m_pPerformance->CreateStandardAudioPath( 
                        DMUS_APATH_DYNAMIC_3D,
                        64, TRUE,
                        &m_pSounds[m_NumSounds].p3DPath);


   m_pSounds[m_NumSounds].p3DPath->GetObjectInPath( 
                        DMUS_PCHANNEL_ALL,  // Performance channel.
                        DMUS_PATH_BUFFER,   // Stage in the path.
                        0,                  // Index of buffer in chain.
                        GUID_NULL,          // Class of object.
                        0,                  // Index of object in buffer; ignored.
                        IID_IDirectSound3DBuffer,
                        (void**)
                        &m_pSounds[m_NumSounds].p3DBuffer);

   m_NumSounds++;
   return ZFX_OK;
   } // LoadSound
/*----------------------------------------------------------------*/


/**
 * Play the sound. If you want then let it loose looping.
 */
void ZFXDA::PlaySound(UINT nID, bool bLoop) {
   if (nID >= m_NumSounds)
      return;

   // any not yet applied param changes
   if (m_pSounds[nID].bChanged) {
      m_pListener->CommitDeferredSettings();
      m_pSounds[nID].bChanged = false;
      }

   if (bLoop)
      m_pSounds[nID].pSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);

   // play as secondary buffer
   m_pPerformance->PlaySegment(m_pSounds[nID].pSegment, 
                               DMUS_SEGF_DEFAULT | 
                               DMUS_SEGF_SECONDARY, 
                               0, 0);
   } // PlaySound
/*----------------------------------------------------------------*/


/**
 * Just stop the sound from playing.
 */
void ZFXDA::StopSound(UINT nID) {
   if (nID >= m_NumSounds)
      return;

   m_pPerformance->Stop(m_pSounds[nID].pSegment, 0, 0, 0);
   } // StopSound
/*----------------------------------------------------------------*/


/**
 * Set the position of the sound in 3D space.
 */
void ZFXDA::SetSoundPosition(ZFXVector vPos, UINT nID) {
   IDirectSound3DBuffer8 *p3DBuffer;
   if (nID >= m_NumSounds) return;

   p3DBuffer = m_pSounds[m_NumSounds].p3DBuffer;
   m_pSounds[m_NumSounds].bChanged = true;

   p3DBuffer->GetAllParameters(&m_dsBuffer);
   
   m_dsBuffer.dwMode = DS3DMODE_NORMAL;
   memcpy(&m_dsBuffer.vPosition, &vPos, g_szoff3);
   
   p3DBuffer->SetAllParameters(&m_dsBuffer, DS3D_DEFERRED);
   } // SetSoundPosition
/*----------------------------------------------------------------*/


/**
 * Set the direction and velocity vector of a given sound.
 */
void ZFXDA::SetSoundDirection(ZFXVector vDir, ZFXVector vV, UINT nID) {
   IDirectSound3DBuffer8 *p3DBuffer;
   if (nID >= m_NumSounds) return;

   p3DBuffer = m_pSounds[m_NumSounds].p3DBuffer;
   m_pSounds[m_NumSounds].bChanged = true;

   p3DBuffer->GetAllParameters(&m_dsBuffer);
   
   m_dsBuffer.dwMode = DS3DMODE_NORMAL;
   memcpy(&m_dsBuffer.vVelocity, &vV, g_szoff3);
   memcpy(&m_dsBuffer.vConeOrientation, &vDir, g_szoff3);
   
   p3DBuffer->SetAllParameters(&m_dsBuffer, DS3D_DEFERRED);
   } // SetSoundDistance
/*----------------------------------------------------------------*/


/**
 * Set the max distance from where the sound can be heard.
 */
void ZFXDA::SetSoundMaxDist(float fDis, UINT nID) {
   if (nID >= m_NumSounds) return;
   IDirectSound3DBuffer8 *p3DBuffer;
   if (nID >= m_NumSounds) return;

   p3DBuffer = m_pSounds[m_NumSounds].p3DBuffer;
   m_pSounds[m_NumSounds].bChanged = true;

   p3DBuffer->GetAllParameters(&m_dsBuffer);
   
   m_dsBuffer.dwMode = DS3DMODE_NORMAL;
   m_dsBuffer.flMaxDistance = fDis;
   
   p3DBuffer->SetAllParameters(&m_dsBuffer, DS3D_DEFERRED);
   } // SetSoundMaxDist
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: bool - flush immediately
 *        char - format string to output
 *        ...  - output values
 */
void ZFXDA::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXDADevice]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/

