/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<XACTSystem.h>


DECLARE_ENGINE_NAMESPACE


// *** XACTSystem ***


bool XACTSystem::SetupXACT()
{
   if(FAILED(XACTCreateEngine(XACT_FLAG_API_AUDITION_MODE, &m_soundEngine)))
      return false;

   if(m_soundEngine == NULL)
      return false;

   XACT_RUNTIME_PARAMETERS xparams = {0};
   xparams.lookAheadTime = 250;

   if(FAILED(m_soundEngine->Initialize(&xparams)))
      return false;

   return true;
}


void XACTSystem::Cleanup()
{
   if(m_soundEngine)
      {
         m_soundEngine->ShutDown();
         m_soundEngine->Release();
      }

   m_soundEngine = NULL;
}


void XACTSystem::Update()
{
   if(m_soundEngine)
      m_soundEngine->DoWork();
}


// *** XACTAudio ***

      
void XACTAudio::Cleanup()
{
   if(m_soundBankData)
      {
         UnmapViewOfFile(m_soundBankData);
         m_soundBankData = NULL;
      }

   if(m_waveBankData)
      {
         UnmapViewOfFile(m_waveBankData);
         m_waveBankData = NULL;
      }

   m_soundSystem = NULL;
}


bool XACTAudio::Initialize(char *waveBank, char *soundBank, XACTSystem *soundSystem)
{
   if(soundSystem == NULL)
      return false;

   m_soundSystem = soundSystem;

   if(m_soundSystem == NULL)
      return false;

   if(!LoadBank(waveBank, BB_WAVE_BANK_TYPE))
      return false;

   if(!LoadBank(soundBank, BB_SOUND_BANK_TYPE))
      return false;

   return true;
}    


bool XACTAudio::LoadBank(char *fileName, BB_BANK_TYPE bankType)
{
   if(m_soundSystem == NULL)
      return false;

   HANDLE file;

   file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ,
                     NULL, OPEN_EXISTING, 0, NULL);

   if(file == INVALID_HANDLE_VALUE)
      return false;

   DWORD fileSize;
   HANDLE mapFile;
   void *ptr;
   bool result = true;
   IXACTEngine *xactEngine = NULL;

   fileSize = GetFileSize(file, NULL);

   if(fileSize == -1)
      {
         CloseHandle(file);
         return false;
      }

   // Create memory mapped data since its faster.
   mapFile = CreateFileMapping(file, NULL, PAGE_READONLY, 0, fileSize, NULL);

   if(!mapFile)
      {
         CloseHandle(file);
         return false;
      }

   ptr = MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, 0);

   if(!ptr)
      {
         CloseHandle(mapFile);
         CloseHandle(file);
         return false;
      }

   xactEngine = m_soundSystem->GetXACTEngine();

   if(xactEngine != NULL)
      {
         if(bankType == BB_WAVE_BANK_TYPE)
            {
               m_waveBankData = ptr;

               if(FAILED(xactEngine->CreateInMemoryWaveBank(m_waveBankData,
                  fileSize, 0, 0, &m_waveBank)))
                  {
                     result = false;
                  }
            }
         else
            {
               m_soundBankData = ptr;

               if(FAILED(xactEngine->CreateSoundBank(m_soundBankData,
                  fileSize, 0, 0, &m_soundBank)))
                  {
                     result = false;
                  }
            }
      }
   else
      {
         result = false;
      }

   CloseHandle(mapFile);
   CloseHandle(file);

   return result;
}


// *** XACTCue ***


void XACTCue::GetCue(char *name, XACTAudio *audioBanks)
{
   IXACTSoundBank *sBank = NULL;

   if(audioBanks == NULL)
      return;

   m_soundBanks = audioBanks;

   if(m_soundBanks != NULL)
      sBank = m_soundBanks->GetSoundBank();

   if(sBank != NULL)
      m_cueIndex = sBank->GetCueIndex("tap");
}


bool XACTCue::isPlaying()
{
   unsigned long state = 0;
   IXACTSoundBank *sBank = NULL;

   if(m_soundBanks == NULL)
      return false;

   if(m_soundBanks != NULL)
      sBank = m_soundBanks->GetSoundBank();

   if(sBank == NULL)
      return false;

   sBank->GetState(&state);

   if(state && XACT_CUESTATE_PLAYING)
      return true;

   return false;
}


void XACTCue::PlaySound()
{
   IXACTSoundBank *sBank = NULL;

   if(m_soundBanks != NULL)
      sBank = m_soundBanks->GetSoundBank();

   if(sBank != NULL)
      sBank->Play(m_cueIndex, 0, 0, NULL);
}


void XACTCue::StopSound()
{
   IXACTSoundBank *sBank = NULL;

   if(m_soundBanks != NULL)
      sBank = m_soundBanks->GetSoundBank();

   if(sBank != NULL)
      sBank->Stop(m_cueIndex, 0);
}


END_ENGINE_NAMESPACE