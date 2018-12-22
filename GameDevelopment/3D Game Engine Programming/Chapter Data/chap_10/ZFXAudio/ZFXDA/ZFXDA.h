// File: ZFXDA.h
#ifndef ZFXDA_H
#define ZFXDA_H

#include <windows.h>
//#include <dmusicc.h>
#include <dmusici.h>
#include <dsound.h>



BOOL WINAPI DllEntryPoint( HINSTANCE hDll,
                           DWORD     fdwReason,
                           LPVOID    lpvRserved );

#include "..\ZFXAudioDevice.h"
#include "..\ZFX.h"
#include <dsound.h>


// struct to store a sound file
typedef struct ZFXSOUND_TYPE {
   char                   *chName;
   bool                    bChanged;
   IDirectMusicSegment8   *pSegment;
   IDirectMusicAudioPath8 *p3DPath;
   IDirectSound3DBuffer8  *p3DBuffer;
   } ZFXSOUND;
/*----------------------------------------------------------------*/


/**
 * Class definition for a DirectAudio sound device, implementing the
 * interface ZFXAudioDevice
 */
class ZFXDA : public ZFXAudioDevice {
   public:
      ZFXDA(HINSTANCE hDLL);
      ~ZFXDA(void);
      
      // initialize the engine stuff
      HRESULT Init(HWND, const char*, bool);
     
      // Interface functions
      void    Release(void);
      bool    IsRunning(void) { return m_bRunning; }
      void    StopAll(void)   { if (m_pPerformance) m_pPerformance->Stop(NULL,NULL,0,0); }
      HRESULT LoadSound(const char*, UINT*);

      void    SetListener(ZFXVector,ZFXVector,ZFXVector,ZFXVector);

      void    SetSoundPosition(ZFXVector,UINT);
      void    SetSoundDirection(ZFXVector,ZFXVector,UINT);
      void    SetSoundMaxDist(float,UINT);

      void    PlaySound(UINT nID, bool);
      void    StopSound(UINT nID);
      

   private:
      IDirectMusicLoader8      *m_pLoader;
      IDirectMusicPerformance8 *m_pPerformance;
      IDirectSound3DListener8  *m_pListener;
      IDirectMusicAudioPath8   *m_pDAPath;
      DS3DLISTENER              m_dsListener; 
      DS3DBUFFER                m_dsBuffer;

      ZFXSOUND                 *m_pSounds;
      UINT                      m_NumSounds;

      // write to log file
      void Log(char *, ...);
   }; // class
/*----------------------------------------------------------------*/


#endif