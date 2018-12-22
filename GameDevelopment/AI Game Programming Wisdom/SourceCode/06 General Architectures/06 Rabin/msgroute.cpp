/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#include "msgroute.h"
#include "gameobject.h"
#include "statemch.h"
#include "database.h"
#include "global.h"
#include "time.h"
#include "msg.h"



MsgRoute::MsgRoute( void )
{

}


MsgRoute::~MsgRoute( void )
{

}


void MsgRoute::SendMsg( float delay, MSG_Name name, objectID receiver, objectID sender, int state )
{

	if( delay <= 0.0f )
	{	//Deliver immediately
		MSG_Object msg( g_time.GetCurTime(), name, receiver, sender, state );
		RouteMsg( msg );
	}
	else
	{	//Check for duplicates - then store
		MessageContainer::iterator i;
		for( i=m_delayedMessages.begin(); i!=m_delayedMessages.end(); ++i )
		{
			if( (*i)->IsDelivered() == false &&
				(*i)->GetMsgName() == name &&
				(*i)->GetReceiver() == receiver &&
				(*i)->GetSender() == sender &&
				(*i)->GetMsgState() == state )
			{	//Already in list - don't add
				return;
			}
		}
		
		//Store in delivery list
		float deliveryTime = delay + g_time.GetCurTime();
		MSG_Object * msg = new MSG_Object( deliveryTime, name, receiver, sender, state );
		m_delayedMessages.push_back( msg );
	}

}


void MsgRoute::DeliverDelayedMessages( void )
{
	MessageContainer::iterator i;
	for( i=m_delayedMessages.begin(); i!=m_delayedMessages.end(); ++i )
	{
		float dtime = (*i)->GetDeliveryTime();
		float ctime = g_time.GetCurTime();
		if( (*i)->GetDeliveryTime() <= g_time.GetCurTime() )
		{	//Deliver and delete msg
			MSG_Object * msg = *i;
			RouteMsg( *msg );
			delete( msg );
			i = m_delayedMessages.erase( i );
		}
	}

}

void MsgRoute::RouteMsg( MSG_Object & msg )
{
	GameObject * object = g_database.Find( msg.GetReceiver() );

	if( object != 0 )
	{
		if( msg.GetMsgState() < 0 || 
			msg.GetMsgState() == object->GetStateMachine()->GetState() )
		{	//State was irrelevant or current state matches msg state (for msg scoping)
			msg.SetDelivered( true );
			object->GetStateMachine()->Process( EVENT_Message, &msg );
		}
	}

}







