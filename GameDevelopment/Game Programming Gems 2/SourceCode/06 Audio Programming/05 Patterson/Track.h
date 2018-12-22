/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// Track.h

#ifndef TRACK_H
#define TRACK_H

#include "Def.h"
#include "MusicTypes.h"
#include "Voice.h"
#include "TrackData.h"

class Sequence_t;
class InstrumentData_t;

class TrackState_t {
public:
    Volume_t    m_Volume;
    PitchBend_t m_PitchBend;
    Pan_t       m_Pan;
    Effect_t    m_Effect;

	PlayInit();
};

class Track_t {
public:
	bool                          m_bActive;         // this track is playing
    TrackState_t                  m_State;
	//TrackState_t                m_TargetState;     // interactive feature
	//TrackInterpolator_t         m_Interpolator;    // interactive feature
    Sequence_t                   *m_pOwner;
    TimeInt_t                     m_TimeElapsedInt;
	EventDataPtrList_t::iterator  m_iEDPLStart;
	EventDataPtrList_t::iterator  m_iEDPLEnd;
	EventDataPtrList_t::iterator  m_iEDPL;           // the current event pending
    InstrumentData_t             *m_pInstrumentData;
    VoicePtrList_t                m_VoicePtrList;
};

typedef std::vector< Track_t >    TrackVector_t;
typedef std::list< Track_t * >    TrackPtrList_t;

#endif