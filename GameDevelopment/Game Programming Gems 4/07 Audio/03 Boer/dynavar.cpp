

#include "dynavar.h"


//----------------------------------------------------------------------------
// DynamicVar
//----------------------------------------------------------------------------
DynamicVar::DynamicVar()    
{  
    m_fVar = 0.0f;  
    m_fTarget = 0.0f;
    m_fDelta = 0.0f;
    m_fTime = 0.0f;
    m_fTimeTarget = 0.0f;

}

void DynamicVar::setVar(float fVal, float fTime)
{
    m_fTarget = fVal;
    m_fDelta = (m_fTarget - m_fVar) / (fTime * 60.0f);
    m_fTime = 0.0f;
    m_fTimeTarget = fTime;
}

void DynamicVar::update()
{
    m_fTime += (1.0f / 60.0f);
    if(m_fTarget == m_fVar)
        return;
    m_fVar += m_fDelta;
    if(fabsf(m_fTarget - m_fVar) <= fabsf(m_fDelta))
        m_fVar = m_fTarget;
}




//----------------------------------------------------------------------------
// EnvelopeVar
//----------------------------------------------------------------------------


void EnvelopeVar::clear()
{
    m_Envelope.clear();
    m_Level = 0.0f;
    m_eState = ENV_FLOOR;
}

void EnvelopeVar::setVar(const SoundEnvelope& env)
{
    m_Envelope = env;
    m_eState = ENV_INITIAL;
    m_Level.setVar(m_Level, m_Envelope.m_fInitialTime);
}

void EnvelopeVar::update()
{
    m_Level.update();
    if(m_Level.hasReachedTarget())
    {
        switch(m_eState)
        {
        case ENV_FLOOR:
            break;
        case ENV_INITIAL:
            m_eState = ENV_ATTACK;
            m_Level.setVar(m_Envelope.m_fAttackLevel, m_Envelope.m_fAttackTime);
            break;
        case ENV_ATTACK:
            m_eState = ENV_DECAY;
            m_Level.setVar(m_Envelope.m_fDecayLevel, m_Envelope.m_fDecayTime);
            break;
        case ENV_DECAY:
            m_eState = ENV_SUSTAIN;
            m_Level.setVar(m_Envelope.m_fSustainLevel, m_Envelope.m_fSustainTime);
            break;
        case ENV_SUSTAIN:
            m_eState = ENV_RELEASE;
            m_Level.setVar(m_Envelope.m_fReleaseLevel, m_Envelope.m_fReleaseTime);
            break;
        case ENV_RELEASE:
            m_eState = ENV_FLOOR;
            break;
        };
    }
}





//----------------------------------------------------------------------------
// AudioManager
//----------------------------------------------------------------------------
AudioManager::AudioManager() 
{ 
    clear(); 
    init();
}


AudioManager::~AudioManager() 
{
    SoundDataMap::iterator sitr_map;
    for(sitr_map = m_SoundDataMap.begin(); sitr_map != m_SoundDataMap.end(); ++sitr_map)
    {
        SoundDataVector* pVector = sitr_map->second;
        SoundDataVector::iterator itr_vec;
        for(itr_vec = pVector->begin(); itr_vec != pVector->end(); ++itr_vec)
            delete *itr_vec;
        delete pVector;
    }
    MusicDataMap::iterator mitr_map;
    for(mitr_map = m_MusicDataMap.begin(); mitr_map != m_MusicDataMap.end(); ++mitr_map)
    {
        MusicDataVector* pVector = mitr_map->second;
        MusicDataVector::iterator itr_vec;
        for(itr_vec = pVector->begin(); itr_vec != pVector->end(); ++itr_vec)
            delete *itr_vec;
        delete pVector;
    }
    clear();
}


void AudioManager::clear()
{
    // Commentary stuff
    m_pAnnouncer = 0;
    m_SoundDataMap.clear();
    while(!m_PlayByPlayQueue.empty())
        m_PlayByPlayQueue.pop();
    while(!m_ColorQueue.empty())
        m_ColorQueue.pop();
    m_eCommentaryPlaying = NO_COMMENTARY_PLAYING;
    m_eCommentaryPlayState = COMMENTARY_NONE;

    // Crowd stuff
    for(int i = 0; i < NUM_CROWD_SOUNDS; i++)
        m_CrowdSound[i].clear();
    m_pCrowdOhh = 0;

    m_MusicDataMap.clear();
    m_eMusicState = MUSIC_NONE;
    m_MusicVolume = -100.0f;
    m_sCurrentMusic.erase();
    m_iCurrentTick = 0;
    m_iNextCommentaryUpdate = 0;
    m_sStadiumPath.clear();
    m_pAtBatLoopLeft = 0;
    m_pAtBatLoopRight = 0;
    m_AtBatLoopVolume = AT_BAT_LOOP_ATTENUATION;
    m_pStadiumLoopLeft = 0;
    m_pStadiumLoopRight = 0;
    m_StadiumLoopVolume = STADIUM_LOOP_ATTENUATION;
    m_bPauseCommentary = false;
    m_bForceMinimalDelay = false;
    m_bCrowdPaused = false;
}


void AudioManager::init()
{
    FN("AudioManager::init()");
    twig::TableResource * pRes =
        findResource<TableResource> ("config/sounds");
    assert(pRes);
    int iLoop = 0;
    if(pRes)
        iLoop = pRes->numRows();
    int i;
    // Make a resource name
    std::string sRawName;
    FString<256> sName;

    // Get the sound
    for(i = 0; i < iLoop; ++i)
    {
        SoundData* pSoundData = new SoundData;
        SoundDataVector* pVector = 0;
        SoundDataMap::iterator itr = m_SoundDataMap.find(pRes->getString(i, 0));
        if(itr == m_SoundDataMap.end())
        {
            pVector = new SoundDataVector;
            m_SoundDataMap.insert(make_pair(std::string(pRes->getString(i, 0)), pVector));
        }
        else
        {
            pVector = itr->second;
        }

        sRawName = pRes->getString(i, 1);
        std::string::size_type si = sRawName.find("|s");
#ifdef PS2
        sRawName.replace(si, 2, "_ps");
#else
        sRawName.replace(si, 2, "_gc");
#endif
        sName.printf("sounds/%s", sRawName.c_str());
        pSoundData->m_pResource = findResource<twig::SoundDataResource>(sName.c_str());
        if(!pSoundData->m_pResource)
        {
            LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load sound resource %s\n", pRes->getString(i, 1));
            continue;
        }

        // Get the delay and pitch variance from the table
        pSoundData->m_iDelay = static_cast<int>(pRes->getNumber(i, "Delay"));
        pSoundData->m_fPitch = pRes->getNumber(i, "Pitch");

        LOG(LOG_AUDIO, "AudioMgr: Associating resource %s with ID %s\n", sName.c_str(), pRes->getString(i, 0));

        pVector->push_back(pSoundData);
    }

    // Get all the music resources
    pRes = findResource<TableResource> ("config/music");
    assert(pRes);
    iLoop = 0;
    if(pRes)
        iLoop = pRes->numRows();
    iLoop = pRes->numRows();

    // Get the sound
    for(i = 0; i < iLoop; ++i)
    {
        MusicData* pMusicData = new MusicData;
        MusicDataVector* pVector = 0;
        MusicDataMap::iterator itr = m_MusicDataMap.find(pRes->getString(i, 0));
        if(itr == m_MusicDataMap.end())
        {
            pVector = new MusicDataVector;
            m_MusicDataMap.insert(make_pair(std::string(pRes->getString(i, 0)), pVector));
        }
        else
        {
            pVector = itr->second;
        }

        sprintf(pMusicData->szResource, "streams/%s", pRes->getString(i, 1));

        LOG(LOG_AUDIO, "AudioMgr: Associating resource %s with ID %s\n", pRes->getString(i, 1), pRes->getString(i, 0));

        pVector->push_back(pMusicData);
    }

    // Set the initial volume
    g_TitleOptions.setSoundEffectsVolume(g_TitleOptions.getSoundEffectsVolume());
    g_TitleOptions.setMusicVolume(g_TitleOptions.getMusicVolume());
}




void AudioManager::pauseCommentary()
{
    FN("AudioManager::pauseCommentary()");
    stopCommentary();
    m_bPauseCommentary = true;
}


void AudioManager::resumeCommentary()
{
    FN("AudioManager::resumeCommentary()");
    m_bPauseCommentary = false;
}


// Load stadium-specific audio resources
void AudioManager::loadStadiumAudioResources(mud::ResourceFileSet* pResSet)
{
    FN("AudioManager::loadStadiumAudioResources()");

    // Load the audio resource
    PathString sSoundSet;
    sSoundSet = "sounds/levels/";
    sSoundSet += g_BaseballGlobals.initGame.getStadiumName();
    sSoundSet += ".res";
    pResSet->insert(sSoundSet);

    // Define the path to the stadium-specific resources
#ifdef PS2
    m_sStadiumPath = "sounds/levels_ps/";
#else
    m_sStadiumPath = "sounds/levels_gc/";
#endif
    m_sStadiumPath += g_BaseballGlobals.initGame.getStadiumName();
}
    

void AudioManager::playStadiumMusicLoop()
{
    FN("AudioManager::playStadiumMusicLoop()");

    if(m_pStadiumLoopLeft)
    {
        m_pStadiumLoopLeft->stop();
        m_pStadiumLoopLeft->release();
    }
    if(m_pStadiumLoopRight)
    {
        m_pStadiumLoopRight->stop();
        m_pStadiumLoopRight->release();
    }

    FString<256> sName;
    twig::SoundDataResource* pRsrc;

    // Get the stadium music loop resource
    sName.printf("%s/music_loop_01", m_sStadiumPath.c_str());
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load stadium music loop.");
        return;
    }

    m_pStadiumLoopLeft = Sound::make(pRsrc, MIXCHAN_MUSIC | SND_LOOPING);
    m_pStadiumLoopRight = Sound::make(pRsrc, MIXCHAN_MUSIC | SND_LOOPING);
    m_StadiumLoopVolume = STADIUM_LOOP_ATTENUATION;
    m_pStadiumLoopLeft->setVolume(m_StadiumLoopVolume);
    m_pStadiumLoopRight->setVolume(m_StadiumLoopVolume + MUSIC_LOOP_RIGHT_ATTENUATION);
    m_pStadiumLoopLeft->setPan(-0.5f);
    m_pStadiumLoopRight->setPan(0.5f);
    m_pStadiumLoopLeft->play(1.0f);
    m_pStadiumLoopRight->play(MUSIC_LOOP_DELAY + 1.0f);

}


void AudioManager::stopStadiumMusicLoop()
{
    FN("AudioManager::stopStadiumMusicLoop()");
    m_StadiumLoopVolume.setVar(-100.0f, 5.0f);

}


void AudioManager::playAtBatMusicLoop()
{
    FN("AudioManager::playAtBatMusicLoop()");

    if(m_pAtBatLoopLeft)
    {
        m_pAtBatLoopLeft->stop();
        m_pAtBatLoopLeft->release();
    }
    if(m_pAtBatLoopRight)
    {
        m_pAtBatLoopRight->stop();
        m_pAtBatLoopRight->release();
    }

    FString<256> sName;
    twig::SoundDataResource* pRsrc;

    // Get ambient sound 1
    int iRand = irand(4) + 1;
    sName.printf("%s/at_bat_loop_%.2d", m_sStadiumPath.c_str(), iRand);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load stadium at bat loop.");
        return;
    }
    m_pAtBatLoopLeft = Sound::make(pRsrc, MIXCHAN_MUSIC | SND_LOOPING);
    m_pAtBatLoopRight = Sound::make(pRsrc, MIXCHAN_MUSIC | SND_LOOPING);
    m_AtBatLoopVolume = AT_BAT_LOOP_ATTENUATION;
    m_pAtBatLoopLeft->setPan(-0.5f);
    m_pAtBatLoopRight->setPan(0.5f);
    m_pAtBatLoopLeft->setVolume(m_AtBatLoopVolume);
    m_pAtBatLoopRight->setVolume(m_AtBatLoopVolume + MUSIC_LOOP_RIGHT_ATTENUATION);
    m_pAtBatLoopLeft->play();
    m_pAtBatLoopRight->play(MUSIC_LOOP_DELAY);

}


void AudioManager::stopAtBatMusicLoop()
{
    FN("AudioManager::stopAtBatMusicLoop()");
    m_AtBatLoopVolume.setVar(-100.0f, 7.0f);
}


void AudioManager::playPlayByPlay(const char* pszResID, unsigned int nPreDelay, 
    unsigned int nPostDelay, unsigned int nExpiration, bool* pUsed)
{
    FN("AudioManager::playPlayByPlay()");

    // Exit if commentary is disabled
    if(!g_TitleOptions.getCommentaryEnable())
        return;

    // Also exit if we're in the middle of playing music
    if(isMusicPlaying())
        return;

    LOG(LOG_AUDIO, "AudioMgr: Queuing resource play-by-play ID %s\n", pszResID);

    // Place the commentary into the play queue
    CommentaryFrame frame;
    if(!pszResID)
        return;
    ASSERT(strlen(pszResID) < MAX_RESOURCE_NAME_LENGTH-1);
    strcpy(frame.szResource, pszResID);
    frame.nPreDelay = nPreDelay;
    frame.nPostDelay = nPostDelay;
    frame.nExpiration = nExpiration;
    frame.pUsed = pUsed;
    // An expiration of zero means the commentary will only expire after a fixed
    // time of one minute.
    if(nExpiration)
        frame.nExpiration += m_iCurrentTick;
    else
        frame.nExpiration = m_iCurrentTick + 3600;

    // Push the commentary data into the playback queue
    m_PlayByPlayQueue.push(frame);
}

void AudioManager::playColor(const char* pszResID, unsigned int nPreDelay, 
    unsigned int nPostDelay, unsigned int nExpiration, bool* pUsed)
{
    FN("AudioManager::playColor()");

    // Exit if commentary is disabled
    if(!g_TitleOptions.getCommentaryEnable())
        return;

    // Also exit if we're in the middle of playing music
    if(isMusicPlaying())
        return;

    LOG(LOG_AUDIO, "AudioMgr: Queuing resource color ID %s\n", pszResID);

    // Place the commentary into the play queue
    CommentaryFrame frame;
    if(!pszResID)
        return;
    ASSERT(strlen(pszResID) < MAX_RESOURCE_NAME_LENGTH-1);
    strcpy(frame.szResource, pszResID);
    frame.nPreDelay = nPreDelay;
    frame.nPostDelay = nPostDelay;
    frame.nExpiration = nExpiration;
    frame.pUsed = pUsed;
    // An expiration of zero means the commentary will never expire
    if(nExpiration)
        frame.nExpiration += m_iCurrentTick;

    // Push the color data into the playback queue
    m_ColorQueue.push(frame);
}

void AudioManager::stopCommentary()
{
    FN("AudioManager::stopCommentary()");
    if(!isCommentaryPlaying())
        return;

    if(m_pAnnouncer && m_pAnnouncer->isPlaying())
    {
        m_pAnnouncer->release();
        m_pAnnouncer = 0;
    }
    else
    {
        streaming::stop();
    }
}

// Play a music stream
void AudioManager::playMusic(const char* pszResID, bool bLooping)
{
    FN("AudioManager::playMusic()");

    // Find the sound data resource for a given string ID
    MusicDataMap::iterator itr = m_MusicDataMap.find(pszResID);
    if(itr == m_MusicDataMap.end())
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to play music ID %s\n", pszResID);
        return;
    }
    MusicDataVector* pVector = itr->second;
    if(!pVector || !pVector->size())
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to play music ID %s\n", pszResID);
        return;
    }

    // Store the current resource ID for looping
    m_sCurrentMusic = pszResID;

    // Play the sound on a free channel
    int iIndex = irand(pVector->size());

    LOG(LOG_AUDIO, "AudioMgr: playing music resource %s (index %d (zero based) of %d total songs)\n", 
        pVector->at(iIndex)->szResource, iIndex, pVector->size());

    // Stop any commentary from playing.  We'll consider commentary and music 
    // mutually exclusive since the both primarily use the streaming channel.  Technically,
    // commentary or music could be loaded in memory, especially on the GameCube with its 
    // 16MB of audio memory, but it simplifies matters to consider just one case.
    if(isCommentaryPlaying())
        stopCommentary();
    else if(isMusicPlaying())
        stopMusic();

    // start the streaming channel and queue the music!!!
    streaming::play();
    streaming::setVolume(g_TitleOptions.getMixedMusicVolume(), g_TitleOptions.getMixedMusicVolume());
    streaming::queue(pVector->at(iIndex)->szResource);

    if(bLooping)
        m_eMusicState = MUSIC_PLAYING_LOOPED;
    else
        m_eMusicState = MUSIC_PLAYING;

}


void AudioManager::stopMusic(float fTime)
{
    FN("AudioManager::stopMusic()");
    if(m_eMusicState != MUSIC_NONE)
    {
        streaming::stop(fTime);
        m_eMusicState = MUSIC_NONE;
    }
}


bool AudioManager::isMusicPlaying()
{
    if(m_eMusicState == MUSIC_NONE)
        return false;
    return true;
}

void AudioManager::playSound(const char* pszResID)
{
    FN("AudioManager::playSound()");

    // Find the sound data resource for a given string ID
    SoundDataMap::iterator itr = m_SoundDataMap.find(pszResID);
    if(itr == m_SoundDataMap.end())
    {
        LOG(LOG_AUDIO, "AudioMgr: WARNING!!!: Unable to play sound ID %s\n", pszResID);
        return;
    }
    SoundDataVector* pVector = itr->second;
    if(!pVector || !pVector->size())
    {
        LOG(LOG_AUDIO, "AudioMgr: WARNING!!!: Unable to play sound ID %s\n", pszResID);
        return;
    }

    // Play the sound on a free channel
    int iIndex = irand(pVector->size());
    SoundData data;
    memcpy(&data, pVector->at(iIndex), sizeof(SoundData));
    if(data.m_fPitch != 0.0f)
        data.m_fPitch = rrand(-data.m_fPitch, data.m_fPitch);
    m_SoundsToPlay.push_back(data);
    
}


void AudioManager::loadCrowdResources(ResourceFileSet* pResSet)
{
    FN("AudioManager::loadCrowdResources()");

    PathString sSoundSet;
    // Load the crowd resource set
    sSoundSet = "sounds/crowd.res";
    pResSet->insert(sSoundSet);
}


void AudioManager::startCrowd()
{
    FN("AudioManager::startCrowd()");

    m_bCrowdPaused = false;

    FString<256> sName;
    twig::SoundDataResource* pRsrc;

    // Get ambient sound 1
    sName.printf("%s/amb_1", g_szCrowdBase);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load ambient crowd sound 1.");
        return;
    }
    m_CrowdSound[CROWD_AMBIENT].m_pLeftChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);

    // Get ambient sound 2
    sName.printf("%s/amb_2", g_szCrowdBase);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load ambient crowd sound 2.");
        return;
    }
    m_CrowdSound[CROWD_AMBIENT].m_pRightChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);

    // Crowd cheer sounds
    sName.printf("%s/big_cheer_lp", g_szCrowdBase);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load crowd cheer sound.");
        return;
    }
    m_CrowdSound[CROWD_CHEER].m_pLeftChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);
    m_CrowdSound[CROWD_CHEER].m_pRightChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);

    // Crowd clapping sounds
    sName.printf("%s/claps_small_lp", g_szCrowdBase);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load crowd clap sound.");
        return;
    }
    m_CrowdSound[CROWD_CLAP].m_pLeftChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);
    m_CrowdSound[CROWD_CLAP].m_pRightChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);

    // Crowd chanting sounds
    sName.printf("%s/med_rythm_chant_lp", g_szCrowdBase);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load crowd boo sound.");
        return;
    }
    m_CrowdSound[CROWD_CHANT].m_pLeftChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);
    m_CrowdSound[CROWD_CHANT].m_pRightChannel = 0;
    m_CrowdSound[CROWD_CHANT].m_bSingleChannel = true;

    // Crowd booing sounds
    sName.printf("%s/boo_lp", g_szCrowdBase);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load crowd boo sound.");
        return;
    }
    m_CrowdSound[CROWD_BOO].m_pLeftChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);
    m_CrowdSound[CROWD_BOO].m_pRightChannel = Sound::make(pRsrc, MIXCHAN_SFX | SND_LOOPING);


    // Load the crowd "ohh" sound
    sName.printf("%s/ow", g_szCrowdBase);
    pRsrc = findResource<twig::SoundDataResource>(sName.c_str());
    if(!pRsrc)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to find crowd sound \"ohh\".");
        return;
    }
    m_pCrowdOhh = Sound::make(pRsrc, MIXCHAN_SFX);
    if(!m_pCrowdOhh)
    {
        LOG(LOG_ERROR, "AudioMgr: WARNING!!!: Unable to load crowd sound \"ohh\".");
        return;
    }

    for(int i = 0; i < NUM_CROWD_SOUNDS; i++)
    {
        m_CrowdSound[i].m_Envelope.clear();
        m_CrowdSound[i].m_LeftPan = -0.75f;
        m_CrowdSound[i].m_Pitch = 0.0f;
        m_CrowdSound[i].m_pLeftChannel->setVolume(linearToLog(m_CrowdSound[i].m_Envelope));
        m_CrowdSound[i].m_pLeftChannel->setPan(m_CrowdSound[i].m_LeftPan);
        m_CrowdSound[i].m_pLeftChannel->adjustPitch(m_CrowdSound[i].m_Pitch);
        m_CrowdSound[i].m_pLeftChannel->play();
        if(m_CrowdSound[i].m_bSingleChannel)
            continue;
        m_CrowdSound[i].m_RightPan = 0.75f;
        m_CrowdSound[i].m_pRightChannel->setVolume(linearToLog(m_CrowdSound[i].m_Envelope));
        m_CrowdSound[i].m_pRightChannel->setPan(m_CrowdSound[i].m_RightPan);
        m_CrowdSound[i].m_pRightChannel->adjustPitch(m_CrowdSound[i].m_Pitch);
        m_CrowdSound[i].m_pRightChannel->play(0.5f);
    }
}


void AudioManager::stopCrowd()
{
    FN("AudioManager::stopCrowd()");

    m_bCrowdPaused = false;

    for(int i = 0; i < NUM_CROWD_SOUNDS; i++)
    {
        if(m_CrowdSound[i].m_pLeftChannel)
        {
            m_CrowdSound[i].m_pLeftChannel->stop();
            m_CrowdSound[i].m_pLeftChannel->release();
            m_CrowdSound[i].m_pLeftChannel = 0;
        }
        if(m_CrowdSound[i].m_pRightChannel)
        {
            m_CrowdSound[i].m_pRightChannel->stop();
            m_CrowdSound[i].m_pRightChannel->release();
            m_CrowdSound[i].m_pRightChannel = 0;
        }
    }

    if(m_pCrowdOhh)
    {
        if(m_pCrowdOhh->isPlaying())
            m_pCrowdOhh->stop();
        m_pCrowdOhh->release();
        m_pCrowdOhh = 0;
    }
}


void AudioManager::pauseCrowd()
{
    FN("AudioManager::pauseCrowd()");

    m_bCrowdPaused = true;

    for(int i = 0; i < NUM_CROWD_SOUNDS; i++)
    {
        if(m_CrowdSound[i].m_pLeftChannel)
        {
            m_CrowdSound[i].m_pLeftChannel->stop();
        }
        if(m_CrowdSound[i].m_pRightChannel)
        {
            m_CrowdSound[i].m_pRightChannel->stop();
        }
    }

    if(m_pCrowdOhh)
    {
        if(m_pCrowdOhh->isPlaying())
            m_pCrowdOhh->stop();
    }

}


void AudioManager::resumeCrowd()
{
    FN("AudioManager::resumeCrowd()");

    m_bCrowdPaused = false;

    for(int i = 0; i < NUM_CROWD_SOUNDS; i++)
    {
        m_CrowdSound[i].m_pLeftChannel->play();
        if(m_CrowdSound[i].m_bSingleChannel)
            continue;
        m_CrowdSound[i].m_pRightChannel->play(0.5f);
    }
}


void AudioManager::playAmbience(const SoundEnvelope& env)
{
    FN("AudioManager::playAmbience()");
    m_CrowdSound[CROWD_AMBIENT].m_Envelope.setVar(env);
}

void AudioManager::playCheer(const SoundEnvelope& env)
{
    FN("AudioManager::playCheer()");
    m_CrowdSound[CROWD_CHEER].m_Envelope.setVar(env);
}

void AudioManager::playClapping(const SoundEnvelope& env)
{
    FN("AudioManager::playClapping()");
    m_CrowdSound[CROWD_CLAP].m_Envelope.setVar(env);
}

void AudioManager::playChant(const SoundEnvelope& env)
{
    FN("AudioManager::playChant()");
    m_CrowdSound[CROWD_CHANT].m_Envelope.setVar(env);
}

void AudioManager::playBoo(const SoundEnvelope& env)
{
    FN("AudioManager::playBooChant()");
    m_CrowdSound[CROWD_BOO].m_Envelope.setVar(env);
}

void AudioManager::playOhh(float fLevel)
{
    FN("AudioManager::playOhh()");
    if(!m_pCrowdOhh)
        return;
    m_pCrowdOhh->setVolume(linearToLog(fLevel));
    m_pCrowdOhh->play();
}



// Is any commentary currently playing?
bool AudioManager::isCommentaryPlaying()
{
    if((m_pAnnouncer && m_pAnnouncer->isPlaying()) || 
        (streaming::queueLength() && !isMusicPlaying()))
        return true;
    return false;
}


void AudioManager::clearPlayByPlayQueue()
{
    FN("AudioManager::clearPlayByPlayQueue()");
    if(m_eCommentaryPlaying == PLAY_BY_PLAY_PLAYING)
    {
        if(m_eCommentaryPlayState == COMMENTARY_CONTINUE_PLAY)
        {
            // We don't want to clear the last element in the commentary queue, 
            // since this is what's currently playing
            while(m_PlayByPlayQueue.size() > 1)
                m_PlayByPlayQueue.pop();
            m_bForceMinimalDelay = true;
        }
        else
        {
            // Clear out all elements in the queue since this type is not playing
            while(m_PlayByPlayQueue.size() > 0)
                m_PlayByPlayQueue.pop();
            // Reset the play state variables so we're sure to update properly
            m_eCommentaryPlayState = COMMENTARY_NONE;
            m_iNextCommentaryUpdate = m_iCurrentTick;
        }
    }
    else
    {
        // Clear out all elements in the queue since this type is not playing
        while(m_PlayByPlayQueue.size() > 0)
            m_PlayByPlayQueue.pop();
    }
}

void AudioManager::clearColorQueue()
{
    FN("AudioManager::clearColorQueue()");
    if(m_eCommentaryPlaying == COLOR_PLAYING)
    {
        if(m_eCommentaryPlayState == COMMENTARY_CONTINUE_PLAY)
        {
            // We don't want to clear the last element in the commentary queue, 
            // since this is what's currently playing
            while(m_ColorQueue.size() > 1)
                m_ColorQueue.pop();
            m_bForceMinimalDelay = true;
        }
        else
        {
            // Clear out all elements in the queue since this type is not playing
            while(m_ColorQueue.size() > 0)
                m_ColorQueue.pop();
            // Reset the play state variables so we're sure to update properly
            m_eCommentaryPlayState = COMMENTARY_NONE;
            m_iNextCommentaryUpdate = m_iCurrentTick;
        }
    }
    else
    {
        // Clear out all elements in the queue since this type is not playing
        while(m_ColorQueue.size() > 0)
            m_ColorQueue.pop();
    }

}


void AudioManager::updateCrowdSounds()
{
    FN("AudioManager::updateCrowdSounds()");

    const float MIN_TIME = 7.0f;
    const float MAX_TIME = 10.0f;

    const float MIN_PITCH = -0.1f;
    const float MAX_PITCH = 0.1f;

    if(m_bCrowdPaused)
        return;

    for(int i = 0; i < NUM_CROWD_SOUNDS; i++)
    {
        //safety check...
        if(!m_CrowdSound[i].m_pLeftChannel)
            continue;
        
        // Update the global volume envelope
        m_CrowdSound[i].m_Envelope.update();

        // Update the left channel
        m_CrowdSound[i].m_LeftPan.update();
        m_CrowdSound[i].m_Pitch.update();
        m_CrowdSound[i].m_pLeftChannel->setVolume(linearToLog(m_CrowdSound[i].m_Envelope));
        m_CrowdSound[i].m_pLeftChannel->setPan(m_CrowdSound[i].m_LeftPan);
        m_CrowdSound[i].m_pLeftChannel->adjustPitch(m_CrowdSound[i].m_Pitch);
        if(m_CrowdSound[i].m_Pitch.hasReachedTarget())
            m_CrowdSound[i].m_Pitch.setVar(rrand(MIN_PITCH, MAX_PITCH), rrand(MIN_TIME, MAX_TIME));
        if(m_CrowdSound[i].m_LeftPan.hasReachedTarget())
        {
            if(m_CrowdSound[i].m_bSingleChannel)
                m_CrowdSound[i].m_LeftPan.setVar(rrand(-0.33, 0.33f), rrand(MIN_TIME, MAX_TIME));
            else
                m_CrowdSound[i].m_LeftPan.setVar(rrand(-1.0, 0.0f), rrand(MIN_TIME, MAX_TIME));
        }

        // Update the right channel only if allowed
        if(m_CrowdSound[i].m_bSingleChannel)
            continue;
      
        //safety check...
        if(!m_CrowdSound[i].m_pRightChannel)
            continue;

        m_CrowdSound[i].m_RightPan.update();
        m_CrowdSound[i].m_pRightChannel->setVolume(linearToLog(m_CrowdSound[i].m_Envelope));
        m_CrowdSound[i].m_pRightChannel->setPan(m_CrowdSound[i].m_RightPan);
        m_CrowdSound[i].m_pRightChannel->adjustPitch(m_CrowdSound[i].m_Pitch);
        if(m_CrowdSound[i].m_RightPan.hasReachedTarget())
            m_CrowdSound[i].m_RightPan.setVar(rrand(0.0, 1.0f), rrand(MIN_TIME, MAX_TIME));
    }
}


void AudioManager::updateMusic()
{
    FN("AudioManager::updateMusic()");
    // In case we need to do any sort of dynamic volume control with
    // the music, we'll leave this here for now.
    switch(m_eMusicState)
    {
    case MUSIC_NONE:
        break;
    case MUSIC_PLAYING:
        if(!streaming::queueLength())
        {
            LOG(LOG_AUDIO, "Music track %s has ended", m_sCurrentMusic.c_str());
        }
        break;
    case MUSIC_PLAYING_LOOPED:
        if(!streaming::queueLength())
        {
            LOG(LOG_AUDIO, "Restarting music track %s", m_sCurrentMusic.c_str());
            playMusic(m_sCurrentMusic.c_str());
        }
        break;
    };
}



void AudioManager::updateCommentary()
{
    FN("AudioManager::updateCommentary()");
    // Only update if the appropriate time has elapsed
    if((m_iCurrentTick < m_iNextCommentaryUpdate) || m_bPauseCommentary)
        return;

    switch(m_eCommentaryPlayState)
    {
    case COMMENTARY_NONE:
        {
            if(!m_PlayByPlayQueue.empty())
            {
                m_eCommentaryPlaying = PLAY_BY_PLAY_PLAYING;
                m_eCommentaryPlayState = COMMENTARY_PRE_DELAY;
            }
            else if(!m_ColorQueue.empty())
            {
                m_eCommentaryPlaying = COLOR_PLAYING;
                m_eCommentaryPlayState = COMMENTARY_PRE_DELAY;
            }
            m_iNextCommentaryUpdate = m_iCurrentTick;
        }
        break;

    case COMMENTARY_PRE_DELAY:
        {
            CommentaryFrame frame;
            if(m_eCommentaryPlaying == PLAY_BY_PLAY_PLAYING)
            {
                ASSERT(m_PlayByPlayQueue.size());
                frame = m_PlayByPlayQueue.front();
            }
            else
            {
                ASSERT(m_ColorQueue.size());
                frame = m_ColorQueue.front();
            }
            m_eCommentaryPlayState = COMMENTARY_START_PLAY;
            m_iNextCommentaryUpdate = m_iCurrentTick + frame.nPreDelay;
            LOG(LOG_AUDIO, "Delaying initial commentary resource %s for %d ticks", frame.szResource, frame.nPreDelay);
        }
        break;

    case COMMENTARY_START_PLAY:
        {
            // Get the current sound data
            CommentaryFrame frame;
            if(m_eCommentaryPlaying == PLAY_BY_PLAY_PLAYING)
            {
                ASSERT(m_PlayByPlayQueue.size());
                frame = m_PlayByPlayQueue.front();
            }
            else
            {
                ASSERT(m_ColorQueue.size());
                frame = m_ColorQueue.front();
            }
            
            // Check to see if the commentary has expired.  This will help to prevent
            // things from being said if it was too long since the action occured
            if((frame.nExpiration != 0) && (frame.nExpiration < m_iCurrentTick))
            {
                LOG(LOG_AUDIO, "Commentary %s has expired - moving on to next in line...", frame.szResource);
                
                // Since this resource actually WON'T be playing now, mark it as unused
                *frame.pUsed = false;

                // Pop the current frame off the queue
                if(m_eCommentaryPlaying == PLAY_BY_PLAY_PLAYING)
                {
                    ASSERT(m_PlayByPlayQueue.size());
                    m_PlayByPlayQueue.pop();
                }
                else
                {
                    ASSERT(m_ColorQueue.size());
                    m_ColorQueue.pop();
                }

                m_eCommentaryPlayState = COMMENTARY_NONE;
                break;
            }

            // Make a resource name
            FString<256> sName;
            sName.printf("sounds/annc_mem/%s", frame.szResource);

            // Get the sound resource
            twig::SoundDataResource* pRsrc =
                findResource<twig::SoundDataResource>(sName.c_str());

            // Check to see if the in-memory resource is there
            if(pRsrc)
            {
                // Play the sound resource on the designated channel
                m_pAnnouncer = Sound::play(pRsrc, MIXCHAN_SFX);
            }
            else
            {
                sName.printf("streams/%s", frame.szResource);
                // start the streaming channel if it's not already started
                //if(isMusicPlaying())
                //    stopMusic();
                streaming::setVolume(
                    g_TitleOptions.getMixedSoundEffectsVolume(), 
                    g_TitleOptions.getMixedSoundEffectsVolume());
                streaming::play();
                streaming::queue(sName.c_str());
            }
            m_eCommentaryPlayState = COMMENTARY_CONTINUE_PLAY;
            m_iNextCommentaryUpdate = m_iCurrentTick;
            LOG(LOG_AUDIO, "Playing commentary resource %s", sName.c_str());
        };
        break;

    case COMMENTARY_CONTINUE_PLAY:
        {
            if(!isCommentaryPlaying())
            {
                if(m_pAnnouncer && !m_pAnnouncer->isPlaying())
                {
                    m_pAnnouncer->release();
                    m_pAnnouncer = 0;
                }
                m_eCommentaryPlayState = COMMENTARY_POST_DELAY;
            }
            m_iNextCommentaryUpdate = m_iCurrentTick;
        }
        break;

    case COMMENTARY_POST_DELAY:
        {
            // Get the current sound data
            CommentaryFrame frame;
            if(m_eCommentaryPlaying == PLAY_BY_PLAY_PLAYING)
            {
                ASSERT(m_PlayByPlayQueue.size());
                frame = m_PlayByPlayQueue.front();
                m_PlayByPlayQueue.pop();
            }
            else
            {
                ASSERT(m_ColorQueue.size());
                frame = m_ColorQueue.front();
                m_ColorQueue.pop();
            }

            m_eCommentaryPlayState = COMMENTARY_NONE;
            if(m_bForceMinimalDelay)
            {
                if(frame.nPostDelay > 30)
                    m_iNextCommentaryUpdate = m_iCurrentTick + 30;
                else
                    m_iNextCommentaryUpdate = m_iCurrentTick + frame.nPostDelay;
                m_bForceMinimalDelay = false;
            }
            else
                m_iNextCommentaryUpdate = m_iCurrentTick + frame.nPostDelay;
            m_eCommentaryPlaying = NO_COMMENTARY_PLAYING;
            LOG(LOG_AUDIO, "Finished playing %s, delaying next commentary for %d ticks", frame.szResource, frame.nPostDelay);
        }
        break;

    default:
        assert(!"Invalid state in audio manager commentary system.");
    };
}


void AudioManager::updateSound()
{
    FN("AudioManager::updateSound()");

    // Check all playing sounds - release them when they're finished playing
    SoundVector::iterator itor;
    for(itor = m_SoundsPlaying.begin(); itor != m_SoundsPlaying.end();)
    {
        if(!(*itor)->isPlaying())
        {
            (*itor)->release();
            itor = m_SoundsPlaying.erase(itor);
        }
        else
            ++itor;
    }

    // Update all the delayed sounds, and play them when they're ready
    SoundDelayVector::iterator itr;
    for(itr = m_SoundsToPlay.begin(); itr != m_SoundsToPlay.end();)
    {
        (*itr).m_iDelay--;
        if((*itr).m_iDelay <= 0)
        {
            Sound* pSound = Sound::make((*itr).m_pResource, MIXCHAN_SFX);
            pSound->adjustPitch((*itr).m_fPitch);
            pSound->play();
            itr = m_SoundsToPlay.erase(itr);
            m_SoundsPlaying.push_back(pSound);
        }
        else
            ++itr;
    }

}


void AudioManager::updateStadiumMusic()
{
    FN("AudioManager::updateStadiumMusic()");

    if(m_pStadiumLoopLeft && m_pStadiumLoopRight)
    {
        m_StadiumLoopVolume.update();
        m_pStadiumLoopLeft->setVolume(m_StadiumLoopVolume);
        m_pStadiumLoopRight->setVolume(m_StadiumLoopVolume + MUSIC_LOOP_RIGHT_ATTENUATION);
        if(m_StadiumLoopVolume < -90.0f)
        {
            m_pStadiumLoopLeft->stop();
            m_pStadiumLoopRight->stop();
            m_pStadiumLoopLeft->release();
            m_pStadiumLoopRight->release();
            m_pStadiumLoopLeft = 0;
            m_pStadiumLoopRight = 0;
            m_StadiumLoopVolume = 0.0f;
        }
    }

    if(m_pAtBatLoopLeft && m_pAtBatLoopRight)
    {
        m_AtBatLoopVolume.update();
        m_pAtBatLoopLeft->setVolume(m_AtBatLoopVolume);
        m_pAtBatLoopRight->setVolume(m_AtBatLoopVolume + MUSIC_LOOP_RIGHT_ATTENUATION);
        if(m_AtBatLoopVolume < -90.0f)
        {
            m_pAtBatLoopLeft->stop();
            m_pAtBatLoopRight->stop();
            m_pAtBatLoopLeft->release();
            m_pAtBatLoopRight->release();
            m_pAtBatLoopLeft = 0;
            m_pAtBatLoopRight = 0;
            m_AtBatLoopVolume = 0.0f;
        }
    }
}



void AudioManager::update()
{
    FN("AudioManager::update()");
    m_iCurrentTick++;

    updateCommentary();
    updateSound();
    updateCrowdSounds();
    updateMusic();
    updateStadiumMusic();
}


