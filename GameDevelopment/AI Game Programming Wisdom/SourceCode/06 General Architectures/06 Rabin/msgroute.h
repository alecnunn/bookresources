
/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __MSGROUTE_H__
#define __MSGROUTE_H__

#include "msg.h"
#include "global.h"
#include "singleton.h"
#include <list>


typedef std::list<MSG_Object*> MessageContainer;

class MsgRoute : public Singleton <MsgRoute>
{
public:

	MsgRoute( void );
	~MsgRoute( void );

	void DeliverDelayedMessages( void );
	void SendMsg( float delay, MSG_Name name, objectID receiver, objectID sender, int state );

private:

	MessageContainer m_delayedMessages;

	void RouteMsg( MSG_Object & msg );

};


#endif	// __MSGROUTE_H__