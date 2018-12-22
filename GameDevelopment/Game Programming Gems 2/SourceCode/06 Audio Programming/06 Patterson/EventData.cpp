/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// EventData.cpp

#include "StdAfx.h"
#include "EventData.h"
#include "MIDIDef.h"
#include "EventCommand.h"

EventData_t::EventData_t()
{
}

EventData_t::~EventData_t()
{
	delete [] m_pnEventData;
}

void EventData_t::PrintEventDataAsText()
{
	printf( "\"" );
	for( int i=0; i<m_nEventLength; i++ )
	{
		printf( "%c", m_pnEventData[i] );
	}
	printf( "\"" );
}

void EventData_t::PrintEventDataAsBinary()
{
	printf( "Data: " );
	for( int i=0; i<m_nEventLength; i++ )
	{
		printf( "%02x ", m_pnEventData[i] );
	}
}

void EventData_t::SetRuntimeCommand()
{
	m_nRuntimeCommand = EVENT_TYPE_COUNT; // init to out of range value
	int nType;
	switch (m_nEventType)
	{
	case MIDIDEF_MIDIEVENT:
		//printf( "      Details: Midi Event: " );
		if( m_nEventStatus >= 0x80 )
		{
			nType = m_nEventStatus >> 4;
			switch( nType )
			{
			case 0x8:
				m_nRuntimeCommand = EVENT_TYPE_NOTEOFF;
				//printf( " ( NoteOff Key: %d Vel: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0x9:
				m_nRuntimeCommand = EVENT_TYPE_NOTEON;
				//printf( " ( NoteOn  Key: %d Vel: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0xa:
				//printf( " ( PolyKeyPressure Key: %d Press: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0xb:
				if( m_pnEventData[0] == 10 )
				{
					m_nRuntimeCommand = EVENT_TYPE_PAN;
				}
				//printf( " ( Controller: %d Val: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0xc:
				m_nRuntimeCommand = EVENT_TYPE_SETINSTRUMENT;
				//printf( " ( Program Change: %d )", m_pnEventData[0] );
				break;
			case 0xd:
				//printf( " ( ChannelPressure: %d )", m_pnEventData[0] );
				break;
			case 0xe:
				//printf( " ( PitchWheel: %d )", ((int)m_pnEventData[0] << 8) | ((int)m_pnEventData[1] & 0xff) );
				break;
			}
		}
		break;
	case MIDIDEF_SYSEXEVENT:
		//printf( "      Details: Sysex Event: " );
		break;
	case MIDIDEF_METAEVENT:
		//printf( "      Details: Meta Event: " );
		switch(m_nEventStatus)
		{
		case MIDIDEF_METASEQN:
			//printf( "METASEQN " );
			break;
		case MIDIDEF_METATEXT:
			//printf( "METATEXT " );
			break;
		case MIDIDEF_METACOPYR:
			//printf( "METACOPYR " );
			break;
		case MIDIDEF_METATNAME:
			//printf( "METATNAME " );
			break;
		case MIDIDEF_METAINAME:
			//printf( "METAINAME " );
			break;
		case MIDIDEF_METALYRIC:
			//printf( "METALYRIC " );
			break;
		case MIDIDEF_METAMARKER:
			//printf( "METAMARKER " );
			break;
		case MIDIDEF_METACUEPT:
			//printf( "METACUEPT " );
			break;
		case MIDIDEF_METACHANPFX:
			//printf( "METACHANPFX " );
			break;
		case MIDIDEF_METAEOT:
			m_nRuntimeCommand = EVENT_TYPE_TRACKEND;
			//printf( "METAEOT " );
			break;
		case MIDIDEF_METATEMPO:
			//nNum = (m_pnEventData[0] << 16) | (m_pnEventData[1] << 8) | (m_pnEventData[2]);
			//nTempo = 60000000L / nNum;
			//printf( "METATEMPO = %d ", nTempo );
			break;
		case MIDIDEF_METASMPTEOFF:
			//printf( "METASMPTEOFF " );
			break;
		case MIDIDEF_METATIMESIG:
			//printf( "METATIMESIG " );
			break;
		case MIDIDEF_METAKEYSIG:
			//printf( "METAKEYSIG " );
			break;
		case MIDIDEF_METASEQEVENT:
			//printf( "METASEQEVENT" );
			break;
		}
		break;
	}
}

u32 EventData_t::GetRuntimeCommand()
{
	return m_nRuntimeCommand;
}

u8 EventData_t::GetEventDataByte( int nByteIndex )
{
	return m_pnEventData[nByteIndex];
}

void EventData_t::Print()
{
	printf( "    Event: " );

	printf( "Time: %8d Type: 0x%02x Length: %4d Status: 0x%02x\n",
		    m_nEventTime, m_nEventType, m_nEventLength, m_nEventStatus );

	unsigned long nNum, nTempo;
	int nType;
	unsigned int i;

	switch (m_nEventType)
	{
	case MIDIDEF_MIDIEVENT:
		printf( "      Details: Midi Event: " );
		if( m_nEventStatus >= 0x80 )
		{
			nType = m_nEventStatus >> 4;
			switch( nType )
			{
			case 0x8:
				printf( " ( NoteOff Key: %d Vel: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0x9:
				printf( " ( NoteOn  Key: %d Vel: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0xa:
				printf( " ( PolyKeyPressure Key: %d Press: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0xb:
				printf( " ( Controller: %d Val: %d )", m_pnEventData[0], m_pnEventData[1] );
				break;
			case 0xc:
				printf( " ( Program Change: %d )", m_pnEventData[0] );
				break;
			case 0xd:
				printf( " ( ChannelPressure: %d )", m_pnEventData[0] );
				break;
			case 0xe:
				printf( " ( PitchWheel: %d )", ((int)m_pnEventData[0] << 8) | ((int)m_pnEventData[1] & 0xff) );
				break;
			}
		} else {
			printf( " ( Data: " );
			for( i=0; i<m_nEventLength; i++ )
			{
				printf( "%02x ", m_pnEventData[i] );
			}
			printf( ")" );
		}

		break;
	case MIDIDEF_SYSEXEVENT:
		printf( "      Details: Sysex Event: " );
		PrintEventDataAsBinary();
		break;
	case MIDIDEF_METAEVENT:
		printf( "      Details: Meta Event: " );
		switch(m_nEventStatus)
		{
		case MIDIDEF_METASEQN:
			printf( "METASEQN " );
			PrintEventDataAsText();
			break;
		case MIDIDEF_METATEXT:
			printf( "METATEXT " );
			PrintEventDataAsText();
			break;
		case MIDIDEF_METACOPYR:
			printf( "METACOPYR " );
			PrintEventDataAsText();
			break;
		case MIDIDEF_METATNAME:
			printf( "METATNAME " );
			PrintEventDataAsText();
			break;
		case MIDIDEF_METAINAME:
			printf( "METAINAME " );
			PrintEventDataAsText();
			break;
		case MIDIDEF_METALYRIC:
			printf( "METALYRIC " );
			PrintEventDataAsText();
			break;
		case MIDIDEF_METAMARKER:
			printf( "METAMARKER " );
			PrintEventDataAsBinary();
			break;
		case MIDIDEF_METACUEPT:
			printf( "METACUEPT " );
			PrintEventDataAsBinary();
			break;
		case MIDIDEF_METACHANPFX:
			printf( "METACHANPFX " );
			PrintEventDataAsBinary();
			break;
		case MIDIDEF_METAEOT:
			printf( "METAEOT " );
			break;
		case MIDIDEF_METATEMPO:
			nNum = (m_pnEventData[0] << 16) | (m_pnEventData[1] << 8) | (m_pnEventData[2]);
			nTempo = 60000000L / nNum;
			printf( "METATEMPO = %d ", nTempo );
			PrintEventDataAsBinary();
			break;
		case MIDIDEF_METASMPTEOFF:
			printf( "METASMPTEOFF " );
			PrintEventDataAsBinary();
			break;
		case MIDIDEF_METATIMESIG:
			printf( "METATIMESIG " );
			PrintEventDataAsBinary();
			break;
		case MIDIDEF_METAKEYSIG:
			printf( "METAKEYSIG " );
			PrintEventDataAsBinary();
			break;
		case MIDIDEF_METASEQEVENT:
			printf( "METASEQEVENT" );
			PrintEventDataAsBinary();
			break;
		default:
			printf( "      Details: Unknown MetaEvent" );
		}
		break;
	default:
		printf( "      Details: Unkown Event Type" );
	}
	printf( "\n" );

}

void EventData_t::SetEventType( u8 nEventType )
{
	m_nEventType = nEventType;
}

void EventData_t::SetEventTime( s32 nEventTime )
{
	m_nEventTime = nEventTime;
}

void EventData_t::SetEventStatus( u8 nEventStatus )
{
	m_nEventStatus = nEventStatus;
}

void EventData_t::SetEventData( u8 *pnEventData, u32 nEventLength )
{
	m_nEventLength = nEventLength;
	m_pnEventData = new u8[nEventLength];
	memcpy( m_pnEventData, pnEventData, nEventLength );
}


