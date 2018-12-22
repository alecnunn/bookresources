/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// MusicSys.cpp

#include "StdAfx.h"
#include "math.h"
#include "MusicSys.h"
#include "InstrumentData.h"
#include "SequenceData.h"
#include "EventData.h"
#include "TextBoxes.h"

int MusicSequencerSystem_t::ms_anAudioTaper[128];

MusicSequencerSystemConfig_t::MusicSequencerSystemConfig_t()
{
	SetSequenceCount( 8 );
	SetTrackCount( 32 );
	SetVoiceArchitectureTypeCount( 2 );
	SetVoiceCount( 0, 32 );
	SetVoiceCount( 1, 32 );
}

MusicSequencerSystemConfig_t::~MusicSequencerSystemConfig_t()
{
}

void MusicSequencerSystemConfig_t::SetSequenceCount( int nSequenceCount )
{
	m_nSequenceCount = nSequenceCount;
}

int MusicSequencerSystemConfig_t::GetSequenceCount()
{
	return m_nSequenceCount;
}

void MusicSequencerSystemConfig_t::SetTrackCount( int nTrackCount )
{
	m_nTrackCount = nTrackCount;
}

int MusicSequencerSystemConfig_t::GetTrackCount()
{
	return m_nTrackCount;
}

void MusicSequencerSystemConfig_t::SetVoiceArchitectureTypeCount( int nVoiceArchitectureTypeCount )
{
	m_VoiceCountVector.resize( nVoiceArchitectureTypeCount );
	m_nVoiceArchitectureTypeCount = nVoiceArchitectureTypeCount;
}

int  MusicSequencerSystemConfig_t::GetVoiceArchitectureTypeCount()
{
	return m_nVoiceArchitectureTypeCount;
}

void MusicSequencerSystemConfig_t::SetVoiceCount( int nVoiceArchitectureType, int nVoiceCount )
{
	m_VoiceCountVector[nVoiceArchitectureType] = nVoiceCount;
}

int  MusicSequencerSystemConfig_t::GetVoiceCount( int nVoiceArchitectureType )
{
	return m_VoiceCountVector[nVoiceArchitectureType];
}

MusicSequencerSystem_t::MusicSequencerSystem_t()
{
	m_CSynMix = NULL;
}

MusicSequencerSystem_t::~MusicSequencerSystem_t()
{
}

#define TEST_INIT_ERROR(val,msg) \
	if(val < 0) \
	{ \
		TextBoxes_Output_Printf( OUTPUTMODE_COMMANDS, "\nERROR - %s - %s", CSyn_ErrorCodeToText(val), msg); \
	}

#define TEST_ERROR(val,msg) \
	if(val < 0) \
	{ \
		TextBoxes_Output_Printf( OUTPUTMODE_ERRORS, "ERROR - %s - %s\n", CSyn_ErrorCodeToText(val), msg); \
	}


bool MusicSequencerSystem_t::Init( MusicSequencerSystemConfig_t &Config )
{
	CSynErr result;
	int i, j;

	// create an audio taper table
	double minAmplitude = 0.05;
	ms_anAudioTaper[0] = 0;
	for( i=1; i<128; i++ )
	{
		double fVal = minAmplitude / ( exp( log(minAmplitude) * (i / 127.0) ) );
		ms_anAudioTaper[i] = ConvertDoubleToFixed( fVal );
	}

	// CSyn Init
	/* Create a context for the synthesis to occur. */
	m_CSynContext = CSyn_CreateContext();
	if( m_CSynContext == NULL )
	{
		TextBoxes_Output_Printf( OUTPUTMODE_ERRORS, "\nCould not create CSynContext");
		return false;
	}

	/* CSyn synthesis engine must be started before allocating unit generators. */
	result = CSyn_StartEngine( m_CSynContext, 0, 44100.0 );
	TEST_INIT_ERROR(result, "CSyn_StartEngine");
	if( result < 0 ) return false;
	
	/* Create a mix for all the voices. */
	result = CSynMix_Create( m_CSynContext, &m_CSynMix, 0 );
	TEST_INIT_ERROR(result, "CSynMix_Create");
	if( result < 0 ) return false;

	m_Config = Config;

	// allocate sequence resources
	int nSVSize = m_Config.GetSequenceCount();
	m_SequenceVector.resize( nSVSize );

	// assign sequence resources
	for( i=0; i<nSVSize; i++ )
	{
		Sequence_t *pSequence = &m_SequenceVector[i];
		pSequence->m_bActive = false;
		m_FreeSequencePtrList.push_back( pSequence );
	}

	// allocate track resources
	int nTVSize = m_Config.GetTrackCount();
	m_TrackVector.resize( nTVSize );

	// assign track resources
	for( i=0; i<nTVSize; i++ )
	{
		Track_t *pTrack = &m_TrackVector[i];
		pTrack->m_bActive = false;
		m_FreeTrackPtrList.push_back( pTrack );
	}

	// allocate voice architecture resources
	int nVVVSize = m_Config.GetVoiceArchitectureTypeCount();
	m_VoiceVectorVector.resize( nVVVSize );
	m_ActiveVoicePtrListVector.resize( nVVVSize );
	m_FreeVoicePtrListVector.resize( nVVVSize );

	for( i=0; i<nVVVSize; i++ )
	{
		// allocate voice resources
		int nVVSize = m_Config.GetVoiceCount( i );
		m_VoiceVectorVector[i].resize( nVVSize );

		// assign voice resources
		for( j=0; j<nVVSize; j++ )
		{
			Voice_t *pVoice = &m_VoiceVectorVector[i][j];
			pVoice->m_bActive = false;
			pVoice->m_nVoiceArchitectureType = i;
			result = CSynVoice_Load( m_CSynMix, (enum CSynCircuitType)i, NULL, &pVoice->m_SynVoice );
			TEST_ERROR(result, "CSynVoice_Load");
			m_FreeVoicePtrListVector[i].push_back( pVoice );
		}
	}

	return true;
}

double MusicSequencerSystem_t::GetCSynUsage()
{
	if( m_CSynContext )
	{
		return CSyn_GetUsage( m_CSynContext );
	}
	return 0.0;
}

void MusicSequencerSystem_t::CheckForFinishedVoices()
{
	VoicePtrListVector_t::iterator iVPLV;

	for( iVPLV = m_ActiveVoicePtrListVector.begin(); iVPLV != m_ActiveVoicePtrListVector.end(); ++iVPLV )
	{
		VoicePtrList_t &VoicePtrList = *iVPLV;

		VoicePtrList_t::iterator iVPL;
		iVPL = VoicePtrList.begin();
		while( iVPL != VoicePtrList.end() )
		{
			Voice_t *pVoice = *iVPL;
			++iVPL;
			if( pVoice->m_nReleaseTick && (pVoice->m_nReleaseTick < m_nWorkTick) )
			{
				pVoice->m_pOwner->m_VoicePtrList.remove( pVoice );
				FreeActiveVoice( pVoice );
			}
		}
	}
}

void MusicSequencerSystem_t::PerformTargetStateChanges()
{
	TrackPtrList_t::iterator iTPL;
	for( iTPL = m_ActiveTrackPtrList.begin(); iTPL != m_ActiveTrackPtrList.end(); ++iTPL )
	{
		Track_t *pTrack = *iTPL;

		if( pTrack->m_State.m_Volume != pTrack->m_TargetState.m_Volume )
		{
			if( pTrack->m_State.m_Volume > pTrack->m_TargetState.m_Volume )
			{
				pTrack->m_State.m_Volume -= pTrack->m_Interpolator.m_VolumeStep;
			} else {
				pTrack->m_State.m_Volume += pTrack->m_Interpolator.m_VolumeStep;
			}


			VoicePtrList_t::iterator iVPL;
			for( iVPL = pTrack->m_VoicePtrList.begin(); iVPL != pTrack->m_VoicePtrList.end(); ++iVPL )
			{
				Voice_t *pVoice = *iVPL;
				CSynErr result;
				//CSynFixedPoint amplitude = ms_anAudioTaper[ (pVoice->m_nVelocity * pTrack->m_State.m_Volume) / 128 ];
				CSynFixedPoint amplitude = (u64)ms_anAudioTaper[pVoice->m_nVelocity] * (u64)ms_anAudioTaper[pTrack->m_State.m_Volume] / (1 << 15);
				result = CSynVoice_SetAmplitude( pVoice->m_SynVoice, m_nWorkTick, amplitude );
				TEST_ERROR(result, "CSynVoice_SetAmplitude");
			}

		}
	}
}

void MusicSequencerSystem_t::DisplayStats()
{
	TextBoxes_Stats_Clear();

	TextBoxes_Stats_Printf("\nSequences Active:\n\n");

	SequenceVector_t::iterator iSV;
	for( iSV = m_SequenceVector.begin(); iSV != m_SequenceVector.end(); ++iSV )
	{
		Sequence_t &Sequence = *iSV;
		char ch = Sequence.m_bActive ? 'X' : '-';
		TextBoxes_Stats_Printf("%c", ch);
	}

	TextBoxes_Stats_Printf("\n\nTracks Active:\n\n");

	TrackVector_t::iterator iTV;
	for( iTV = m_TrackVector.begin(); iTV != m_TrackVector.end(); ++iTV )
	{
		Track_t &Track = *iTV;
		char ch = Track.m_bActive ? 'X' : '-';
		TextBoxes_Stats_Printf("%c", ch);
	}

	int nType = 0;
	VoiceVectorVector_t::iterator iVVV;
	for( iVVV = m_VoiceVectorVector.begin(); iVVV != m_VoiceVectorVector.end(); ++iVVV )
	{
		VoiceVector_t &VoiceVector = *iVVV;
		
		TextBoxes_Stats_Printf("\n\nType %d Voices Active:\n\n", nType);

		int nUnusedCount = 0;
		VoiceVector_t::iterator iVV;
		for( iVV = VoiceVector.begin(); iVV != VoiceVector.end(); ++iVV )
		{
			Voice_t &Voice = *iVV;
			if( Voice.m_bActive )
			{
				TextBoxes_Stats_Printf("X");
			} else {
				nUnusedCount++;
			}
		}
		while( nUnusedCount-- )
		{
			TextBoxes_Stats_Printf("-");
		}
		nType++;
	}
}

u32 MusicSequencerSystem_t::CalcTimeStep( u16 qpm, u16 ppq, u16 ups )
{
    u32 ppu;
    u32 temp;

    temp = (u32)qpm * (u32)ppq;
    if( temp < 0x10000 )
    {
        ppu = ((temp * 0x10000) / 60)
              / (u32)ups;
    } else {
        ppu = ((temp / 60) * 0x10000)
              / (u32)ups;
    }

    return(ppu);
}

bool MusicSequencerSystem_t::AudioFrame()
{
	// this is where we might protect against callback reentrance
	// this demo sequencer does not require this since it runs in the game thread

	// display status information for this frame
	DisplayStats();

	// this demo assumes that AudioFrame is called 30 times a second
	// CSyn_GetTickRate returns ticks per second
	// CSyn_GetTickCount returns our current tick time

	// we will call the sequencer update function every 4 ticks which is a rate
	// of 172.265625Hz (we tell our sequencer that our updates per second is 172)

	#define MUSICSYS_UPDATES_PER_SECOND 172

	// get the number of frames per second
	u32 nFramesPerSecond = 30;

	// get the current tick count
	u32 nCurTick = CSyn_GetTickCount( m_CSynContext );

	// calculate the number of tick in a frame
	u32 nFrameTicks = CSyn_GetTickRate( m_CSynContext ) / nFramesPerSecond;

	// calculate the tick at the end of this frame
	u32 nFrameEndTick = nCurTick + nFrameTicks;

	// set the number of frames ahead to schedule
	u32 nFramesAhead = 7;

	// calculate the last scheduled tick that will be due this frame
	u32 nScheduledEndTick = nFrameEndTick + (nFramesAhead * nFrameTicks);

	static int nFirstTime = 1;
	if( nFirstTime )
	{
		// calculate the first scheduled tick that will be due this frame
		u32 nScheduledStartTick = nCurTick + (nFramesAhead * nFrameTicks);

		m_nWorkTick = nScheduledStartTick;
		nFirstTime = 0;
	}

	// this is where we might begin a critical section
	// this demo sequencer does not require this since it runs in the game thread
	// OSys_BeginAudioCriticalSection();

    // for the number of update intervals required on this frame
    // {
	//     iterate over sequences
	//         perform per sequence operations

	//         iterate over tracks
	//             perform per track operations
    //
    //     send low-level commands for this time step
    //     move to next time step
    // }

    // determine the number of update intervals required
    // to deliver during this callback

	// while our work ticks is less than the nScheduledEndTick
	while( m_nWorkTick <= nScheduledEndTick )
	{
		m_nWorkTick += 4;
		Update();
	}

	// this is where we might end a critical section
	// this demo sequencer does not require this since it runs in the game thread
	// OSys_EndAudioCriticalSection();

	return true;
}

void NoteOff_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack )
{
	CSynErr result;
	int releaseTime;
	EventData_t *pEventData = *pTrack->m_iEDPL;
	int nKey = pEventData->GetEventDataByte( 0 );

	// find any voices with this key to send a note off to
	VoicePtrList_t::iterator iVPL;
	for( iVPL = pTrack->m_VoicePtrList.begin(); iVPL != pTrack->m_VoicePtrList.end(); ++iVPL )
	{
		Voice_t *pVoice = *iVPL;
		if( pVoice->m_nKey == nKey )
		{
			result = CSynVoice_NoteOff( pVoice->m_SynVoice, pMSS->m_nWorkTick, &releaseTime );
			TEST_ERROR(result, "CSynVoice_NoteOff");

			pVoice->m_nReleaseTick = releaseTime + pMSS->m_nWorkTick;
			TextBoxes_Output_Printf( OUTPUTMODE_RELEASES, "NoteOff Key = %3d relTime = %d tick = %d\n",
				nKey, releaseTime, pMSS->m_nWorkTick);
		}
	}
	TextBoxes_Output_Printf( OUTPUTMODE_EVENTS, "NoteOff Key = %3d  relTime = %d tick = %d\n",
		nKey, releaseTime, pMSS->m_nWorkTick);

}

void NoteOn_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack )
{
	CSynErr result;
	EventData_t *pEventData = *pTrack->m_iEDPL;
	int nKey = pEventData->GetEventDataByte( 0 );
	int nVelocity = pEventData->GetEventDataByte( 1 );
	if( nVelocity == 0 ) 
	{
		NoteOff_Function( pMSS, pTrack );
		return;
	}

	int nType = pTrack->m_pInstrumentData->GetType();
	Voice_t *pVoice = pMSS->MakeActiveVoice( nType );
	if( pVoice )
	{
		result = CSynVoice_SetSound( pVoice->m_SynVoice, pTrack->m_pInstrumentData->GetCSynSound() );
		TEST_ERROR(result, "CSynVoice_SetSound");

		int nPan = pTrack->m_State.m_Pan;
		int nCSynPanValue = ConvertIntegerToFixed(nPan - 64) / 64;
		result = CSynVoice_SetPan( pVoice->m_SynVoice, pMSS->m_nWorkTick, nCSynPanValue );
		TEST_ERROR(result, "CSynVoice_SetPan");
		TextBoxes_Output_Printf( OUTPUTMODE_PANS, "Pan = %3d as fixed = 0x%08x\n", nPan, nCSynPanValue);

		CSynFixedPoint frequency = CSynConvert_PitchToFrequency( ConvertIntegerToFixed( nKey ) );

		// this is a quick test that assumes the sample rate is 44100
		AudioSample *pAudioSample = pTrack->m_pInstrumentData->GetSampleInfoPtr();
		if( pAudioSample->baseNote < (nKey - 12) )
		{
			TextBoxes_Output_Printf( OUTPUTMODE_PITCHES, "Out of range Key = %3d baseNote = %3d Instrument = %3d tick = %d\n",
				nKey, pAudioSample->baseNote, pTrack->m_pInstrumentData->GetID(), pMSS->m_nWorkTick );
		} else {
			TextBoxes_Output_Printf( OUTPUTMODE_PITCHES, "Key = %3d Freq = 0x%08x tick = %d\n",
				nKey, frequency, pMSS->m_nWorkTick);
		}

		//CSynFixedPoint amplitude = pMSS->ms_anAudioTaper[ (nVelocity * pTrack->m_State.m_Volume) / 128 ];
		CSynFixedPoint amplitude = (u64)pMSS->ms_anAudioTaper[nVelocity] * (u64)pMSS->ms_anAudioTaper[pTrack->m_State.m_Volume] / (1 << 15);

		TextBoxes_Output_Printf( OUTPUTMODE_AMPLITUDES, "Vel = %3d Vol = %3d Amp = 0x%08x tick = %d\n",
			nVelocity, pTrack->m_State.m_Volume, amplitude, pMSS->m_nWorkTick );

		//result = CSynVoice_NoteOn( pVoice->m_SynVoice, pMSS->m_nWorkTick, frequency, amplitude );
		result = CSynVoice_NoteOn( pVoice->m_SynVoice, pMSS->m_nWorkTick, frequency, amplitude );
		TEST_ERROR(result, "CSynVoice_NoteOn");

		pVoice->m_pOwner = pTrack;
		pVoice->m_nKey = nKey;
		pVoice->m_nVelocity = nVelocity;
		pTrack->m_VoicePtrList.push_back( pVoice );

		TextBoxes_Output_Printf( OUTPUTMODE_VOICES, "Success Key = %3d Vel = %3d tick = %d\n",
			nKey, nVelocity, pMSS->m_nWorkTick);
	} else {
		TextBoxes_Output_Printf( OUTPUTMODE_VOICES, "Failed Key = %3d Vel = %3d tick = %d\n",
			nKey, nVelocity, pMSS->m_nWorkTick);
	}
	TextBoxes_Output_Printf( OUTPUTMODE_EVENTS, "NoteOn  Key = %3d Velocity = %3d tick = %d\n",
		nKey, nVelocity, pMSS->m_nWorkTick);
}

void SetPan_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack )
{
	EventData_t *pEventData = *pTrack->m_iEDPL;

	// we already know the controller is 10
	// u8 nController = pEventData->GetEventDataByte( 0 );

	// get the pan value
	u8 nValue = pEventData->GetEventDataByte( 1 );

	// we just set the pan for the next note
	// (a more sophisticated pan function would do a dynmaic update of this track's voices)
	pTrack->m_State.m_Pan = nValue;

	TextBoxes_Output_Printf( OUTPUTMODE_EVENTS, "Pan = %3d tick = %d\n",
		nValue, pMSS->m_nWorkTick);
}

void SetInstrument_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack )
{
	EventData_t *pEventData = *pTrack->m_iEDPL;

	int nInstrument = pEventData->GetEventDataByte( 0 );
	InstrumentData_t *pInstrumentData = pMSS->FindInstrumentData( nInstrument );
	if( pInstrumentData )
	{
		pTrack->m_pInstrumentData = pInstrumentData;
	}

	TextBoxes_Output_Printf( OUTPUTMODE_EVENTS, "SetInstrument = %3d tick = %d\n",
		nInstrument, pMSS->m_nWorkTick);
}

void TrackEnd_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack )
{
	// we just loop to the beginning by doing this
	pTrack->m_iEDPL = pTrack->m_iEDPLStart;

	TextBoxes_Output_Printf( OUTPUTMODE_EVENTS, "TrackEnd tick = %d\n", pMSS->m_nWorkTick);
}

MusicSequencerSystem_t::EventFuncPtr_t MusicSequencerSystem_t::ms_aFP[EVENT_TYPE_COUNT] = {
	NoteOff_Function,
	NoteOn_Function,
	SetPan_Function,
	SetInstrument_Function,
	TrackEnd_Function,
};

bool MusicSequencerSystem_t::Update()
{
	CheckForFinishedVoices();

	PerformTargetStateChanges();

	SequencePtrList_t::iterator iSPL;
	for( iSPL = m_ActiveSequencePtrList.begin(); iSPL != m_ActiveSequencePtrList.end(); ++iSPL )
	{
		Sequence_t *pSequence = *iSPL;
		pSequence->m_TimeElapsedFrac += pSequence->m_TimeStep;
		TimeInt_t TimeAdd = pSequence->m_TimeElapsedFrac >> 16;
		pSequence->m_TimeElapsedFrac &= 0xffff;

		TrackPtrList_t::iterator iTPL;
		for( iTPL = pSequence->m_TrackPtrList.begin(); iTPL != pSequence->m_TrackPtrList.end(); ++iTPL )
		{
			Track_t *pTrack = *iTPL;
			pTrack->m_TimeElapsedInt += TimeAdd;

			EventData_t *pEventData = *pTrack->m_iEDPL;

			while( pEventData->m_nEventTime <= pTrack->m_TimeElapsedInt )
			{
				pTrack->m_TimeElapsedInt -= pEventData->m_nEventTime;

				// execute the event if it is a valid command
				u32 nCommand = pEventData->GetRuntimeCommand();
				if( nCommand < EVENT_TYPE_COUNT )
				{
					// this looks weird I know, but this calls the function we want
					// sure, we could use a switch statement, but if we add tons of
					// commands to our sequencer, this is a better way
					ms_aFP[nCommand](this,pTrack);
				}
				if( nCommand != EVENT_TYPE_TRACKEND )
				{
					++pTrack->m_iEDPL;
				}

				pEventData = *pTrack->m_iEDPL;
			}
		}
	}

#if 0
	// this is test code that plays instrument zero every 80 updates
	static int nCount = 0;
	nCount++;
	int offsetTime, releaseTime;
	offsetTime = ((int)CSyn_GetTickRate( m_CSynContext )) / 2;

	int pitch = 60;
	CSynFixedPoint frequency = CSynConvert_PitchToFrequency( ConvertIntegerToFixed( pitch ) );
	CSynFixedPoint amplitude = CSynConvert_VelocityToAmplitude( ConvertIntegerToFixed( 64 ) );

	CSynErr result;
	if( nCount > 80 )
	{
		TextBoxes_Output_Printf("Scheduled Beat at = %d\n", m_nWorkTick);
		InstrumentData_t *pInstrumentData = FindInstrumentData( 0 );
		if( pInstrumentData )
		{
			int nType = pInstrumentData->GetType();
			Voice_t *pVoice = MakeActiveVoice( nType );
			if( pVoice )
			{
				result = CSynVoice_SetSound( pVoice->m_SynVoice, pInstrumentData->GetCSynSound() );
				TEST_ERROR(result, "CSynVoice_SetSound");

				/* Schedule noteOn in immediate future. */
				result = CSynVoice_NoteOn( pVoice->m_SynVoice, m_nWorkTick, frequency, amplitude );
				TEST_ERROR(result, "CSynVoice_NoteOn");

				/* Schedule noteOff slightly later. */
				result = CSynVoice_NoteOff( pVoice->m_SynVoice, m_nWorkTick + offsetTime, &releaseTime );
				TEST_ERROR(result, "CSynVoice_NoteOn");
				pVoice->m_nReleaseTick = releaseTime + m_nWorkTick;
				TextBoxes_Output_Printf("WorkTick = %d\n", m_nWorkTick);
				TextBoxes_Output_Printf("ReleaseTime = %d\n", releaseTime);
			
			}
		}
		nCount = 0;
	}
#endif
	return true;
}

bool MusicSequencerSystem_t::Shutdown()
{
	// CSyn Shutdown
	//UnloadInstruments(); // also deletes the CSynSounds
	//UnloadSequences();

	/* Stop the engine when we are done. */
	if( m_CSynMix ) CSynMix_Delete( m_CSynMix ); /* Delete everything. */

	CSyn_StopEngine( m_CSynContext  );

	CSyn_DeleteContext( m_CSynContext );

	return true;
}

bool MusicSequencerSystem_t::LoadInstrument( InstrumentData_t *pInstrumentData )
{
	CSynErr result;
	CSynSound sound;
	AudioSample *pAudioSample;

	int nID = pInstrumentData->GetID();

	// only load if the nID is not already in the map
	if( m_IntToInstrumentDataPtrMap.find( nID ) == m_IntToInstrumentDataPtrMap.end() )
	{
		// load to appropriate CSyn architecture
		int nType = pInstrumentData->GetType();

		switch( nType )
		{
		case 0:
			/* Load sound template that we can use to make voices. */
			result = CSynSound_Load( m_CSynMix, CIRCTYPE_BASIC_SAMPLER, pInstrumentData->GetFileName().c_str(), &sound );
			TEST_ERROR(result, "CSynSound_Load");
			if( result < 0 ) return false;

			pInstrumentData->SetCSynSound( sound );

			pAudioSample = CSynSound_GetAudioSample( sound );
			pInstrumentData->SetSampleInfoPtr( pAudioSample );
			// some of the stuff available
			// pAudioSample->sampleRate;
			// pAudioSample->bitsPerSample;
			// pAudioSample->baseNote;
			// pAudioSample->detune;
			// pAudioSample->highNote;
			// pAudioSample->lowNote;
			// pAudioSample->highVelocity;
			// pAudioSample->lowVelocity;

			break;
		case 1:
			/* Load sound template that we can use to make voices. */
			result = CSynSound_Load( m_CSynMix, CIRCTYPE_AMPENV_SAMPLER, pInstrumentData->GetFileName().c_str(), &sound );
			TEST_ERROR(result, "CSynSound_Load");
			if( result < 0 ) return false;

			pInstrumentData->SetCSynSound( sound );

			pAudioSample = CSynSound_GetAudioSample( sound );
			pInstrumentData->SetSampleInfoPtr( pAudioSample );
			// some of the stuff available
			// pAudioSample->sampleRate;
			// pAudioSample->bitsPerSample;
			// pAudioSample->baseNote;
			// pAudioSample->detune;
			// pAudioSample->highNote;
			// pAudioSample->lowNote;
			// pAudioSample->highVelocity;
			// pAudioSample->lowVelocity;

			break;
		default:
			return false;
		}

		// put this nID in our map
		m_IntToInstrumentDataPtrMap[nID] = pInstrumentData;
	}

	return true;
}

void MusicSequencerSystem_t::SetSlowInstrumentEnvelope( int nID )
{
	CSynErr result;
	InstrumentData_t *pInstrumentData = FindInstrumentData( nID );
	if( pInstrumentData )
	{
		EnvelopeFrame frames[] = {
			{ 0.0, 0.0 }, // force it to start at zero so we get full attack
			{ 0.3, 1.0 }, // attack frame
			{ 0.2, 0.5 }, // decay and sustain here
			{ 1.0, 0.0 }  // slow release frame
		};
		CSynEnvelopeInfo     envInfo =
		{
			frames, sizeof(frames)/sizeof(EnvelopeFrame),
			2, 0,  /* Sustain before frames[2] */
			-1, 0,
			0
		};
		result = CSynSound_SetAmpEnvelope( pInstrumentData->GetCSynSound(), &envInfo );
		TEST_ERROR(result, "CSynSound_SetAmpEnvelope");
	}
}

InstrumentData_t *MusicSequencerSystem_t::FindInstrumentData( int nID )
{
	IntToInstrumentDataPtrMap_t::iterator iIIDPM;
	if( (iIIDPM = m_IntToInstrumentDataPtrMap.find( nID )) != m_IntToInstrumentDataPtrMap.end() )
	{
		return (*iIIDPM).second;
	}
	return 0;
}

bool MusicSequencerSystem_t::UnloadInstruments()
{
	// delete all from our map
	IntToInstrumentDataPtrMap_t::iterator iIIDPM;
	for( iIIDPM = m_IntToInstrumentDataPtrMap.begin(); iIIDPM != m_IntToInstrumentDataPtrMap.end(); ++iIIDPM )
	{
		delete (*iIIDPM).second;
	}
	// clear the pairs
	m_IntToInstrumentDataPtrMap.clear();
	return true;
}

bool MusicSequencerSystem_t::LoadSequence( SequenceData_t *pSequenceData )
{
	int nID = pSequenceData->GetID();

	// only load if the nID is not already in the map
	if( m_IntToSequenceDataPtrMap.find( nID ) == m_IntToSequenceDataPtrMap.end() )
	{
		if( !pSequenceData->Load( pSequenceData->GetFileName().c_str() ) )
		{
			return false;
		}
		// put this nID in our map
		m_IntToSequenceDataPtrMap[nID] = pSequenceData;

		return true;
	}

	return false;
}

void MusicSequencerSystem_t::SetSequenceMixes( SequenceData_t *pSequenceData, IntVectorVector_t *pTrackVolumeVectorVector )
{
	if( pSequenceData )
	{
		pSequenceData->m_pTrackVolumeVectorVector = pTrackVolumeVectorVector;
	}
}

SequenceData_t *MusicSequencerSystem_t::FindSequenceData( int nID )
{
	IntToSequenceDataPtrMap_t::iterator iISDPM;
	if( (iISDPM = m_IntToSequenceDataPtrMap.find( nID )) != m_IntToSequenceDataPtrMap.end() )
	{
		return (*iISDPM).second;
	}
	return 0;
}

bool MusicSequencerSystem_t::UnloadSequences()
{
	// delete all from our map
	IntToSequenceDataPtrMap_t::iterator iISDPM;
	for( iISDPM = m_IntToSequenceDataPtrMap.begin(); iISDPM != m_IntToSequenceDataPtrMap.end(); ++iISDPM )
	{
		delete (*iISDPM).second;
	}
	// clear the pairs
	m_IntToSequenceDataPtrMap.clear();
	return true;
}

bool MusicSequencerSystem_t::PlaySequence( int nID, int nMixID )
{
	SequenceData_t *pSequenceData = FindSequenceData( nID );
	if( !pSequenceData )
	{
		// if not found, return false
		return false;
	}

	Sequence_t *pSequence = MakeActiveSequence();
	if( !pSequence )
	{
		// if not available, return false
		return false;
	}

	pSequence->m_nID = nID;
	pSequence->m_pTrackVolumeVectorVector = pSequenceData->m_pTrackVolumeVectorVector;
	pSequence->m_Ppq = pSequenceData->GetPpq();
	pSequence->m_State.m_Tempo = pSequenceData->GetTempo();
	pSequence->m_TimeElapsedFrac = 0;
	pSequence->m_TimeStep = CalcTimeStep( pSequence->m_State.m_Tempo, pSequence->m_Ppq, MUSICSYS_UPDATES_PER_SECOND );

	TrackDataPtrList_t::iterator iTDPL;
	for( iTDPL = pSequenceData->m_TrackDataPtrList.begin(); iTDPL != pSequenceData->m_TrackDataPtrList.end(); ++iTDPL )
	{
		TrackData_t *pTrackData = *iTDPL;

		if( !pTrackData->IsTempoTrack() )
		{
			Track_t *pTrack = MakeActiveTrack();
			if( !pTrack ) break;
			pTrack->m_pOwner = pSequence;
			pTrack->m_TimeElapsedInt = 0;
			pTrack->m_iEDPL = pTrack->m_iEDPLStart = pTrackData->m_EventDataPtrList.begin();
			pTrack->m_iEDPLEnd = pTrackData->m_EventDataPtrList.end();
			pTrack->m_pInstrumentData = FindInstrumentData( 0 );
			pSequence->m_TrackPtrList.push_back( pTrack );
		}
	}

	pSequence->SetMix( nMixID );

	return false;
}

void MusicSequencerSystem_t::UpdateSequenceMix( int nID, int nMixID )
{

	SequencePtrList_t::iterator iSPL;
	for( iSPL = m_ActiveSequencePtrList.begin(); iSPL != m_ActiveSequencePtrList.end(); ++iSPL )
	{
		Sequence_t *pSequence = *iSPL;
		if( pSequence->m_nID == nID )
		{
			pSequence->SetTargetMix( nMixID );
		}
	}
}

void Track_AllNotesOff( MusicSequencerSystem_t *pMSS, Track_t *pTrack )
{
	CSynErr result;
	int releaseTime;

	// find all voices to send a note off to
	VoicePtrList_t::iterator iVPL;
	for( iVPL = pTrack->m_VoicePtrList.begin(); iVPL != pTrack->m_VoicePtrList.end(); ++iVPL )
	{
		Voice_t *pVoice = *iVPL;
		if( !pVoice->m_nReleaseTick )
		{
			result = CSynVoice_NoteOff( pVoice->m_SynVoice, pMSS->m_nWorkTick, &releaseTime );
			TEST_ERROR(result, "CSynVoice_NoteOff");

			pVoice->m_nReleaseTick = releaseTime + pMSS->m_nWorkTick;
			TextBoxes_Output_Printf( OUTPUTMODE_RELEASES, "NoteOff Key = %3d relTime = %d tick = %d\n",
				pVoice->m_nKey, releaseTime, pMSS->m_nWorkTick);
		}
	}

	TextBoxes_Output_Printf( OUTPUTMODE_EVENTS, "AllNotesOff WorkTick = %d\n", pMSS->m_nWorkTick);
}

void MusicSequencerSystem_t::StopSequences()
{
	SequencePtrList_t::iterator iSPL;
	iSPL = m_ActiveSequencePtrList.begin();
	while( iSPL != m_ActiveSequencePtrList.end() )
	{
		Sequence_t *pSequence = *iSPL;
		++iSPL;

		TrackPtrList_t::iterator iTPL;
		iTPL = pSequence->m_TrackPtrList.begin();
		while( iTPL != pSequence->m_TrackPtrList.end() )
		{
			Track_t *pTrack = *iTPL;
			++iTPL;

			VoicePtrList_t::iterator iVPL;
			iVPL = pTrack->m_VoicePtrList.begin();
			while( iVPL != pTrack->m_VoicePtrList.end() )
			{
				Voice_t *pVoice = *iVPL;
				++iVPL;

				if( !pVoice->m_nReleaseTick )
				{
					Track_AllNotesOff( this, pTrack );
					pTrack->m_VoicePtrList.remove( pVoice );
					FreeActiveVoice( pVoice );
				}
			}

			pSequence->m_TrackPtrList.remove( pTrack );
			FreeActiveTrack( pTrack );
		}

		FreeActiveSequence( pSequence );
	}
}

Sequence_t *MusicSequencerSystem_t::MakeActiveSequence()
{
	if( m_FreeSequencePtrList.size() > 0 )
	{
		Sequence_t *pSequence = m_FreeSequencePtrList.front();
		m_FreeSequencePtrList.pop_front();
		pSequence->m_bActive = true;
		m_ActiveSequencePtrList.push_back( pSequence );
		return pSequence;
	}
	return 0;
}

void MusicSequencerSystem_t::FreeActiveSequence( Sequence_t *pSequence )
{
	m_ActiveSequencePtrList.remove( pSequence );
	pSequence->m_bActive = false;
	m_FreeSequencePtrList.push_back( pSequence );
}

Track_t *MusicSequencerSystem_t::MakeActiveTrack()
{
	if( m_FreeTrackPtrList.size() > 0 )
	{
		Track_t *pTrack = m_FreeTrackPtrList.front();
		m_FreeTrackPtrList.pop_front();
		pTrack->m_bActive = true;
		m_ActiveTrackPtrList.push_back( pTrack );
		return pTrack;
	}
	return 0;
}

void MusicSequencerSystem_t::FreeActiveTrack( Track_t *pTrack )
{
	m_ActiveTrackPtrList.remove( pTrack );
	pTrack->m_bActive = false;
	m_FreeTrackPtrList.push_back( pTrack );
}

Voice_t *MusicSequencerSystem_t::MakeActiveVoice( int nVoiceArchitectureType )
{
	if( m_FreeVoicePtrListVector[nVoiceArchitectureType].size() > 0 )
	{
		Voice_t *pVoice = m_FreeVoicePtrListVector[nVoiceArchitectureType].front();
		m_FreeVoicePtrListVector[nVoiceArchitectureType].pop_front();
		pVoice->m_bActive = true;
		pVoice->m_nReleaseTick = 0;
		m_ActiveVoicePtrListVector[nVoiceArchitectureType].push_back( pVoice );
		return pVoice;
	}
	return 0;
}

void MusicSequencerSystem_t::FreeActiveVoice( Voice_t *pVoice )
{
	int nVoiceArchitectureType = pVoice->m_nVoiceArchitectureType;
	m_ActiveVoicePtrListVector[nVoiceArchitectureType].remove( pVoice );
	pVoice->m_bActive = false;
	m_FreeVoicePtrListVector[nVoiceArchitectureType].push_back( pVoice );
}

