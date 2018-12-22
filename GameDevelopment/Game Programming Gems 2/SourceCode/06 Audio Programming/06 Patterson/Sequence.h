/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// Sequence.h

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "Def.h"
#include "Track.h"

class SequenceState_t {
public:
    Tempo_t  m_Tempo;
    Volume_t m_Volume;
};

class Sequence_t {
public:
	void SetTargetMix( int nMixID );
	void SetMix( int nMixID );

	int             m_nID;
	bool            m_bActive;          // this sequence is playing
    SequenceState_t m_State;
	TimeFrac_t      m_Ppq;              // parts per quarter note
	TimeFrac_t      m_TimeElapsedFrac;
    TimeFrac_t      m_TimeStep;
    TrackPtrList_t  m_TrackPtrList;
	IntVectorVector_t *m_pTrackVolumeVectorVector;
};

typedef std::vector< Sequence_t > SequenceVector_t;
typedef std::list< Sequence_t * > SequencePtrList_t;

#endif