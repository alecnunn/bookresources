/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_XACT_H_
#define _BB_XACT_H_

#define _WIN32_DCOM
#include<windows.h>
#include<xact.h>
#include<NameSpace.h>


DECLARE_ENGINE_NAMESPACE


class XACTSystem
{
   public:
      XACTSystem() { m_soundEngine = NULL; }
      ~XACTSystem() { Cleanup(); }

      bool SetupXACT();
      void Cleanup();
      void Update();

      IXACTEngine *GetXACTEngine()
      {
         return m_soundEngine;
      }


   private:
      IXACTEngine *m_soundEngine;
};


class XACTAudio
{
   public:
      XACTAudio()
      {
         m_waveBank = NULL;
         m_soundBank = NULL;
         m_waveBankData = NULL;
         m_soundBankData = NULL;
         m_soundSystem = NULL;
      }

      ~XACTAudio()
      {
         Cleanup();
      }
      
      bool Initialize(char *waveBank, char *soundBank,
                      XACTSystem *soundSystem);
      void Cleanup();

      IXACTSoundBank *GetSoundBank() { return m_soundBank; }
      XACTSystem *GetSoundSystem() { return m_soundSystem; }


   private:
      enum BB_BANK_TYPE { BB_WAVE_BANK_TYPE = 1, BB_SOUND_BANK_TYPE };

      bool LoadBank(char *fileName, BB_BANK_TYPE bankType);

      XACTSystem *m_soundSystem;
      IXACTWaveBank *m_waveBank;
      IXACTSoundBank *m_soundBank;

      void *m_waveBankData;
      void *m_soundBankData;
};


class XACTCue
{
   public:
      XACTCue()
      {
         m_cueIndex = 0;
         m_soundBanks = NULL;
      }

      ~XACTCue()
      {

      }

      void GetCue(char *name, XACTAudio *audioBanks);

      bool isPlaying();
      void PlaySound();
      void StopSound();


   private:
      XACTINDEX m_cueIndex;
      XACTAudio *m_soundBanks;
      
};


END_ENGINE_NAMESPACE

#endif