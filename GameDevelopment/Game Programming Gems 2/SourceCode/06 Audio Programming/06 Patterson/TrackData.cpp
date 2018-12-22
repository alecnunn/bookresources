/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// TrackData.cpp

#include "StdAfx.h"
#include "EventData.h"
#include "TrackData.h"
#include "Portable.h"
#include "MIDIDef.h"

void TrackData_t::Init( u8 *pData, int nTrack )
{
	u8 aBuffer[4];
	u8 *pByte;
	u8 *pTrack;
	s32 nTrackLength;
	
	m_pData = pData;

	pTrack = m_pData;
	while (nTrack--)
    {
		pByte = pTrack;
		pByte += 4;
		memcpy(aBuffer,pByte,sizeof(aBuffer));
		nTrackLength = Portable_ConvertStringToS32(aBuffer)+8L;
		pTrack += nTrackLength;
    }

	pByte = pTrack;
	pByte += 4;

	//memcpy(aBuffer,pByte,sizeof(aBuffer));
	//nTrackLength = Portable_ConvertStringToS32(aBuffer);
	pByte += 4;

	m_pCurrent = pByte;
	m_nCurrentCommand = 0x90;
	m_nCurrentCommandLength = 2;
	m_bTrackEnd = false;
	m_bIsTempoTrack = false;
	m_nTempo = 120;
}

u8 TrackData_t::ReadU8()
{
	return(*m_pCurrent++);
}

static const int _CommandLength[8] = {2,2,2,2,1,1,2,0};

int _GetCommandLength(u8 nCmd)
{
	return (_CommandLength[(nCmd >> 4) & 7]);
}

bool TrackData_t::ReadNextEvent( u8 *pnType, s32 *pnTime, u8 *pnStatus, u8 **ppData, u32 *pnLength )
{
	u32 i;
	u8 nNewByte;
	u8 *pBuffer;
    static u8 aBuffer[4];
	
	// read delta-time
	*pnTime=ReadVariableLengthQuantity();
	
	// read command
	nNewByte = ReadU8();
	switch (nNewByte)
    {
	case 0xF0:
	case 0xF7:
        *pnType=MIDIDEF_SYSEXEVENT;
        *pnStatus = nNewByte;
        *pnLength = (u32)ReadVariableLengthQuantity();
        *ppData = m_pCurrent;
        m_pCurrent += *pnLength;
        break;
	case 0xFF:
        *pnType = MIDIDEF_METAEVENT;
        *pnStatus = ReadU8();
        *pnLength = (u32)ReadVariableLengthQuantity();
        *ppData = m_pCurrent;
        m_pCurrent += *pnLength;

        if (*pnStatus == MIDIDEF_METATEMPO)
		{
			u32 nTemp, nTempo;
			nTemp = ((*ppData)[0] << 16) | ((*ppData)[1] << 8) | ((*ppData)[2]);
			nTempo = 60000000L / nTemp;
			m_bIsTempoTrack = true;
			m_nTempo = nTempo;
		}

        if (*pnStatus == MIDIDEF_METAEOT)
		{
			m_bTrackEnd = true;
		}
        break;
	default:
        *pnType = MIDIDEF_MIDIEVENT;
        if (nNewByte & 0x80)
		{
			// running status
			m_nCurrentCommand = nNewByte;
            *pnStatus = m_nCurrentCommand;
            m_nCurrentCommandLength = _GetCommandLength(m_nCurrentCommand);
			*pnLength = m_nCurrentCommandLength;
            for (i=0, pBuffer = aBuffer; i < m_nCurrentCommandLength; i++)
			{
				*pBuffer++ = ReadU8();
			}
            *ppData = (u8 *)aBuffer;
		}
        else
		{
            *pnStatus = m_nCurrentCommand;
            *pnLength = m_nCurrentCommandLength;
            pBuffer = aBuffer;
            *pBuffer++ = nNewByte;
            for (i=1; i < m_nCurrentCommandLength; i++)
			{
				*pBuffer++ = ReadU8();
			}
            *ppData = (u8 *)aBuffer;
		}
        break;
    }
	
	return(!m_bTrackEnd);
}

s32 TrackData_t::ReadVariableLengthQuantity()
{
	s32 nValue;
	u8 nByte;
	
	if ((nValue = ReadU8()) & 0x80)
    {
		nValue &= 0x7F;
		do
		{
			nValue = (nValue << 7) + ((nByte = ReadU8()) & 0x7F);
        } while (nByte & 0x80);
    }
	return nValue;
}

TrackData_t::TrackData_t()
{
	m_pData = 0;
    m_pCurrent = 0;
    m_nCurrentCommand = 0;
    m_nCurrentCommandLength = 0;
	
	m_bTrackEnd = false;
}

TrackData_t::~TrackData_t()
{
	EventDataPtrList_t::iterator iEDL;

	for( iEDL = m_EventDataPtrList.begin(); iEDL != m_EventDataPtrList.end(); ++iEDL )
	{
		delete *iEDL;
	}
}

bool TrackData_t::Load( unsigned char *pData, int nTrack )
{
	bool bMore = true;

	Init( pData, nTrack );

	do
	{
		EventData_t *pEventData = new EventData_t;

		u8	nEventType;
		s32	nEventTime;
		u8	nEventStatus;
		u8  *pnEventData;
		u32	nEventLength;

		bMore = ReadNextEvent(&nEventType,
								  &nEventTime,
								  &nEventStatus,
								  &pnEventData,
								  &nEventLength);

		pEventData->SetEventType( nEventType );
		pEventData->SetEventTime( nEventTime );
		pEventData->SetEventStatus( nEventStatus );
		pEventData->SetEventData( pnEventData, nEventLength );

		// here we determine the runtime command ID for this event
		pEventData->SetRuntimeCommand();

		PushBackEvent( pEventData );

	} while( bMore );

	return true;
}

bool TrackData_t::IsTempoTrack()
{
	return m_bIsTempoTrack;
}

u32 TrackData_t::GetTempo()
{
	return m_nTempo;
}

void TrackData_t::Print()
{
	EventDataPtrList_t::iterator iEDL;

	printf( "  Track: \n" );

	for( iEDL = m_EventDataPtrList.begin(); iEDL != m_EventDataPtrList.end(); ++ iEDL )
	{
		EventData_t *pEventData = *iEDL;

		pEventData->Print();
	}
}

void TrackData_t::PushBackEvent( EventData_t *pEventData )
{
	m_EventDataPtrList.push_back( pEventData );
}
