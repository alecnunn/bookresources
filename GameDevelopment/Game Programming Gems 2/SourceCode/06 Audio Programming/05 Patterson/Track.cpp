/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// Track.cpp

#include "StdAfx.h"
#include "Track.h"

TrackState_t::PlayInit()
{
    m_Volume = 64;
    m_PitchBend = 0;
    m_Pan = 64;
    m_Effect = 0;
}
