/**************************************************
WinMain.cpp
GameCore Component

Programming Role-Playing Games with DirectX
by Jim Adams (01 Jan 2002)
**************************************************/

#include "Core_Global.h"

//////////////////////////////////////////////////////////////////////
//
// cSound Functions
//
//////////////////////////////////////////////////////////////////////
cSound::cSound()
{
  short i;
  
  // Initialize COM
  CoInitialize(NULL);

  m_hWnd   = NULL;
  m_Volume = 0;

  m_hThread = NULL;
  m_ThreadID = NULL;
  m_ThreadActive = FALSE;

  m_pDMPerformance = NULL;
  m_pDMLoader      = NULL;

  m_pDS = NULL;
  m_pDSBPrimary = NULL;

  for(i=0;i<32;i++)
    m_EventChannel[i] = NULL;

  for(i=0;i<33;i++)
    m_Events[i] = NULL;
}

cSound::~cSound()
{
  Shutdown();

  // Uninitialize COM
  CoUninitialize();
}

BOOL cSound::Init(HWND hWnd, long Frequency, short Channels, short BitsPerSample, long CooperativeLevel)
{
  CHAR         strPath[MAX_PATH];
  WCHAR        wstrSearchPath[MAX_PATH];
  DSBUFFERDESC dsbd;
  WAVEFORMATEX wfex;
  long         VolumeLevel;
  short        i;
  
  // Shutdown system in case of prior install
  Shutdown();

  // Save parent window handle
  if((m_hWnd = hWnd) == NULL)
    return FALSE;

  ///////////////////////////////////////////////////////////////////
  // Initialize DirectSound
  ///////////////////////////////////////////////////////////////////

  // Save settings of sound setup
  if(CooperativeLevel == DSSCL_NORMAL)
    CooperativeLevel = DSSCL_PRIORITY;
  m_CooperativeLevel = CooperativeLevel;
  m_Frequency        = Frequency;
  m_Channels         = Channels;
  m_BitsPerSample    = BitsPerSample;

  // create an IDirectSound8 object
  if(FAILED(DirectSoundCreate8(NULL, &m_pDS, NULL)))
    return FALSE;

  // Set cooperative mode
  if(FAILED(m_pDS->SetCooperativeLevel(m_hWnd, m_CooperativeLevel)))
    return FALSE;

  // Get primary buffer control
  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
  dsbd.dwSize        = sizeof(DSBUFFERDESC);
  dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
  dsbd.dwBufferBytes = 0;
  dsbd.lpwfxFormat   = NULL;
  if(FAILED(m_pDS->CreateSoundBuffer(&dsbd, &m_pDSBPrimary, NULL)))
    return FALSE;

  // Set the primary buffer format
  ZeroMemory(&wfex, sizeof(WAVEFORMATEX)); 
  wfex.wFormatTag      = WAVE_FORMAT_PCM; 
  wfex.nChannels       = (WORD)m_Channels;
  wfex.nSamplesPerSec  = m_Frequency;
  wfex.wBitsPerSample  = (WORD)m_BitsPerSample;
  wfex.nBlockAlign     = wfex.wBitsPerSample / 8 * wfex.nChannels;
  wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
  if(FAILED(m_pDSBPrimary->SetFormat(&wfex)))
    return FALSE;

  // Create the events, plus 
  // an extra one for thread termination
  for(i=0;i<33;i++) {
    if((m_Events[i] = CreateEvent(NULL,FALSE,FALSE,NULL)) == NULL)
      return FALSE;
  }

  // Create a thread for handling notifications
  if((m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, (LPVOID)this, 0, &m_ThreadID)) == NULL)
    return FALSE;

  // Start main buffer playing
  if(FAILED(m_pDSBPrimary->Play(0, 0, DSBPLAY_LOOPING)))
    return FALSE;

  ///////////////////////////////////////////////////////////////////
  // Initialize DirectMusic
  ///////////////////////////////////////////////////////////////////

  // Create the DirectMusic loader object
  CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
                   IID_IDirectMusicLoader8, (void**)&m_pDMLoader);

  // Create the DirectMusic performance object
  CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                   IID_IDirectMusicPerformance8, (void**)&m_pDMPerformance);

  // Initialize the performance with the standard audio path.
  // This initializes both DirectMusic and DirectSound and 
  // sets up the synthesizer. 
  m_pDMPerformance->InitAudio(NULL, NULL, m_hWnd,
                              DMUS_APATH_SHARED_STEREOPLUSREVERB, 128,
                              DMUS_AUDIOF_ALL, NULL);

  // set the performance global volume to +10 decibels
  VolumeLevel = 1000;
  if(FAILED(m_pDMPerformance->SetGlobalParam(GUID_PerfMasterVolume, &VolumeLevel, sizeof(long))))
    return FALSE;

  // Tell DirectMusic where the default search path is
  GetCurrentDirectory(MAX_PATH, strPath);
  MultiByteToWideChar(CP_ACP, 0, strPath, -1, wstrSearchPath, MAX_PATH);
  m_pDMLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wstrSearchPath, FALSE);

  // Set default volume to full
  SetVolume(100);
  
  return TRUE;
}

BOOL cSound::Shutdown()
{
  short i;

  // Stop the music, and close down 
  if(m_pDMPerformance != NULL) {
    m_pDMPerformance->Stop(NULL, NULL, 0, 0);
    m_pDMPerformance->CloseDown();
  }

  // Release the DirectMusic objects
  ReleaseCOM(m_pDMPerformance);
  ReleaseCOM(m_pDMLoader);

  // Go through all used sound channels and free them
  for(i=0;i<32;i++) {
    if(m_EventChannel[i] != NULL) {
      m_EventChannel[i]->Free();
      m_EventChannel[i] = NULL;
    }

    // Clear the event status
    if(m_Events[i] != NULL)
      ResetEvent(m_Events[i]);
  }

  // Stop the primary channel from playing
  if(m_pDSBPrimary != NULL)
    m_pDSBPrimary->Stop();

  // Release the DirectSound objects
  ReleaseCOM(m_pDSBPrimary);
  ReleaseCOM(m_pDS);

  // Force a closure of the thread by triggering the last event
  // and waiting for it to terminate
  if(m_hThread != NULL) {
    if(m_Events[32] != NULL) {
      while(m_ThreadActive == TRUE)
        SetEvent(m_Events[32]);
    } else {
      // getting here means no event assigned on thread
      // and need to terminate it - not desirable
      TerminateThread(m_hThread, 0);
    }
  }

  // Close all event handles
  for(i=0;i<33;i++) {
    if(m_Events[i] != NULL) {
      CloseHandle(m_Events[i]);
      m_Events[i] = NULL;
    }
  }

  // Free the thread handle
  if(m_hThread != NULL) {
    CloseHandle(m_hThread);
    m_hThread = NULL;
  }
  m_ThreadID = NULL;

  return TRUE;
}

IDirectSound8 *cSound::GetDirectSoundCOM()
{
  return m_pDS;
}

IDirectSoundBuffer *cSound::GetPrimaryBufferCOM()
{
  return m_pDSBPrimary;
}

IDirectMusicPerformance8 *cSound::GetPerformanceCOM()
{
  return m_pDMPerformance;
}

IDirectMusicLoader8 *cSound::GetLoaderCOM()
{
  return m_pDMLoader;
}

BOOL cSound::AssignEvent(cSoundChannel *Channel, short *EventNum, HANDLE *EventHandle)
{
  short i;

  for(i=0;i<32;i++) {
    if(m_Events[i] != NULL && m_EventChannel[i] == NULL) {
      ResetEvent(m_Events[i]);
      m_EventChannel[i] = Channel;
      *EventNum = i;
      *EventHandle = m_Events[i];
          return TRUE;
    }
  }

  return FALSE;
}

BOOL cSound::ReleaseEvent(cSoundChannel *Channel, short *EventNum)
{
  if((unsigned short)*EventNum < 33 && m_EventChannel[*EventNum] == Channel) {
    ResetEvent(m_Events[*EventNum]);
    m_EventChannel[*EventNum] = NULL;
    *EventNum = -1;
    return TRUE;
  } 

  return FALSE;
}

long cSound::GetVolume()
{
  return m_Volume;
}

BOOL cSound::SetVolume(long Percent)
{
  long Volume;

  // Set the sound main volume
  if(m_pDSBPrimary == NULL)
    return FALSE;

  // calculate a usable volume level
  if(!Percent)
    Volume = DSBVOLUME_MIN;
  else 
    Volume = -20 * (100 - (Percent % 101));

  if(FAILED(m_pDSBPrimary->SetVolume(Volume)))
    return FALSE;

  m_Volume = Percent % 101;

  return TRUE;
}

DWORD cSound::HandleNotifications(LPVOID lpvoid)
{
  DWORD   dwResult, Channel;
  cSound *SoundPtr;
  BOOL    Complete;
  MSG     Msg;

  SoundPtr = (cSound*)lpvoid;

  SoundPtr->m_ThreadActive = TRUE;

  Complete = FALSE;

  while(Complete == FALSE) {
    // Wait for a message
    dwResult = MsgWaitForMultipleObjects(33, SoundPtr->m_Events,
                                         FALSE, INFINITE, 
                                         QS_ALLEVENTS);

    // Get channel # to update
    Channel = dwResult - WAIT_OBJECT_0;

    // Check for channel update
    if(Channel >=0 && Channel < 32) {
      if(SoundPtr->m_EventChannel[Channel] != NULL)
        SoundPtr->m_EventChannel[Channel]->Update();
    } else

    // Check for thread closure
    if(Channel == 32) {
      Complete = TRUE;
    } else

    // Check for waiting messages
    if(Channel > 32) {
      while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
        if(Msg.message == WM_QUIT) {
          Complete = TRUE;
          break;
        }
      }
    }
  }

  SoundPtr->m_ThreadActive = FALSE;

  return 0L;
}

BOOL cSound::Restore()
{
  short i;

  // Handle primary 
  if(m_pDSBPrimary != NULL)
    m_pDSBPrimary->Restore();

  // Handle all used sound channels
  for(i=0;i<32;i++) {
    if(m_EventChannel[i] != NULL)
      m_EventChannel[i]->m_pDSBuffer->Restore();
  }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// cSoundData Functions
//
//////////////////////////////////////////////////////////////////////
cSoundData::cSoundData()
{
  m_Frequency     = 22050;
  m_Channels      = 1;
  m_BitsPerSample = 16;

  m_fp       = NULL;
  m_Ptr      = m_Buf = NULL;
  m_StartPos = m_Pos = 0;
  m_Size     = m_Left = 0;
}

cSoundData::~cSoundData()
{
  Free();
}

BOOL cSoundData::Create()
{
  return Create(m_Size);
}

BOOL cSoundData::Create(long Size)
{
  // Free prior allocated data
  Free();

  // Check for valid size
  if((m_Size = Size) == 0)
    return FALSE;

  // Create a new buffer
  m_Buf = new char[m_Size];
  if(m_Buf == NULL)
    return FALSE;

  // Point to new buffer
  m_Ptr = m_Buf;
  m_fp  = NULL;

  // Clear out new buffer
  ZeroMemory(m_Buf, m_Size);

  return TRUE;
}

BOOL cSoundData::Free()
{
  if(m_Buf != NULL) {
    delete[] m_Buf;
    m_Buf = NULL;
  }

  m_Ptr = NULL;
  m_Size = 0;

  return TRUE;
}

char *cSoundData::GetPtr()
{
  return m_Buf;
}

long cSoundData::GetSize()
{
  return m_Size;
}

BOOL cSoundData::SetFormat(long Frequency, short Channels, short BitsPerSample)
{
  m_Frequency = Frequency;
  m_Channels = Channels;
  m_BitsPerSample = BitsPerSample;

  return TRUE;
}

BOOL cSoundData::SetSource(FILE *fp, long Pos, long Size)
{
  m_fp = fp;
  m_Ptr = NULL;

  if(Pos != -1)
    m_StartPos = m_Pos = Pos;
  if(Size != -1)
    m_Size = m_Left = Size;

  return TRUE;
}

BOOL cSoundData::SetSource(void *Ptr, long Pos, long Size)
{
  m_fp = NULL;
  m_Ptr = (char*)Ptr;
  if(Pos != -1)
    m_StartPos = m_Pos = Pos;
  if(Size != -1)
    m_Size = m_Left = Size;

  return TRUE;
}

BOOL cSoundData::LoadWAV(char *Filename, FILE *fp)
{
  if(LoadWAVHeader(Filename, fp) == FALSE)
    return FALSE;

  if(Create() == FALSE)
    return FALSE;

  // open file, seek to position and read in data
  if(Filename != NULL) {
    if((fp=fopen(Filename, "rb"))==NULL)
      return FALSE;
  }
  fseek(fp, m_StartPos, SEEK_SET);
  fread(m_Buf, 1, m_Size, fp);
  m_StartPos = m_Pos = 0;

  // close up file
  if(Filename != NULL)
    fclose(fp);

  return TRUE;
}

BOOL cSoundData::LoadWAVHeader(char *Filename, FILE *fp)
{
  sWaveHeader Hdr;
  BOOL ReturnVal;
  long Pos;

  if(Filename == NULL && fp == NULL)
    return FALSE;

  if(Filename != NULL) {
    if((fp=fopen(Filename, "rb"))==NULL)
      return FALSE;
  }

  // Save position in file
  Pos = ftell(fp);

  // Read in header and parse
  ReturnVal = FALSE;
  fread(&Hdr, 1, sizeof(sWaveHeader), fp);
  if(!memcmp(Hdr.RiffSig, "RIFF", 4) && !memcmp(Hdr.WaveSig, "WAVE", 4) && !memcmp(Hdr.FormatSig, "fmt ", 4) && !memcmp(Hdr.DataSig, "data", 4)) {
    m_Frequency     = Hdr.SampleRate;
    m_Channels      = Hdr.Channels;
    m_BitsPerSample = Hdr.BitsPerSample;

    m_Size = m_Left = Hdr.DataSize;
    m_StartPos = m_Pos = ftell(fp);

    ReturnVal = TRUE;
  }

  // Close if we opened file
  // otherwise return to original position
  if(Filename != NULL)
    fclose(fp);
  else 
    fseek(fp, Pos, SEEK_SET);

  return ReturnVal;
}

BOOL cSoundData::Copy(cSoundData *Source)
{
  if(Source == NULL)
    return FALSE;
  
  m_Frequency     = Source->m_Frequency;
  m_Channels      = Source->m_Channels;
  m_BitsPerSample = Source->m_BitsPerSample;

  m_fp       = Source->m_fp;
  m_Ptr      = Source->m_Ptr;
  m_Size     = Source->m_Size;
  m_Left     = Source->m_Left;
  m_Pos      = Source->m_Pos;
  m_StartPos = Source->m_StartPos;

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// cSoundChannel Functions
//
//////////////////////////////////////////////////////////////////////
cSoundChannel::cSoundChannel()
{
  m_Sound     = NULL;
  m_pDSBuffer = NULL;
  m_pDSNotify = NULL;

  m_Event     = -1;

  m_Volume    = 0;
  m_Pan       = 0;
  m_Frequency = 0;
  m_Playing   = FALSE;
}

cSoundChannel::~cSoundChannel()
{
  Free();
}

IDirectSoundBuffer8 *cSoundChannel::GetSoundBufferCOM()
{
  return m_pDSBuffer;
}

IDirectSoundNotify8 *cSoundChannel::GetNotifyCOM()
{
  return m_pDSNotify;
}    

BOOL cSoundChannel::Create(cSound *Sound, long Frequency, short Channels, short BitsPerSample)
{
  DSBUFFERDESC dsbd;
  WAVEFORMATEX wfex;
  HANDLE       hEvent;
  DSBPOSITIONNOTIFY   dspn[4];
  IDirectSoundBuffer *pDSBuffer;

  // Free a prior channel
  Free();

  if((m_Sound = Sound) == NULL)
    return FALSE;
  if(m_Sound->GetDirectSoundCOM() == NULL)
    return FALSE;

  // Save playback format
  m_Frequency     = Frequency;
  m_BitsPerSample = BitsPerSample;
  m_Channels      = Channels;

  // Create a new sound buffer for this channel
  // Using specified format
  ZeroMemory(&wfex, sizeof(WAVEFORMATEX)); 
  wfex.wFormatTag      = WAVE_FORMAT_PCM; 
  wfex.nChannels       = (WORD)m_Channels;
  wfex.nSamplesPerSec  = m_Frequency;
  wfex.wBitsPerSample  = (WORD)m_BitsPerSample;
  wfex.nBlockAlign     = wfex.wBitsPerSample / 8 * wfex.nChannels;
  wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
  dsbd.dwSize        = sizeof(DSBUFFERDESC);
  dsbd.dwFlags       = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
  dsbd.dwBufferBytes = g_SoundBufferSize;
  dsbd.lpwfxFormat   = &wfex;
  if(FAILED(m_Sound->GetDirectSoundCOM()->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL)))
    return FALSE;

  // query for newer interface
  if(FAILED(pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pDSBuffer))) {
    pDSBuffer->Release();
    return FALSE;
  }

  // Release old object - we have the newer one now
  pDSBuffer->Release();

  // Create the notification interface
  if(FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&m_pDSNotify)))
  //if(FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&m_pDSNotify)))
    return FALSE;

  // Get an event for this
  if(m_Sound->AssignEvent(this, &m_Event, &hEvent) == FALSE)
    return FALSE;

  // Setup the 4 notification positions
  dspn[0].dwOffset = g_SoundBufferChunk - 1;
  dspn[0].hEventNotify = hEvent;
  dspn[1].dwOffset = g_SoundBufferChunk * 2 - 1;
  dspn[1].hEventNotify = hEvent;
  dspn[2].dwOffset = g_SoundBufferChunk * 3 - 1;
  dspn[2].hEventNotify = hEvent;
  dspn[3].dwOffset = g_SoundBufferSize - 1;
  dspn[3].hEventNotify = hEvent;

  if(FAILED(m_pDSNotify->SetNotificationPositions(4, dspn)))
    return FALSE;

  // set the pan and default volume
  SetVolume(100);
  SetPan(0);

  return TRUE;
}

BOOL cSoundChannel::Create(cSound *Sound, cSoundData *SoundDesc)
{
  return Create(Sound, SoundDesc->m_Frequency, SoundDesc->m_Channels, SoundDesc->m_BitsPerSample);
}

BOOL cSoundChannel::Free()
{
  // Stop any playback
  Stop();

  // Release the notification
  ReleaseCOM(m_pDSNotify);

  // Release the buffer
  ReleaseCOM(m_pDSBuffer);

  // Release event from parent cSound class
  m_Sound->ReleaseEvent(this, &m_Event);

  // Set to no parent sound
  m_Sound = NULL;

  return TRUE;
}

BOOL cSoundChannel::Play(cSoundData *Desc, long VolumePercent, long Loop)
{
  if(Desc == NULL)
    return FALSE;
  if(m_pDSBuffer == NULL)
    return FALSE;
  if(m_pDSNotify == NULL)
    return FALSE;

  // Stop any playback
  Stop();

  // Restore a lost buffer just in case
  m_pDSBuffer->Restore();

  // Setup playing information
  m_Desc.Copy(Desc);
  
  // Set looping data
  m_Loop = Loop;

  // Calculate stop section position
  if(!m_Loop)
    m_StopSection = -1;
  else
    m_StopSection = (short)(((m_Desc.m_Size * m_Loop) % g_SoundBufferSize) / g_SoundBufferChunk);

  // Buffer in data
  m_LoadSection = 0;
  BufferData();
  BufferData();
  BufferData();
  BufferData();

  // Set the volume
  SetVolume(VolumePercent);

  // Set position and begin play
  m_NextNotify = 0;
  if(FAILED(m_pDSBuffer->SetCurrentPosition(0)))
    return FALSE;
  if(FAILED(m_pDSBuffer->Play(0,0,DSBPLAY_LOOPING)))
    return FALSE;

  // Flag as playing
  m_Playing = TRUE;

  return TRUE;
}

BOOL cSoundChannel::Stop()
{
  if(m_pDSBuffer)
    m_pDSBuffer->Stop();

  m_Playing = FALSE;

  return TRUE;
}

long cSoundChannel::GetVolume()
{
  return m_Volume;
}

BOOL cSoundChannel::SetVolume(long Percent)
{
  long Volume;

  if(m_pDSBuffer == NULL)
    return FALSE;

  // calculate a usable volume level
  if(!Percent)
    Volume = DSBVOLUME_MIN;
  else 
    Volume = -20 * (100 - (Percent % 101));

  if(FAILED(m_pDSBuffer->SetVolume(Volume)))
    return FALSE;

  m_Volume = Percent % 101;

  return TRUE;
}

signed long cSoundChannel::GetPan()
{
  return m_Pan;
}

BOOL cSoundChannel::SetPan(signed long Level)
{
  signed long Pan;

  if(m_pDSBuffer == NULL)
    return FALSE;

  // calculate a usable setting
  if(Level < 0) 
    Pan = DSBPAN_LEFT / 100 * ((-Level) % 101);
  else
    Pan = DSBPAN_RIGHT / 100 * (Level % 101);

  if(FAILED(m_pDSBuffer->SetPan(Pan)))
    return FALSE;

  m_Pan = Level % 101;

  return TRUE;  
}

long cSoundChannel::GetFrequency()
{
  return m_Frequency;
}

BOOL cSoundChannel::SetFrequency(long Level)
{
  if(m_pDSBuffer == NULL)
    return FALSE;

  if(FAILED(m_pDSBuffer->SetFrequency(Level)))
    return FALSE;

  m_Frequency = Level;

  return TRUE;
}

BOOL cSoundChannel::IsPlaying()
{
  if(m_Sound == NULL || m_pDSBuffer == NULL || m_pDSNotify == NULL)
    return FALSE;

  return m_Playing;
}

BOOL cSoundChannel::BufferData()
{
  long Pos, Size;
  long ToLoad, LoadPos;
  char *Ptr;

  if(m_pDSBuffer == NULL)
    return FALSE;

  // Setup position to load in
  Pos  = (m_LoadSection % 4) * g_SoundBufferChunk;
  if(FAILED(m_pDSBuffer->Lock(Pos, g_SoundBufferChunk, (void**)&Ptr, (DWORD*)&Size, NULL, NULL, 0)))
    return FALSE;
    
  // Clear out buffer if nothing left to load
  if(!m_Desc.m_Left)
    ZeroMemory(Ptr, Size);
  else {
    // Load in the data - take looping into account
    ToLoad = Size;
    LoadPos = 0;
    for(;;) {
      if(m_Desc.m_Left > ToLoad) {
        if(m_Desc.m_fp != NULL) {
          fseek(m_Desc.m_fp, m_Desc.m_Pos, SEEK_SET);
          fread(&Ptr[LoadPos], 1, ToLoad, m_Desc.m_fp);
        } else 
          memcpy(&Ptr[LoadPos], &m_Desc.m_Ptr[m_Desc.m_Pos], ToLoad);
        m_Desc.m_Left -= ToLoad;
        m_Desc.m_Pos += ToLoad;
        break;
      } else {
        if(m_Desc.m_fp != NULL) {
          fseek(m_Desc.m_fp, m_Desc.m_Pos, SEEK_SET);
          fread(&Ptr[LoadPos], 1, m_Desc.m_Left, m_Desc.m_fp);
        } else 
          memcpy(&Ptr[LoadPos], &m_Desc.m_Ptr[m_Desc.m_Pos], m_Desc.m_Left);
        ToLoad -= m_Desc.m_Left;
        LoadPos += m_Desc.m_Left;

        // Check if we need to stop loop
        if(m_Loop >= 1) {
          m_Loop--;
          if(!m_Loop) {
            // Clear out remaining buffer space
            if(ToLoad)
              ZeroMemory(&Ptr[LoadPos], ToLoad);
            m_Desc.m_Left = 0L;
            break;
          }
        }

        m_Desc.m_Pos = m_Desc.m_StartPos;
        m_Desc.m_Left = m_Desc.m_Size;

        // See if we need to stop loading data
        if(!ToLoad)
          break;
      }
    }
  }

  // Unlock the buffer
  m_pDSBuffer->Unlock(Ptr, Size, NULL, 0);

  // Mark next section to load
  if((m_LoadSection+=1) > 3)
    m_LoadSection = 0;

  return TRUE;
}

BOOL cSoundChannel::Update()
{
  // Check for end of sound
  if(m_NextNotify == m_StopSection && !m_Desc.m_Left) {
    Stop();
  } else {
    // Buffer in more data
    BufferData();
    if((m_NextNotify+=1) > 3)
      m_NextNotify = 0;
  }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// cMusicChannel Functions
//
//////////////////////////////////////////////////////////////////////
cMusicChannel::cMusicChannel()
{
  m_Sound      = NULL;
  m_pDMSegment = NULL;
  m_Volume     = 0;
}

cMusicChannel::~cMusicChannel()
{
  Free();
}

IDirectMusicSegment8 *cMusicChannel::GetSegmentCOM()
{
  return m_pDMSegment;
}

BOOL cMusicChannel::Create(cSound *Sound)
{
  Free();

  // Make sure all objects there
  if((m_Sound = Sound) == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;
  if(m_Sound->GetLoaderCOM() == NULL)
    return FALSE;

  return TRUE;
}

BOOL cMusicChannel::Load(char *Filename)
{
  DMUS_OBJECTDESC dmod;

  Free();

  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;
  if(m_Sound->GetLoaderCOM() == NULL)
    return FALSE;

  // Get the object
  ZeroMemory(&dmod, sizeof(DMUS_OBJECTDESC));
  dmod.dwSize = sizeof(DMUS_OBJECTDESC);
  dmod.guidClass = CLSID_DirectMusicSegment;
  dmod.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH;
  mbstowcs(dmod.wszFileName, Filename, MAX_PATH);
  if(FAILED(m_Sound->GetLoaderCOM()->GetObject(&dmod, IID_IDirectMusicSegment8, (LPVOID*)&m_pDMSegment)))
    return FALSE;

  // Setup MIDI playing
  if(strstr(Filename, ".mid") != NULL) {
    if(FAILED(m_pDMSegment->SetParam(GUID_StandardMIDIFile, 0xFFFFFFFF, 0, 0, NULL)))
      return FALSE;
  }

  // Download the band
  if(FAILED(m_pDMSegment->Download(m_Sound->GetPerformanceCOM())))
    return FALSE;

  return TRUE;
}

BOOL cMusicChannel::Free()
{
  Stop();

  if(m_Sound != NULL) {
    // unload instrument data
    if(m_pDMSegment != NULL) {
      if(m_Sound->GetPerformanceCOM() != NULL) {
        if(FAILED(m_pDMSegment->Unload(m_Sound->GetPerformanceCOM())))
          return FALSE;
      }

      // free loader data
      if(m_Sound->GetLoaderCOM() != NULL) {
        if(FAILED(m_Sound->GetLoaderCOM()->ReleaseObjectByUnknown(m_pDMSegment)))
          return FALSE;
      }
    }
  }

  // release the segment
  ReleaseCOM(m_pDMSegment);
  
  return TRUE;
}

BOOL cMusicChannel::SetDLS(cDLS *DLS)
{
  if(DLS == NULL)
    return FALSE;
  if(DLS->GetCollectionCOM() == NULL)
    return FALSE;
  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;
  if(m_pDMSegment == NULL)
    return FALSE;

  // Connect to the collection
  if(FAILED(m_pDMSegment->SetParam(GUID_ConnectToDLSCollection, 0xFFFFFFFF, 0, 0, (void*)DLS->GetCollectionCOM())))
    return FALSE;

  // unload and then re-download new instruments
  if(FAILED(m_pDMSegment->Unload(m_Sound->GetPerformanceCOM())))
    return FALSE;
  if(FAILED(m_pDMSegment->Download(m_Sound->GetPerformanceCOM())))
    return FALSE;
  
  return TRUE;
}

BOOL cMusicChannel::Play(long VolumePercent, long Loop)
{
  Stop();

  // Return if not setup correctly
  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;
  if(m_pDMSegment == NULL)
    return FALSE;

  // Set the number of loops
  if(!Loop)
    m_pDMSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
  else
    m_pDMSegment->SetRepeats(Loop-1);

  // Set the playback volume
  SetVolume(VolumePercent);

  // Play on default audio path
  if(FAILED(m_Sound->GetPerformanceCOM()->PlaySegmentEx(
               m_pDMSegment, NULL, NULL, 
               0, 0, NULL, NULL, NULL)))
    return FALSE;

  return TRUE;
}

BOOL cMusicChannel::Stop()
{
  // Return if not setup correctly
  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;
  if(m_pDMSegment == NULL)
    return FALSE;

  // Stop playback 
  if(FAILED(m_Sound->GetPerformanceCOM()->Stop(m_pDMSegment, NULL, 0, 0)))
    return FALSE;

  return TRUE;
}

long cMusicChannel::GetVolume()
{
  return m_Volume;
}

BOOL cMusicChannel::SetVolume(long Percent)
{
  IDirectMusicAudioPath8 *pDMAudioPath;
  long Volume;

  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;

  if(FAILED(m_Sound->GetPerformanceCOM()->GetDefaultAudioPath(&pDMAudioPath)))
    return FALSE;

  // calculate a usable volume level
  if(!Percent)
    Volume = -9600;
  else
    Volume = (long)(-32.0 * (100.0 - (float)(Percent % 101)));

  if(FAILED(pDMAudioPath->SetVolume(Volume,0))) {
    pDMAudioPath->Release();
    return FALSE;
  }
  pDMAudioPath->Release();

  m_Volume = Percent % 101;

  return TRUE;
}

BOOL cMusicChannel::SetTempo(long Percent)
{
  float Tempo;

  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;

  // calculate tempo setting based on percentage
  Tempo = (float)Percent / 100.0f;

  // set master performance tempo
  if(FAILED(m_Sound->GetPerformanceCOM()->SetGlobalParam(GUID_PerfMasterTempo, (void*)&Tempo, sizeof(float))))
    return FALSE;

  return TRUE;
}

BOOL cMusicChannel::IsPlaying()
{
  // Return if not setup correctly
  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetPerformanceCOM() == NULL)
    return FALSE;
  if(m_pDMSegment == NULL)
    return FALSE;

  if(m_Sound->GetPerformanceCOM()->IsPlaying(m_pDMSegment, NULL) == S_OK)
    return TRUE;

  return FALSE;
}

cDLS::cDLS()
{
  m_Sound = NULL;
  m_pDMCollection = NULL;
}

cDLS::~cDLS()
{
  Free();
}

BOOL cDLS::Create(cSound *Sound)
{
  Free();

  if((m_Sound = Sound) == NULL)
    return FALSE;
  if(m_Sound->GetLoaderCOM() == NULL)
    return FALSE;

  return TRUE;
}

BOOL cDLS::Load(char *Filename)
{
  DMUS_OBJECTDESC dmod;

  Free();

  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetLoaderCOM() == NULL)
    return FALSE;

  ZeroMemory(&dmod, sizeof(DMUS_OBJECTDESC));
  dmod.dwSize = sizeof(DMUS_OBJECTDESC);
  dmod.guidClass = CLSID_DirectMusicCollection;

  if(Filename == NULL) {
    // Get the default collection
    dmod.guidObject = GUID_DefaultGMCollection;
    dmod.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_OBJECT;
  } else {
    // Get the collection object
    dmod.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH;
    mbstowcs(dmod.wszFileName, Filename, MAX_PATH);
  }

  if(FAILED(m_Sound->GetLoaderCOM()->GetObject(&dmod, IID_IDirectMusicCollection8, (LPVOID*)&m_pDMCollection)))
    return FALSE;

  return TRUE;
}

BOOL cDLS::Free()
{
  if(m_Sound == NULL)
    return FALSE;
  if(m_Sound->GetLoaderCOM() == NULL)
    return FALSE;

  if(m_pDMCollection != NULL) {
    if(FAILED(m_Sound->GetLoaderCOM()->ReleaseObjectByUnknown(m_pDMCollection)))
      return FALSE;
  }
  ReleaseCOM(m_pDMCollection);

  return TRUE;
}

IDirectMusicCollection8 *cDLS::GetCollectionCOM()
{
  return m_pDMCollection;
}

long cDLS::GetNumPatches()
{
  HRESULT hr;
  DWORD   dwPatch;
  long    i;

  hr = S_OK;
  for(i=0; hr == S_OK; i++) {
    hr = m_pDMCollection->EnumInstrument(i, &dwPatch, NULL, 0);
    if(hr != S_OK)
      break;
  }
  return i;
}

long cDLS::GetPatch(long Index)
{
  DWORD dwPatch;

  if(m_pDMCollection == NULL)
    return -1;

  if(FAILED(m_pDMCollection->EnumInstrument(Index, &dwPatch, NULL, 0)))
    return -1;

  return (long)dwPatch;
}

BOOL cDLS::Exists(long Patch)
{
  IDirectMusicInstrument8 *pDMInstrument;

  if(m_pDMCollection == NULL)
    return FALSE;

  if(FAILED(m_pDMCollection->GetInstrument(Patch, &pDMInstrument)))
    return FALSE;
  pDMInstrument->Release();

  return TRUE;
}

IGraphBuilder  *m_pDSGraphBuilder;
IMediaControl  *m_pDSMediaControl;
IMediaEvent    *m_pDSMediaEvent;

cMP3::cMP3()
{
  m_pDSGraphBuilder = NULL;
  m_pDSMediaControl = NULL;
  m_pDSMediaEvent   = NULL;
}

cMP3::~cMP3()
{
  Shutdown();
}

BOOL cMP3::Init()
{
  Shutdown();  // Shutdown prior initialized system

  // Initialize COM
  CoInitialize(NULL);

  // Create graph builder object and media controls
  if(FAILED(CoCreateInstance(CLSID_FilterGraph, NULL,          \
                             CLSCTX_INPROC_SERVER,             \
                             IID_IGraphBuilder,                \
                             (void**)&m_pDSGraphBuilder)))
    return FALSE;

  m_pDSGraphBuilder->QueryInterface(IID_IMediaControl,         \
                               (void**)&m_pDSMediaControl); 
  m_pDSGraphBuilder->QueryInterface(IID_IMediaEvent,           \
                               (void**)&m_pDSMediaEvent);

  return TRUE;
}

BOOL cMP3::Shutdown()
{
  ReleaseCOM(m_pDSMediaEvent);
  ReleaseCOM(m_pDSMediaControl);
  ReleaseCOM(m_pDSGraphBuilder);

  CoUninitialize();
  
  return TRUE;
}

BOOL cMP3::Render(char *Filename)
{
  // Error checking
  if(m_pDSGraphBuilder == NULL || m_pDSMediaControl == NULL)
    return FALSE;

  // Stop the current song
  m_pDSMediaControl->Stop();

  // Convert filename to wide-character string
  WCHAR wFilename[MAX_PATH];
  mbstowcs(wFilename, Filename, MAX_PATH);

  // Render the file
  if(FAILED(m_pDSGraphBuilder->RenderFile(wFilename, NULL)))
    return FALSE;

  // Return success
  return TRUE;
}

BOOL cMP3::Play()
{
  // Error checking
  if(m_pDSMediaControl == NULL)
    return FALSE;

  // Try to play media
  if(FAILED(m_pDSMediaControl->Run()))
    return FALSE;

  return TRUE;
}

BOOL cMP3::Stop()
{
  // Error checking
  if(m_pDSMediaControl == NULL)
    return FALSE;

  // Try to stop media
  if(FAILED(m_pDSMediaControl->Stop()))
    return FALSE;

  return TRUE;
}

BOOL cMP3::Pause()
{
  // Error checking
  if(m_pDSMediaControl == NULL)
    return FALSE;

  // Try to pause media
  if(FAILED(m_pDSMediaControl->Pause()))
    return FALSE;

  return TRUE;
}

BOOL cMP3::Playing()
{
  // Error checking
  if(m_pDSMediaEvent == NULL)
    return FALSE;

  // Get event and handle it
  long Event, Param1, Param2;
  m_pDSMediaEvent->GetEvent(&Event, &Param1, &Param2, 1);
  if(Event == EC_COMPLETE) {
    m_pDSMediaEvent->FreeEventParams(Event, Param1, Param2);
    return TRUE;
  }
  m_pDSMediaEvent->FreeEventParams(Event, Param1, Param2);
 
  return FALSE;
}
