/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: MsgDaemon.h
 *    Desc: defenition for the inter-object messaging
 *          system
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include <assert.h>
#include "MsgDaemon.h"

cMsgDaemon* cMsgDaemon::m_pGlobalMsgDaemon = NULL;

cMsgDaemon::cMsgDaemon()
{
}

cMsgDaemon::~cMsgDaemon()
{
	// iterate and destroy all the objects?
}



void cMsgDaemon::RegObject( objID id, iGameObject* pObj )
{
	/**
	 * We must have unique ID's for each object.
	 */
	assert( m_objectMap[ id ] == NULL );

	m_objectMap[ id ] = pObj;
}



void cMsgDaemon::UnRegObject( objID id )
{
	m_objectMap[ id ] = NULL;
}



uint cMsgDaemon::DeliverMessage( const sMsg& msg )
{
	// Make sure there isn't a bogus receiver
	assert( g_invalidID != msg.m_dest );

	iGameObject* pDest = m_objectMap[ msg.m_dest ];
	if( pDest == NULL )
	{
		char buff[1024];
		sprintf( buff, "Message %d sent to non-existent object %d",
			msg.m_type, msg.m_dest );
		throw cGameError( buff );
	}
	return pDest->ProcMsg( msg );
}



void DebPrint( char* first, ... )
{
	char buff[1024];

	va_list marker;

	va_start( marker, first );     /* Initialize variable arguments. */

	vsprintf( buff, first, marker );

	va_end( marker );              /* Reset variable arguments.      */

	// g_gameServer is the same for the server and the client.
	SendMessage( sMsg( msgPrintString, g_gameServer, buff ) );
}

void LogPrint( char* first, ... )
{
	char buff[1024];

	va_list marker;

	va_start( marker, first );     /* Initialize variable arguments. */

	vsprintf( buff, first, marker );

	va_end( marker );              /* Reset variable arguments.      */

	static bool bFirst = true;
	static FILE* fp;
	if( bFirst )
	{
		bFirst = false;

		if( IsClient() )
		{
			fp = fopen("clientLog.txt", "w" );
			fprintf( fp, "Log file for Client\n===============================\n" );
		}
		else
		{
			fp = fopen("serverLog.txt", "w" );
			fprintf( fp, "Log file for Server\n===============================\n" );
		}
	}
	fprintf( fp, buff );
	fprintf( fp, "\n" );
	fflush(fp);
}


cGameWorld* GetWorld()
{
	iGameObject* pObj = MsgDaemon()->Get( g_gameWorldID );
	return (cGameWorld*) pObj;
}


void ReliableSendToPlayer( cNetMessage& nMsg, objID player )
{
	sMsg msg;
	msg.m_pData = &nMsg;
	msg.m_i[1] = player;
	msg.m_type = msgSendNetMessageR;
	msg.m_dest = g_gameServer;
	SendMessage( msg );
}

void UnreliableSendToPlayer( cNetMessage& nMsg, objID player )
{
	sMsg msg;
	msg.m_pData = &nMsg;
	msg.m_i[1] = player;
	msg.m_type = msgSendNetMessageUR;
	msg.m_dest = g_gameServer;
	SendMessage( msg );
}

void UnreliableBroadcast( cNetMessage& nMsg )
{
	sMsg msg;
	msg.m_pData = &nMsg;
	msg.m_type = msgBroadcastUR;
	msg.m_dest = g_gameServer;
	SendMessage( msg );
}

void ReliableBroadcast( cNetMessage& nMsg )
{
	sMsg msg;
	msg.m_pData = &nMsg;
	msg.m_type = msgBroadcastR;
	msg.m_dest = g_gameServer;
	SendMessage( msg );
}

void ReliableBroadcastExcept( cNetMessage& nMsg, objID exception )
{
	sMsg msg;
	msg.m_pData = &nMsg;
	msg.m_i[1] = exception;
	msg.m_type = msgBroadcastRExcept;
	msg.m_dest = g_gameServer;
	SendMessage( msg );
}


