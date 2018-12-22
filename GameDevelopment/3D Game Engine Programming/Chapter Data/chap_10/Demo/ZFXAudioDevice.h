// File: ZFXAudioDevice.h
#ifndef ZFXAUDIOINTERFACE_H
#define ZFXAUDIOINTERFACE_H


// I N C L U D E S /////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "ZFX.h"


// D E F I N E S ///////////////////////////////////////////////////




// S T R U C T S ///////////////////////////////////////////////////

/**
 * 
 */
class ZFXAudioDevice {
   protected:
      HWND       m_hWndMain;          // application main window
      HINSTANCE  m_hDLL;              // dll module handle
      bool       m_bRunning;          // after succesful initialization
      FILE      *m_pLog;              // log file
      
      
   public:
      ZFXAudioDevice(void) {};
      virtual ~ZFXAudioDevice(void) {};
      
      
      // INIT/RELEASE STUFF:
      // ===================
      
      virtual HRESULT Init(HWND, const char*, bool)=0;
      
      // release API specific stuff
      virtual void Release(void)=0;
      
      // is initialized?
      virtual bool IsRunning(void)=0;

      // stop all audio at once
      virtual void StopAll(void)=0;

      // load WAV from file
      virtual HRESULT LoadSound(const char*, UINT*)=0;

      // play a specific sound
      virtual void PlaySound(UINT, bool bLoop)=0;

      // stop a specific sound
      virtual void StopSound(UINT)=0;

      // set listener parameters
      virtual void SetListener(ZFXVector vPos, ZFXVector vDir, 
                               ZFXVector vUp,  ZFXVector vV)=0;

      // set sound parameters
      virtual void SetSoundPosition(ZFXVector, UINT)=0;
      virtual void SetSoundDirection(ZFXVector, ZFXVector vV, UINT)=0;
      virtual void SetSoundMaxDist(float, UINT)=0;

   }; // class

typedef class ZFXAudioDevice *LPZFXAUDIODEVICE;
/*----------------------------------------------------------------*/


extern "C" {
   HRESULT CreateAudioDevice(HINSTANCE hDLL, ZFXAudioDevice **pInterface);
   typedef HRESULT (*CREATEAUDIODEVICE)(HINSTANCE hDLL, ZFXAudioDevice **pInterface);
   
   HRESULT ReleaseAudioDevice(ZFXAudioDevice **pInterface);
   typedef HRESULT (*RELEASEAUDIODEVICE)(ZFXAudioDevice **pInterface);
   }
/*----------------------------------------------------------------*/

#endif