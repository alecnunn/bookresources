/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// Voice.h

#ifndef VOICE_H
#define VOICE_H

#include "Def.h"
#include "csynutil.h"

typedef int SynthVolume_t;
typedef int SynthPitch_t;
typedef int SynthPan_t;
typedef int SynthEffect_t;

class Track_t;

class VoiceState_t {
public:
    SynthVolume_t m_Volume;
    SynthPitch_t  m_Pitch;
    SynthPan_t    m_Pan;
    SynthEffect_t m_Effect;
};

class Voice_t {
public:
	bool          m_bActive;         // this voice is playing
	int           m_nReleaseTick;    // when active, 0 means note on, 1 means note off
    VoiceState_t  m_State;
    Track_t      *m_pOwner;
    char          m_nKey;
	char          m_nVelocity;
	int           m_nVoiceArchitectureType;
	CSynVoice     m_SynVoice;
};

typedef std::vector< Voice_t >    VoiceVector_t;
typedef std::vector< VoiceVector_t > VoiceVectorVector_t;
typedef std::list< Voice_t * >    VoicePtrList_t;
typedef std::vector< VoicePtrList_t >    VoicePtrListVector_t;

#endif