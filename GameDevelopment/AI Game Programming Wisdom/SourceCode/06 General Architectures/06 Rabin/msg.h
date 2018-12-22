/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __MSG_H__
#define __MSG_H__

#include "global.h"


//Add new messages here
typedef enum { MSG_NULL,
			   MSG_Randomize,
			   MSG_Timeout,
			   MSG_ChangeState
} MSG_Name;


class MSG_Object
{
public:

	MSG_Object( void );
	MSG_Object( float deliveryTime, MSG_Name name, objectID sender, objectID receiver, int state = -1 );
	~MSG_Object( void ) {}

	MSG_Name GetMsgName( void )					{ return( m_Name ); }
	void SetMsgName( MSG_Name name )			{ m_Name = name; }

	objectID GetSender( void )					{ return( m_Sender ); }
	void SetSender( objectID sender )			{ m_Sender = sender; }

	objectID GetReceiver( void )				{ return( m_Receiver ); }
	void SetReceiver( objectID receiver )		{ m_Receiver = receiver; }

	int GetMsgState( void )						{ return( m_State ); }
	void SetMsgState( int state )				{ m_State = state; }

	float GetDeliveryTime( void )				{ return( m_DeliveryTime ); }
	void SetDeliveryTime( float time )			{ m_DeliveryTime = time; }

	bool IsDelivered( void )					{ return( m_Delivered ); }
	void SetDelivered( bool value )				{ m_Delivered = value; }

private:

	MSG_Name m_Name;		//Message name
	objectID m_Sender;		//Object that sent the message
	objectID m_Receiver;	//Object that will get the message
	int m_State;			//State in which the receiver is allowed get the message (-1 means any state)

	float m_DeliveryTime;	//Time at which to send the message
	bool m_Delivered;		//Whether the message has been delivered

};


#endif	// __MSG_H__