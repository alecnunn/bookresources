/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// Sequence.cpp

#include "StdAfx.h"
#include "Sequence.h"

void Sequence_t::SetTargetMix( int nMixID )
{
	if( m_pTrackVolumeVectorVector )
	{
		if( nMixID < m_pTrackVolumeVectorVector->size() )
		{
			IntVector_t &VolumeVector = (*m_pTrackVolumeVectorVector)[nMixID];

			int nCount = 0;
			TrackPtrList_t::iterator iTPL;
			for( iTPL = m_TrackPtrList.begin(); iTPL != m_TrackPtrList.end(); ++iTPL )
			{
				Track_t *pTrack = *iTPL;
				pTrack->m_TargetState.m_Volume = VolumeVector[nCount];
				pTrack->m_Interpolator.m_VolumeStep = 1;
				nCount++;
			}
		}
	}
}

void Sequence_t::SetMix( int nMixID )
{
	if( m_pTrackVolumeVectorVector )
	{
		if( nMixID < m_pTrackVolumeVectorVector->size() )
		{
			IntVector_t &VolumeVector = (*m_pTrackVolumeVectorVector)[nMixID];

			int nCount = 0;
			TrackPtrList_t::iterator iTPL;
			for( iTPL = m_TrackPtrList.begin(); iTPL != m_TrackPtrList.end(); ++iTPL )
			{
				Track_t *pTrack = *iTPL;
				pTrack->m_State.PlayInit();
				pTrack->m_State.m_Volume = VolumeVector[nCount];
				pTrack->m_TargetState.m_Volume = VolumeVector[nCount];
				pTrack->m_Interpolator.m_VolumeStep = 1;
				nCount++;
			}
		}
	}
}
