/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: LoginNM.h
 *    Desc: Login-oriented net messages.
 *          Grok NetMessages.h before you explore here.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _LOGINNM_H
#define _LOGINNM_H

#include "NetMessages.h"

//=================================================================

/**
 * Message: Login Request
 * ID: 1
 */
class cNM_LoginRequest 
: public cNetMessage
{
	char m_name[32];
public:

	cNM_LoginRequest( string clientName );
	cNM_LoginRequest(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};

//=================================================================

/**
 * Message: Login Acceptance
 * ID: 2
 */
class cNM_LoginAccepted 
: public cNetMessage
{
	int m_playerID; // the newly minted player ID.
public:

	cNM_LoginAccepted( int playerID );
	cNM_LoginAccepted(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};

/**
 * Message: Logout Notification
 * ID: 7
 */
class cNM_LogoutNotice 
: public cNetMessage
{
	int m_playerID; // ID of the player leaving
public:

	cNM_LogoutNotice( int playerID );
	cNM_LogoutNotice(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};


/**
 * Message: New Player notification
 * ID: 8
 */
class cNM_NewPlayer 
: public cNetMessage
{
	int m_entID;
	int m_score;
	char m_name[32];
	sEntState m_state;
public:

	cNM_NewPlayer( int id, int score, string clientName, sEntState state );
	cNM_NewPlayer(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};


#endif //_LOGINNM_H