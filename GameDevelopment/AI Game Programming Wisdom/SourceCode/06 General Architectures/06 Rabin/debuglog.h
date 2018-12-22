/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __DEBUGLOG_H__
#define __DEBUGLOG_H__

#include "statemch.h"
#include "global.h"
#include "singleton.h"
#include <list>

class LogEntry
{
public:
	
	LogEntry( void );
	~LogEntry( void ) {}

	objectID m_owner;
	bool m_handled;

	float m_timestamp;
	char m_statename[64];
	char m_eventmsgname[64];

	//msg only info
	objectID m_receiver;
	objectID m_sender;

};

class DebugLog : public Singleton <DebugLog>
{
public:

	DebugLog( void );
	~DebugLog( void );

	void LogStateMachineEvent( objectID id, MSG_Object * msg, char* statename, char* eventmsgname, bool handled ); 

private:

	std::list<LogEntry*> m_log;

};


#endif	// __DEBUGLOG_H__