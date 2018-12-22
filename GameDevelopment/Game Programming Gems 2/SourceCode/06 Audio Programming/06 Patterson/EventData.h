/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// EventData.h

class EventData_t
{
public:
	EventData_t();
	~EventData_t();

	void Print();

	void SetEventType( u8 nEventType );
	void SetEventTime( s32 nEventTime );
	void SetEventStatus( u8 nEventStatus );
	void SetEventData( u8 *pnEventData, u32 nEventLength );

	void SetRuntimeCommand();
	u32  GetRuntimeCommand();
	u8   GetEventDataByte( int nByteIndex );

	s32 m_nEventTime;
private:
	void PrintEventDataAsText();
	void PrintEventDataAsBinary();

	u8 m_nEventType;
	u8 m_nEventStatus;
	u8 *m_pnEventData;
	u32 m_nEventLength;

	u32 m_nRuntimeCommand;
};
