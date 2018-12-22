/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#include "msg.h"


MSG_Object::MSG_Object( void )
{
	m_Name = MSG_NULL;
	m_Sender = INVALID_OBJECT_ID;
	m_Receiver = INVALID_OBJECT_ID;
	m_State = -1;
	m_DeliveryTime = 0.0f;
}


MSG_Object::MSG_Object( float deliveryTime, MSG_Name name, objectID sender, objectID receiver, int state )
{
	SetDeliveryTime( deliveryTime );
	SetMsgName( name );
	SetSender( sender );
	SetReceiver( receiver );
	SetMsgState( state );
	SetDelivered( false );
}

