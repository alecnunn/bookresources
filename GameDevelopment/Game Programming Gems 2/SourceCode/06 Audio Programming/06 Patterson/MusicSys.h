/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// MusicSys.h

#ifndef MUSICSYS_H
#define MUSICSYS_H

#include "Def.h"
#include "Sequence.h"
#include "Track.h"
#include "Voice.h"
#include "EventCommand.h"
#include "csyn.h"
#include "csynutil.h"

class MusicSequencerSystemConfig_t {
public:
	MusicSequencerSystemConfig_t();
	~MusicSequencerSystemConfig_t();

	// set the system maximum sequence count
	void SetSequenceCount( int nSequenceCount );
	int  GetSequenceCount();
	// set the system maximum track count
	void SetTrackCount( int nTrackCount );
	int  GetTrackCount();
	// set the system maximum voice architecture type count
	void SetVoiceArchitectureTypeCount( int nVoiceArchitectureTypeCount );
	int  GetVoiceArchitectureTypeCount();
	// set the system maximum voice count per architecture
	void SetVoiceCount( int nVoiceArchitectureType, int nVoiceCount );
	int  GetVoiceCount( int nVoiceArchitectureType );
private:
	int m_nSequenceCount;
	int m_nTrackCount;
	int m_nVoiceArchitectureTypeCount;  // number of voice architecture types
	IntVector_t m_VoiceCountVector;     // number of voices per voice architecture type
};

class MusicSequencerSystemState_t {
	bool m_bInitialized;
};

class SequenceData_t;
typedef std::map< int, SequenceData_t * > IntToSequenceDataPtrMap_t;

class InstrumentData_t;
typedef std::map< int, InstrumentData_t * > IntToInstrumentDataPtrMap_t;

class MusicSequencerSystem_t {
public:
	MusicSequencerSystem_t();
	~MusicSequencerSystem_t();

	bool Init( MusicSequencerSystemConfig_t &Config );
	bool AudioFrame();
	bool Shutdown();

	double GetCSynUsage();

	bool LoadInstrument( InstrumentData_t *pInstrumentData );
	void SetSlowInstrumentEnvelope( int nID );
	bool UnloadInstruments();

	bool LoadSequence( SequenceData_t *pSequenceData );
	void SetSequenceMixes( SequenceData_t *pSequenceData, IntVectorVector_t *pTrackVolumeVectorVector );
	bool PlaySequence( int nID, int nMixID );
	void UpdateSequenceMix( int nID, int nMixID );
	void StopSequences();
	bool UnloadSequences();

private:
	// system config
    MusicSequencerSystemConfig_t  m_Config;

	// system state
    MusicSequencerSystemState_t   m_State;

	// system update
	int m_nWorkTick;
	bool Update();

	void DisplayStats();
	void CheckForFinishedVoices();
	void PerformTargetStateChanges(); // interactive feature
	u32 CalcTimeStep( u16 qpm, u16 ppq, u16 ups );

	// resource allocation
	SequenceVector_t      m_SequenceVector;
	TrackVector_t         m_TrackVector;
	VoiceVectorVector_t   m_VoiceVectorVector; // voice vector per voice architecture type

	// resource assignment
    SequencePtrList_t     m_ActiveSequencePtrList;
    SequencePtrList_t     m_FreeSequencePtrList;
	Sequence_t *MakeActiveSequence();
	void FreeActiveSequence( Sequence_t *pSequence );

    TrackPtrList_t        m_ActiveTrackPtrList;
    TrackPtrList_t        m_FreeTrackPtrList;
	Track_t *MakeActiveTrack();
	void FreeActiveTrack( Track_t *pTrack );

	VoicePtrListVector_t  m_ActiveVoicePtrListVector; // active voice ptr list per voice architecture type
	VoicePtrListVector_t  m_FreeVoicePtrListVector; // free voice ptr list per voice architecture type
	Voice_t *MakeActiveVoice( int nVoiceArchitectureType );
	void FreeActiveVoice( Voice_t *pVoice );

	// sequence data registration
	IntToSequenceDataPtrMap_t  m_IntToSequenceDataPtrMap;
	SequenceData_t *FindSequenceData( int nID );
	// instrument data registration
	IntToInstrumentDataPtrMap_t  m_IntToInstrumentDataPtrMap;
	InstrumentData_t *FindInstrumentData( int nID );

	// event commands
	friend void NoteOff_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack );
	friend void NoteOn_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack );
	friend void SetPan_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack );
	friend void SetInstrument_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack );
	friend void TrackEnd_Function( MusicSequencerSystem_t *pMSS, Track_t *pTrack );

	// cleanup commands
	friend void Track_AllNotesOff( MusicSequencerSystem_t *pMSS, Track_t *pTrack );

	// this defines a type that is a function pointer
	typedef void (*EventFuncPtr_t)(MusicSequencerSystem_t *,Track_t *);

	// this is an array of function pointers
	static EventFuncPtr_t ms_aFP[EVENT_TYPE_COUNT];

	// audio exponential taper table
	static int ms_anAudioTaper[128];

	// private CSyn stuff
	CSynMix     m_CSynMix;
	CSynContext m_CSynContext;
};

#endif