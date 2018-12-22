/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// TrackData.h

#ifndef TRACKDATA_H
#define TRACKDATA_H

class EventData_t;

typedef std::list< EventData_t * > EventDataPtrList_t;

class TrackData_t
{
public:
	TrackData_t();
	~TrackData_t();

	bool Load( u8 *pData, int nTrack );
	bool IsTempoTrack();
	u32  GetTempo();
	void Print();

	void PushBackEvent( EventData_t *pEventData );

	EventDataPtrList_t m_EventDataPtrList;
private:
	void Init( u8 *m_pData, int nTrack );
	s32	 ReadVariableLengthQuantity();
	u8	 ReadU8();
	bool ReadNextEvent( u8 *pnType, s32 *pnTime, u8 *pnStatus, u8 **ppData, u32 *pnLength );

	u8 *m_pData;
    u8 *m_pCurrent;

    u8 m_nCurrentCommand;
    u8 m_nCurrentCommandLength;

	bool m_bTrackEnd;
	bool m_bIsTempoTrack;
	u32 m_nTempo;
};

#endif
